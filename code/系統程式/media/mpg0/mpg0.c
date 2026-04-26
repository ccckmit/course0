#include <stdio.h>
#include <stdlib.h>

// 宣告這行來實作 pl_mpeg 的內部函式
#define PL_MPEG_IMPLEMENTATION
#include "pl_mpeg.h"

// 功能 1：印出 MPG 基本資訊
void print_mpg_info(const char *in_file) {
    // 載入並解析 MPEG 標頭 (這個動作極快，不會解碼整個影片)
    plm_t *plm = plm_create_with_filename(in_file);
    if (!plm) {
        printf("無法開啟或解析檔案: %s (請確定這是一個 MPEG-1 檔案)\n", in_file);
        return;
    }

    // 檢查是否有影像串流 (修正: pl_mpeg 沒有 plm_has_video 函式)
    if (plm_get_num_video_streams(plm) == 0) {
        printf("此檔案沒有包含支援的 MPEG-1 影像串流。\n");
        plm_destroy(plm);
        return;
    }

    // 獲取基本資訊
    int width = plm_get_width(plm);
    int height = plm_get_height(plm);
    double framerate = plm_get_framerate(plm);
    double duration = plm_get_duration(plm); // 時長 (秒)
    int audio_streams = plm_get_num_audio_streams(plm);

    // 估算總影格數
    int total_frames = (int)(duration * framerate);

    printf("=== MPG 影片資訊 ===\n");
    printf("檔案名稱: %s\n", in_file);
    printf("解 析 度: %d x %d\n", width, height);
    printf("影 格 率: %.2f FPS\n", framerate);
    printf("影片時長: %.2f 秒\n", duration);
    if (total_frames > 0) {
        printf("總影格數: 約 %d 影格\n", total_frames);
    }
    
    if (audio_streams > 0) {
        int samplerate = plm_get_samplerate(plm);
        printf("音訊串流: %d 個 (取樣率: %d Hz)\n", audio_streams, samplerate);
    } else {
        printf("音訊串流: 無音訊\n");
    }
    printf("====================\n");

    plm_destroy(plm);
}

// 功能 2：抽出指定影格
void extract_frame(const char *in_file, int target_frame, const char *out_file) {
    if (target_frame < 0) {
        printf("錯誤：影格索引不能小於 0。\n");
        return;
    }

    plm_t *plm = plm_create_with_filename(in_file);
    if (!plm) {
        printf("無法開啟或解析檔案: %s\n", in_file);
        return;
    }

    // 檢查是否有影像串流 (修正)
    if (plm_get_num_video_streams(plm) == 0) {
        printf("此檔案沒有影片串流可供解碼。\n");
        plm_destroy(plm);
        return;
    }

    plm_frame_t *frame = NULL;
    int current_frame = 0;

    printf("正在解碼並尋找第 %d 個影格...\n", target_frame);

    // 逐影格解碼，直到抵達目標影格
    while (current_frame <= target_frame) {
        frame = plm_decode_video(plm);
        
        if (!frame) {
            printf("錯誤：找不到第 %d 個影格。影片可能已結束 (總共只找到 %d 個影格)。\n", target_frame, current_frame);
            break;
        }

        if (current_frame == target_frame) {
            break; // 找到了目標影格！
        }
        current_frame++;
    }

    // 若成功抓取到該影格
    if (frame) {
        int width = frame->width;
        int height = frame->height;
        
        // 分配 RGB 記憶體緩衝區
        uint8_t *rgb_data = (uint8_t *)malloc(width * height * 3);

        // 將 YCrCb 轉換為 RGB
        plm_frame_to_rgb(frame, rgb_data, width * 3);

        // 寫入無損的 PPM 影像檔
        FILE *f = fopen(out_file, "wb");
        if (f) {
            fprintf(f, "P6\n%d %d\n255\n", width, height);
            fwrite(rgb_data, 1, width * height * 3, f);
            fclose(f);
            
            printf("成功匯出第 %d 個影格至 %s (解析度: %dx%d)\n", target_frame, out_file, width, height);
        } else {
            printf("無法建立輸出檔案: %s\n", out_file);
        }

        free(rgb_data);
    }

    plm_destroy(plm);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("用法:\n");
        printf("  %s <input.mpg>                           # 印出 MPG 基本資訊\n", argv[0]);
        printf("  %s <input.mpg> <frame_index> <out.ppm>   # 抽出指定影格並儲存為 PPM\n", argv[0]);
        printf("範例:\n");
        printf("  %s test.mpg 200 test.ppm\n", argv[0]);
        return 1;
    }

    // 模式 1：只有輸入檔名，印出資訊
    if (argc == 2) {
        print_mpg_info(argv[1]);
    } 
    // 模式 2：包含檔名、影格數、輸出檔名，抽出影格
    else if (argc == 4) {
        int target_frame = atoi(argv[2]);
        extract_frame(argv[1], target_frame, argv[3]);
    } 
    else {
        printf("參數錯誤。請不帶參數執行以查看用法。\n");
    }

    return 0;
}