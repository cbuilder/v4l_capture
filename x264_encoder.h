
int h264_init(int width, int height);
void h264_close();
int h264_encode(int width, int height, unsigned char *FrameIn, unsigned char **FrameOut);
