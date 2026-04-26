/*
 * mpeg2image.c
 *
 * 從 MPEG 影片中抽出指定的第 N 張影像，存成 PNG 或 PPM 檔案。
 *
 * 編譯 (需要安裝 libavcodec / libavformat / libavutil / libswscale):
 *   gcc mpeg2image.c -o mpeg2image \
 *       $(pkg-config --cflags --libs libavcodec libavformat libavutil libswscale)
 *
 * 用法:
 *   ./mpeg2image <input.mpg> <frame_number> [output.ppm]
 *
 * 範例:
 *   ./mpeg2image movie.mpg 10          → 輸出 frame_0010.ppm
 *   ./mpeg2image movie.mpg 10 out.ppm  → 輸出 out.ppm
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

/* ------------------------------------------------------------------ */
/* 把 AVFrame (RGB24) 存成 PPM (Portable Pixmap) 檔案                  */
/* PPM 是最簡單、不需要額外函式庫的影像格式                             */
/* ------------------------------------------------------------------ */
static int save_ppm(AVFrame *frame, int width, int height,
                    const char *filename)
{
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "無法開啟輸出檔案 '%s': %s\n",
                filename, strerror(errno));
        return -1;
    }

    /* PPM header */
    fprintf(fp, "P6\n%d %d\n255\n", width, height);

    /* 逐行寫入像素 (R G B 各 1 byte) */
    for (int y = 0; y < height; y++) {
        fwrite(frame->data[0] + (size_t)y * frame->linesize[0],
               1, (size_t)width * 3, fp);
    }

    fclose(fp);
    return 0;
}

