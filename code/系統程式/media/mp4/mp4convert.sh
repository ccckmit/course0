ffmpeg -i test.mp4 -profile:v baseline -level 3.0 -pix_fmt yuv420p test_baseline.mp4
mv test_baseline.mp4 test.mp4