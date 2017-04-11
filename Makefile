all:
	gcc capture.c x264_encoder.c rtp.c -o capture -lx264 -lortp -Wall
