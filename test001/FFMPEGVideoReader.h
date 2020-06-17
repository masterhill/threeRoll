#pragma once


extern "C"
{
#include <libavutil/imgutils.h>
#include <libavutil/parseutils.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/file.h>
}

class   FFMPEGVideoReader
{
public:
    AVFormatContext*pFormatCtx;
    int             i;
    int             videoindex;
    AVCodecContext* pCodecCtx;
    AVCodec*        pCodec;
    AVFrame*        pFrame;
    AVFrame*        pFrameRGB;
    SwsContext*     img_convert_ctx;
    int             screen_w;
    int             screen_h;
public:
    FFMPEGVideoReader()
    {
        av_register_all();
        avformat_network_init();
        pFormatCtx = avformat_alloc_context();
    }

    ~FFMPEGVideoReader()
    {
        sws_freeContext(img_convert_ctx);
        av_free(pFrameRGB);
        avcodec_close(pCodecCtx);
        avformat_close_input(&pFormatCtx);
    }

    int    load(const char* filepath = "c:/FlickAnimation.avi")
    {
        FILE *fp_yuv;
        int     ret;
        int     got_picture;

        if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) != 0) 
        {
            return -1;
        }
        if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
        {
            printf("Couldn't find stream information.\n");
            return -1;
        }
        videoindex = -1;
        for (i = 0; i < pFormatCtx->nb_streams; i++)
        {
            if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) 
            {
                videoindex = i;
                break;
            }
        }
        if (videoindex == -1) 
        {
            printf("Didn't find a video stream.\n");
            return -1;
        }
        pCodecCtx   =   pFormatCtx->streams[videoindex]->codec;
        pCodec      =   avcodec_find_decoder(pCodecCtx->codec_id);
        if (pCodec == NULL)
        {
            printf("Codec not found.\n");
            return -1;
        }
        if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) 
        {
            printf("Could not open codec.\n");
            return -1;
        }
        pFrame      =   av_frame_alloc();
        pFrameRGB   =   av_frame_alloc();

        screen_w    =   pCodecCtx->width;
        screen_h    =   pCodecCtx->height;

        img_convert_ctx = sws_getContext(
            pCodecCtx->width
            , pCodecCtx->height
            , pCodecCtx->pix_fmt
            , pCodecCtx->width
            , pCodecCtx->height
            , AV_PIX_FMT_RGB24
            , SWS_BICUBIC
            , NULL
            , NULL
            , NULL
            );

        if ((ret = av_image_alloc(dst_data, dst_linesize,
            pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB24, 1)) < 0)
        {
            fprintf(stderr, "Could not allocate destination image\n");
            return  -1;
        }
        return  0;
    }
    uint8_t *src_data[4], *dst_data[4];
    int src_linesize[4], dst_linesize[4];

    void*   readFrame()
    {
        AVPacket packet;
        av_init_packet(&packet);
        for (;;) 
        {
            if (av_read_frame(pFormatCtx, &packet)) 
            {
                av_free_packet(&packet);
                return 0;
            }


            if (packet.stream_index != videoindex) 
            {
                continue;
            }
            int frame_finished = 0;
            int res = avcodec_decode_video2(pCodecCtx, pFrame, &frame_finished, &packet);

            if (frame_finished)
            {

                int res = sws_scale(
                    img_convert_ctx
                    , (const uint8_t* const*)pFrame->data
                    , pFrame->linesize
                    , 0
                    , pCodecCtx->height
                    , dst_data
                    , dst_linesize
                    );
                av_free_packet(&packet);

                return  dst_data[0];
            }
        }

        return  0;
    }



};

