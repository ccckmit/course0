#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MINIMP4_IMPLEMENTATION
#include "minimp4.h"

// Set up for unity build of h264bsd
#include "h264bsd/src/basetype.h"
#include "h264bsd/src/h264bsd_decoder.h"
#include "h264bsd/src/h264bsd_byte_stream.c"
#include "h264bsd/src/h264bsd_cavlc.c"
#include "h264bsd/src/h264bsd_conceal.c"
#include "h264bsd/src/h264bsd_deblocking.c"
#include "h264bsd/src/h264bsd_decoder.c"
#include "h264bsd/src/h264bsd_dpb.c"
#include "h264bsd/src/h264bsd_image.c"
#include "h264bsd/src/h264bsd_inter_prediction.c"
#include "h264bsd/src/h264bsd_intra_prediction.c"
#include "h264bsd/src/h264bsd_macroblock_layer.c"
#include "h264bsd/src/h264bsd_nal_unit.c"
#include "h264bsd/src/h264bsd_neighbour.c"
#include "h264bsd/src/h264bsd_pic_order_cnt.c"
#include "h264bsd/src/h264bsd_pic_param_set.c"
#include "h264bsd/src/h264bsd_reconstruct.c"
#include "h264bsd/src/h264bsd_sei.c"
#include "h264bsd/src/h264bsd_seq_param_set.c"
#include "h264bsd/src/h264bsd_slice_data.c"
#include "h264bsd/src/h264bsd_slice_group_map.c"
#include "h264bsd/src/h264bsd_slice_header.c"
#include "h264bsd/src/h264bsd_storage.c"
#include "h264bsd/src/h264bsd_stream.c"
#include "h264bsd/src/h264bsd_transform.c"
#include "h264bsd/src/h264bsd_util.c"
#include "h264bsd/src/h264bsd_vlc.c"
#include "h264bsd/src/h264bsd_vui.c"

int read_callback(int64_t offset, void *buffer, size_t size, void *token) {
    FILE *f = (FILE*)token;
    fseeko(f, offset, SEEK_SET);
    return fread(buffer, 1, size, f) != size;
}

void save_ppm(storage_t *dec, const char *filename) {
    u32 picId, isIdrPic, numErrMbs;
    u32 *rgba = h264bsdNextOutputPictureRGBA(dec, &picId, &isIdrPic, &numErrMbs);
    if (!rgba) return;

    u32 croppingFlag, left, width, top, height;
    h264bsdCroppingParams(dec, &croppingFlag, &left, &width, &top, &height);
    if (!croppingFlag) {
        width = h264bsdPicWidth(dec) * 16;
        height = h264bsdPicHeight(dec) * 16;
    }

    FILE *f = fopen(filename, "wb");
    if (f) {
        fprintf(f, "P6\n%u %u\n255\n", width, height);
        u8 *p = (u8*)rgba;
        for (int i = 0; i < width * height; i++) {
            fwrite(p, 1, 3, f); // Write R, G, B (Byte 0, 1, 2)
            p += 4;
        }
        fclose(f);
        printf("Saved frame to %s\n", filename);
    } else {
        printf("Failed to open %s for writing\n", filename);
    }
}

