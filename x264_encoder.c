#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <x264.h>
 
static x264_param_t param;
static x264_picture_t pic;
static x264_picture_t pic_out;
static x264_t *h;
static int i_frame;
static int i_frame_size;
static x264_nal_t *nal;
static int i_nal;

int h264_init(int width, int height)
{
    /* Get default params for preset/tuning */
    if( x264_param_default_preset( &param, "medium", NULL ) < 0 )
        goto fail;

    /* Configure non-default params */
    param.i_csp = X264_CSP_I420;
    param.i_width  = width;
    param.i_height = height;
    param.b_vfr_input = 0;
    param.b_repeat_headers = 1;
    param.b_annexb = 1;

    /* Apply profile restrictions. */
    if( x264_param_apply_profile( &param, "high" ) < 0 )
        goto fail;
    if( x264_picture_alloc( &pic, param.i_csp, param.i_width, param.i_height ) < 0 )
        goto fail;
    h = x264_encoder_open( &param );
    if( !h ) {
        x264_picture_clean( &pic );
        goto fail;
    }
    return 0;
fail:
    return -1;
}

void h264_close()
{
    x264_encoder_close(h);
    x264_picture_clean(&pic);
}

int h264_encode(int width, int height, unsigned char *FrameIn, unsigned char **FrameOut)
{
    int luma_size = width * height;
    int chroma_size = luma_size / 4;
    memcpy(pic.img.plane[0], FrameIn, luma_size);
    memcpy(pic.img.plane[1], FrameIn + luma_size, chroma_size);
    memcpy(pic.img.plane[2], FrameIn + (luma_size+chroma_size), chroma_size);

    pic.i_pts = i_frame++;
    i_frame_size = x264_encoder_encode( h, &nal, &i_nal, &pic, &pic_out );
    if (i_frame_size)
        *FrameOut = nal->p_payload;
    else if(i_frame_size < 0)
        goto fail;
            
    /* Flush delayed frames */
    /*
    while( x264_encoder_delayed_frames( h ) )
    {
        i_frame_size = x264_encoder_encode( h, &nal, &i_nal, NULL, &pic_out );
        if( i_frame_size < 0 )
            goto fail;
        else if( i_frame_size )
        {
            if( !fwrite( nal->p_payload, i_frame_size, 1, stdout ) )
                goto fail;
        }
    }*/

    return i_frame_size;

fail:
    h264_close();
    return -1;
}
