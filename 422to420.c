/*****************************************************************
File YUV422toYUV420.cpp

Utility for converting a sequence of frames, stored in a single
file in raw YUV422 format, to a single output file in which they are
stored in raw YUV420 format, by vertically filtering using a (1,3,3,1) filter
and subsampling.
This utility is a filter taking input on stdin and generating its
output on stdout.

Original author: Thomas Davies (based on filters by Tim Borer)
****************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char * argv[] ) {

    if (argc != 4) {
        fprintf(stderr, "\"422to420\" command line format is:\n");
        fprintf(stderr, "Argument 1: width (pixels) e.g. 720\n");
        fprintf(stderr, "Argument 2: height (lines) e.g. 576\n");
        fprintf(stderr, "Argument 3: number of frames e.g. 3\n");
        fprintf(stderr, "Example: YUV422toYUV420 <foo >bar 720 576 3\n");
        fprintf(stderr, "   converts 3 frames, of 720x576 pixels, from file foo to file bar\n");
        return EXIT_SUCCESS;
    }

    //Get command line arguments
    int width = atoi(argv[1]);
    int height = atoi(argv[2]);
    int frames = atoi(argv[3]);

    //Allocate memory for input and output buffers.
    const int FrameInSize = width*height*2; // YUYV 4:2:2 is 2bytes per pixel
    unsigned char *FrameIn =(unsigned char *)malloc(FrameInSize);
    const int YBufOutSize = height*width;
    unsigned char *YBufOut = (unsigned char *)malloc(YBufOutSize);
    const int UVBufOutSize = height*width/4;
    unsigned char *UBufOut = (unsigned char *)malloc(UVBufOutSize);
    unsigned char *VBufOut = (unsigned char *)malloc(UVBufOutSize);

    int frame;
    for (frame=0; frame < frames; ++frame) {

        //fprintf(stderr, "Processing frame %d\n", frame);

        //Read whole frame
        if (read(STDIN_FILENO, FrameIn, FrameInSize) < FrameInSize) {
            fprintf(stderr, "Error: failed to read frame %d\n", frame);
            return EXIT_FAILURE;
        }
    
        int x,y;
        for (x = 0, y = 0; x < FrameInSize; x+=2,y++) {
            YBufOut[y] = FrameIn[x];
        }
 
        int u = 0;
        for (y = 0; y < height; y+=2) {
            for (x = 0; x < width*2; x+=4) {
                UBufOut[u] = (FrameIn[1+x+y*width*2] + FrameIn[1+x+(y+1)*width*2])/2;
                VBufOut[u] = (FrameIn[3+x+y*width*2] + FrameIn[3+x+(y+1)*width*2])/2;
                u++;
            }
        }

        if (write(STDOUT_FILENO, YBufOut, YBufOutSize) < YBufOutSize) {
            fprintf(stderr, "Error: failed to write frame %d\n", frame);
            return EXIT_FAILURE;
        }
        if (write(STDOUT_FILENO, UBufOut, UVBufOutSize) < UVBufOutSize) {
            fprintf(stderr, "Error: failed to write frame %d\n", frame);
            return EXIT_FAILURE;
        }
        if (write(STDOUT_FILENO, VBufOut, UVBufOutSize) < UVBufOutSize) {
            fprintf(stderr, "Error: failed to write frame %d\n",frame);
            return EXIT_FAILURE;
        }

    }

    free(FrameIn);
    free(YBufOut);
    free(UBufOut);
    free(VBufOut);
    return EXIT_SUCCESS;
}
