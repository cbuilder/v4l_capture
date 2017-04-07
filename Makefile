all:
	gcc capture.c x264_encoder.c -o capture -lx264 -Wall