/* ------------------------------------------------------------------ */
/* 主程式                                                               */
/* ------------------------------------------------------------------ */
int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr,
                "用法: %s <input.mpg> <frame_number> [output.ppm]\n",
                argv[0]);
        fprintf(stderr,
                "範例: %s movie.mpg 10\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *input_file  = argv[1];
    int         target_frame = atoi(argv[2]);

    if (target_frame < 0) {
        fprintf(stderr, "frame_number 必須 >= 0\n");
        return EXIT_FAILURE;
    }

    /* 建立輸出檔名 */
    char output_file[512];
    if (argc >= 4) {
        snprintf(output_file, sizeof(output_file), "%s", argv[3]);
    } else {
        snprintf(output_file, sizeof(output_file),
                 "frame_%04d.ppm", target_frame);
    }

    /* ---- 開啟影片容器 -------------------------------------------- */
    AVFormatContext *fmt_ctx = NULL;
    if (avformat_open_input(&fmt_ctx, input_file, NULL, NULL) < 0) {
        fprintf(stderr, "無法開啟影片檔案: %s\n", input_file);
        return EXIT_FAILURE;
    }

    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        fprintf(stderr, "無法取得串流資訊\n");
        avformat_close_input(&fmt_ctx);
        return EXIT_FAILURE;
    }

    /* 印出基本資訊 */
    printf("輸入檔案  : %s\n", input_file);
    printf("目標影格  : %d\n", target_frame);
    printf("輸出檔案  : %s\n", output_file);
    av_dump_format(fmt_ctx, 0, input_file, 0);

    /* ---- 找到第一條視訊串流 -------------------------------------- */
    int video_stream_idx = -1;
    for (unsigned i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx = (int)i;
            break;
        }
    }
    if (video_stream_idx < 0) {
        fprintf(stderr, "找不到視訊串流\n");
        avformat_close_input(&fmt_ctx);
        return EXIT_FAILURE;
    }

    AVStream    *video_stream = fmt_ctx->streams[video_stream_idx];
    AVCodecParameters *codecpar = video_stream->codecpar;

    /* ---- 開啟解碼器 --------------------------------------------- */
    const AVCodec *codec = avcodec_find_decoder(codecpar->codec_id);
    if (!codec) {
        fprintf(stderr, "找不到對應的解碼器\n");
        avformat_close_input(&fmt_ctx);
        return EXIT_FAILURE;
    }

    AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) {
        fprintf(stderr, "無法配置解碼器 context\n");
        avformat_close_input(&fmt_ctx);
        return EXIT_FAILURE;
    }

    if (avcodec_parameters_to_context(codec_ctx, codecpar) < 0) {
        fprintf(stderr, "無法複製解碼器參數\n");
        goto cleanup_codec_ctx;
    }

    if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
        fprintf(stderr, "無法開啟解碼器\n");
        goto cleanup_codec_ctx;
    }

    int width  = codec_ctx->width;
    int height = codec_ctx->height;

    printf("影像大小  : %d x %d\n", width, height);
    printf("解碼器    : %s\n", codec->long_name ? codec->long_name : codec->name);

    /* ---- 配置 RGB 輸出 frame + 色彩空間轉換器 ------------------- */
    AVFrame *frame     = av_frame_alloc();   /* 原始解碼結果 (YUV 等) */
    AVFrame *frame_rgb = av_frame_alloc();   /* RGB24 輸出 */
    if (!frame || !frame_rgb) {
        fprintf(stderr, "av_frame_alloc 失敗\n");
        goto cleanup_codec_ctx;
    }

    int buf_size = av_image_get_buffer_size(AV_PIX_FMT_RGB24,
                                            width, height, 1);
    uint8_t *rgb_buf = (uint8_t *)av_malloc((size_t)buf_size);
    if (!rgb_buf) {
        fprintf(stderr, "av_malloc 失敗\n");
        goto cleanup_frames;
    }

    av_image_fill_arrays(frame_rgb->data, frame_rgb->linesize,
                         rgb_buf, AV_PIX_FMT_RGB24, width, height, 1);

    struct SwsContext *sws_ctx =
        sws_getContext(width, height, codec_ctx->pix_fmt,
                       width, height, AV_PIX_FMT_RGB24,
                       SWS_BILINEAR, NULL, NULL, NULL);
    if (!sws_ctx) {
        fprintf(stderr, "sws_getContext 失敗\n");
        goto cleanup_rgb_buf;
    }

    /* ---- 讀取封包、解碼、計數 ------------------------------------ */
    AVPacket *pkt = av_packet_alloc();
    if (!pkt) {
        fprintf(stderr, "av_packet_alloc 失敗\n");
        goto cleanup_sws;
    }

    int frame_count = 0;   /* 已解碼的影格數 (從 0 開始) */
    int found       = 0;
    int ret;

    while (av_read_frame(fmt_ctx, pkt) >= 0) {
        if (pkt->stream_index != video_stream_idx) {
            av_packet_unref(pkt);
            continue;
        }

        /* 傳送封包給解碼器 */
        ret = avcodec_send_packet(codec_ctx, pkt);
        av_packet_unref(pkt);
        if (ret < 0) {
            fprintf(stderr, "avcodec_send_packet 錯誤: %d\n", ret);
            break;
        }

        /* 從解碼器取出所有可用的影格 */
        while ((ret = avcodec_receive_frame(codec_ctx, frame)) == 0) {
            if (frame_count == target_frame) {
                /* 轉換色彩空間 YUV→RGB */
                sws_scale(sws_ctx,
                          (const uint8_t * const *)frame->data,
                          frame->linesize,
                          0, height,
                          frame_rgb->data,
                          frame_rgb->linesize);

                frame_rgb->width  = width;
                frame_rgb->height = height;

                if (save_ppm(frame_rgb, width, height, output_file) == 0) {
                    printf("✓ 已儲存第 %d 張影格 → %s\n",
                           target_frame, output_file);
                    found = 1;
                }
            }
            frame_count++;

            if (found) break;   /* 已找到，不需繼續解碼 */
        }

        if (found) break;
    }

    /* 沖出解碼器中殘留的影格 (drain) */
    if (!found) {
        avcodec_send_packet(codec_ctx, NULL);
        while (avcodec_receive_frame(codec_ctx, frame) == 0) {
            if (frame_count == target_frame) {
                sws_scale(sws_ctx,
                          (const uint8_t * const *)frame->data,
                          frame->linesize, 0, height,
                          frame_rgb->data, frame_rgb->linesize);
                frame_rgb->width  = width;
                frame_rgb->height = height;
                if (save_ppm(frame_rgb, width, height, output_file) == 0) {
                    printf("✓ 已儲存第 %d 張影格 → %s\n",
                           target_frame, output_file);
                    found = 1;
                }
            }
            frame_count++;
            if (found) break;
        }
    }

    if (!found) {
        fprintf(stderr,
                "✗ 找不到第 %d 張影格（影片共約 %d 張）\n",
                target_frame, frame_count);
    }

    av_packet_free(&pkt);
cleanup_sws:
    sws_freeContext(sws_ctx);
cleanup_rgb_buf:
    av_free(rgb_buf);
cleanup_frames:
    av_frame_free(&frame_rgb);
    av_frame_free(&frame);
cleanup_codec_ctx:
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&fmt_ctx);

    return found ? EXIT_SUCCESS : EXIT_FAILURE;
}