void feed_decoder(storage_t *dec, u8 *data, u32 len, int target_frame, int *decoded_pics, const char *out_ppm) {
    while (len > 0) {
        u32 readBytes = 0;
        u32 result = h264bsdDecode(dec, data, len, 0, &readBytes);
        len -= readBytes;
        data += readBytes;

        if (result == H264BSD_PIC_RDY) {
            (*decoded_pics)++;
            if (*decoded_pics == target_frame) {
                save_ppm(dec, out_ppm);
                exit(0);
            }
            // Discard picture if not target
            u32 picId, isIdrPic, numErrMbs;
            h264bsdNextOutputPicture(dec, &picId, &isIdrPic, &numErrMbs);
        } else if (result == H264BSD_ERROR || result == H264BSD_PARAM_SET_ERROR) {
            fprintf(stderr, "Decode error: result=%d, readBytes=%u, remaining_len=%u\n", result, readBytes, len);
            if (readBytes == 0) {
                // To avoid infinite loop, manually jump over start code or skip data
                break;
            }
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage:\n  %s <input.mp4>\n  %s <input.mp4> <target_frame> <output.ppm>\n", argv[0], argv[0]);
        return 1;
    }

    const char *input_file = argv[1];
    FILE *infile = fopen(input_file, "rb");
    if (!infile) {
        printf("Failed to open %s\n", input_file);
        return 1;
    }

    fseeko(infile, 0, SEEK_END);
    int64_t filesize = ftello(infile);
    fseeko(infile, 0, SEEK_SET);

    MP4D_demux_t mp4 = {0};
    MP4D_open(&mp4, read_callback, infile, filesize);

    if (argc == 2) {
        printf("=== MP4 File Info ===\n");
        printf("Tracks: %u\n", mp4.track_count);
        for(int i = 0; i < mp4.track_count; i++) {
            printf("Track %d: ", i);
            if(mp4.track[i].handler_type == 0x76696465) {
                printf("Video, %u samples\n", mp4.track[i].sample_count);
            } else if(mp4.track[i].handler_type == 0x736F756E) {
                printf("Audio, %u samples\n", mp4.track[i].sample_count);
            } else {
                printf("Unknown, %u samples\n", mp4.track[i].sample_count);
            }
        }
        MP4D_close(&mp4);
        fclose(infile);
        return 0;
    }

    if (argc == 4) {
        int target_frame = atoi(argv[2]);
        const char *out_ppm = argv[3];

        int video_track = -1;
        for (int i = 0; i < mp4.track_count; i++) {
            if (mp4.track[i].handler_type == MP4D_HANDLER_TYPE_VIDE) {
                video_track = i;
                break;
            }
        }

        if (video_track == -1) {
            printf("No video track found\n");
            return 1;
        }

        storage_t dec;
        if (h264bsdInit(&dec, HANTRO_FALSE) != HANTRO_OK) {
            printf("h264bsdInit failed\n");
            return 1;
        }

        u8 start_code[4] = {0, 0, 0, 1};
        int decoded_pics = 0;

        // Feed SPS
        for (int i = 0; i < MINIMP4_MAX_SPS; i++) {
            int sps_bytes;
            const void *sps = MP4D_read_sps(&mp4, video_track, i, &sps_bytes);
            if (!sps) break;
            u8 *sps_buf = malloc(sps_bytes + 4);
            memcpy(sps_buf, start_code, 4);
            memcpy(sps_buf + 4, sps, sps_bytes);
            feed_decoder(&dec, sps_buf, sps_bytes + 4, target_frame, &decoded_pics, out_ppm);
            free(sps_buf);
        }

        // Feed PPS
        for (int i = 0; i < MINIMP4_MAX_PPS; i++) {
            int pps_bytes;
            const void *pps = MP4D_read_pps(&mp4, video_track, i, &pps_bytes);
            if (!pps) break;
            u8 *pps_buf = malloc(pps_bytes + 4);
            memcpy(pps_buf, start_code, 4);
            memcpy(pps_buf + 4, pps, pps_bytes);
            feed_decoder(&dec, pps_buf, pps_bytes + 4, target_frame, &decoded_pics, out_ppm);
            free(pps_buf);
        }

        // Feed Frames
        for (int i = 0; i < mp4.track[video_track].sample_count; i++) {
            unsigned frame_bytes, timestamp, duration;
            MP4D_file_offset_t offset = MP4D_frame_offset(&mp4, video_track, i, &frame_bytes, &timestamp, &duration);

            u8 *frame_data = malloc(frame_bytes);
            if (!frame_data) break;

            fseeko(infile, offset, SEEK_SET);
            fread(frame_data, 1, frame_bytes, infile);

            // Convert AVCC to Annex-B (replace length with start code)
            u32 pos = 0;
            while (pos + 4 <= frame_bytes) {
                u32 len = (frame_data[pos] << 24) | (frame_data[pos+1] << 16) | (frame_data[pos+2] << 8) | frame_data[pos+3];
                frame_data[pos] = 0;
                frame_data[pos+1] = 0;
                frame_data[pos+2] = 0;
                frame_data[pos+3] = 1;
                pos += 4 + len;
            }

            feed_decoder(&dec, frame_data, frame_bytes, target_frame, &decoded_pics, out_ppm);
            free(frame_data);
        }

        printf("Target frame %d not reached. Total decoded: %d\n", target_frame, decoded_pics);
        h264bsdShutdown(&dec);
    }

    MP4D_close(&mp4);
    fclose(infile);
    return 0;
}
