/*
 * Copyright (c) 2010 Nicolas George
 * Copyright (c) 2011 Stefano Sabatini
 * Copyright (c) 2014 Andrey Utkin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * cropvid.c: Transcode an input video to an ouput video, with frame-by-frame
 * cropping and preserving frame timestamps.
 *
 * Adapted by Stephen Marquard <stephen.marquard@uct.ac.za> from FFmpeg example:
 * https://www.ffmpeg.org/doxygen/3.2/transcoding_8c-example.html
 */

#include <assert.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>

static AVFormatContext *ifmt_ctx;
static AVFormatContext *ofmt_ctx;

typedef struct StreamContext {
    AVCodecContext *dec_ctx;
    AVCodecContext *enc_ctx;
} StreamContext;

static StreamContext *stream_ctx;

typedef struct FrameCrop {
    int frame;
    int x;
    int y;
} FrameCrop;

static int open_input_file(const char *filename)
{
    int ret;
    unsigned int i;

    ifmt_ctx = NULL;
    if ((ret = avformat_open_input(&ifmt_ctx, filename, NULL, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
        return ret;
    }

    if ((ret = avformat_find_stream_info(ifmt_ctx, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
        return ret;
    }

    stream_ctx = av_mallocz_array(ifmt_ctx->nb_streams, sizeof(*stream_ctx));
    if (!stream_ctx)
        return AVERROR(ENOMEM);

    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        AVStream *stream = ifmt_ctx->streams[i];
        AVCodec *dec = avcodec_find_decoder(stream->codecpar->codec_id);
        AVCodecContext *codec_ctx;
        if (!dec) {
            av_log(NULL, AV_LOG_ERROR, "Failed to find decoder for stream #%u\n", i);
            return AVERROR_DECODER_NOT_FOUND;
        }
        codec_ctx = avcodec_alloc_context3(dec);
        if (!codec_ctx) {
            av_log(NULL, AV_LOG_ERROR, "Failed to allocate the decoder context for stream #%u\n", i);
            return AVERROR(ENOMEM);
        }
        ret = avcodec_parameters_to_context(codec_ctx, stream->codecpar);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Failed to copy decoder parameters to input decoder context "
                   "for stream #%u\n", i);
            return ret;
        }
        /* Reencode video */
        if (codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
            codec_ctx->framerate = av_guess_frame_rate(ifmt_ctx, stream, NULL);
            /* Open decoder */
            ret = avcodec_open2(codec_ctx, dec, NULL);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Failed to open decoder for stream #%u\n", i);
                return ret;
            }
        }
        stream_ctx[i].dec_ctx = codec_ctx;
    }

    av_dump_format(ifmt_ctx, 0, filename, 0);
    return 0;
}

static int open_output_file(const char *filename, int width, int height)
{
    AVStream *out_stream;
    AVStream *in_stream;
    AVCodecContext *dec_ctx, *enc_ctx;
    AVCodec *encoder;
    int ret;
    unsigned int i;

    ofmt_ctx = NULL;
    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, filename);
    if (!ofmt_ctx) {
        av_log(NULL, AV_LOG_ERROR, "Could not create output context\n");
        return AVERROR_UNKNOWN;
    }

    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        out_stream = avformat_new_stream(ofmt_ctx, NULL);
        if (!out_stream) {
            av_log(NULL, AV_LOG_ERROR, "Failed allocating output stream\n");
            return AVERROR_UNKNOWN;
        }

        in_stream = ifmt_ctx->streams[i];
        dec_ctx = stream_ctx[i].dec_ctx;

        if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {

            /* in this example, we choose transcoding to same codec */
            encoder = avcodec_find_encoder(dec_ctx->codec_id);
            if (!encoder) {
                av_log(NULL, AV_LOG_FATAL, "Necessary encoder not found\n");
                return AVERROR_INVALIDDATA;
            }
            enc_ctx = avcodec_alloc_context3(encoder);
            if (!enc_ctx) {
                av_log(NULL, AV_LOG_FATAL, "Failed to allocate the encoder context\n");
                return AVERROR(ENOMEM);
            }

            if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
		// Set output parameters the same as input params
                enc_ctx->height = height;
                enc_ctx->width = width;
                enc_ctx->sample_aspect_ratio = dec_ctx->sample_aspect_ratio;
                enc_ctx->pix_fmt = dec_ctx->pix_fmt;
                enc_ctx->time_base = dec_ctx->time_base;
                enc_ctx->pkt_timebase = dec_ctx->pkt_timebase;
                enc_ctx->framerate = dec_ctx->framerate;

                // Quality

		// Ignored by libx264
                // https://lists.ffmpeg.org/pipermail/ffmpeg-cvslog/2014-March/075524.html
                // enc_ctx->flags |= CODEC_FLAG_QSCALE;
                // enc_ctx->global_quality = FF_QP2LAMBDA * 23;

                // https://lists.ffmpeg.org/pipermail/libav-user/2015-April/008027.html
                ret = av_opt_set(enc_ctx->priv_data, "crf", "27", AV_OPT_SEARCH_CHILDREN);
                if (ret == AVERROR_OPTION_NOT_FOUND) {
		    av_log(NULL, AV_LOG_ERROR, "Encoding option crf not found (not an H264 encoder?)\n");
                }

		av_log(NULL, AV_LOG_DEBUG, "Output CTX timebase for stream %i is %i/%i\n", i, enc_ctx->time_base.num, enc_ctx->time_base.den);
            }

            /* Third parameter can be used to pass settings to encoder */
            ret = avcodec_open2(enc_ctx, encoder, NULL);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Cannot open video encoder for stream #%u\n", i);
                return ret;
            }
            ret = avcodec_parameters_from_context(out_stream->codecpar, enc_ctx);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Failed to copy encoder parameters to output stream #%u\n", i);
                return ret;
            }
            if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
                enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

            out_stream->time_base = in_stream->time_base;
            out_stream->r_frame_rate = in_stream->r_frame_rate;
            out_stream->avg_frame_rate = in_stream->avg_frame_rate;
            out_stream->start_time = in_stream->start_time;

            if (in_stream->duration > 0)
              out_stream->duration = in_stream->duration;

	    av_log(NULL, AV_LOG_DEBUG, "Output stream timebase for stream %i is %i/%i, duration is %li\n",
              i, out_stream->time_base.num, out_stream->time_base.den, out_stream->duration);

            stream_ctx[i].enc_ctx = enc_ctx;

        } else if (dec_ctx->codec_type == AVMEDIA_TYPE_UNKNOWN) {
            av_log(NULL, AV_LOG_FATAL, "Elementary stream #%d is of unknown type, cannot proceed\n", i);
            return AVERROR_INVALIDDATA;
        } else {
            /* if this stream must be remuxed */
            ret = avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Copying parameters for stream #%u failed\n", i);
                return ret;
            }
            out_stream->time_base = in_stream->time_base;
        }

    }
    av_dump_format(ofmt_ctx, 0, filename, 1);

    if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Could not open output file '%s'", filename);
            return ret;
        }
    }

    /* init muxer, write output file header */
    ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error occurred when opening output file\n");
        return ret;
    }

    return 0;
}

static int encode_write_frame(AVFrame *filt_frame, unsigned int stream_index, int *got_frame) {
    int ret;
    int got_frame_local;
    AVPacket enc_pkt;

    if (!got_frame)
        got_frame = &got_frame_local;

    if (filt_frame != NULL) {
      av_log(NULL, AV_LOG_DEBUG, "Encoding frame pts %li duration %li\n", filt_frame->pts, filt_frame->pkt_duration);
    }

    /* encode filtered frame */
    enc_pkt.data = NULL;
    enc_pkt.size = 0;
    av_init_packet(&enc_pkt);

    ret = avcodec_send_frame(stream_ctx[stream_index].enc_ctx, filt_frame);
    if (ret < 0) {
       if (filt_frame != NULL) {
         av_log(NULL, AV_LOG_INFO, "  encoder ret %i error %s sending frame\n", ret, av_err2str(ret));
         return ret;
       }
    }

    ret = avcodec_receive_packet(stream_ctx[stream_index].enc_ctx, &enc_pkt);

    if (ret < 0) {
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
          *got_frame = 0;
          return 0;
        } else {
          av_log(NULL, AV_LOG_INFO, "  encoder ret %i error %s receive packet\n", ret, av_err2str(ret));
          return ret;
        }
    }

    *got_frame = 1;

    /* prepare packet for muxing */
    enc_pkt.stream_index = stream_index;

    av_log(NULL, AV_LOG_DEBUG, "Muxing packet pts %li dts %li duration %li\n", enc_pkt.pts, enc_pkt.dts, enc_pkt.duration);

    /* mux encoded frame */
    ret = av_interleaved_write_frame(ofmt_ctx, &enc_pkt);

    return ret;
}

static int flush_encoder(unsigned int stream_index)
{
    int ret;
    int got_frame;

    if (!(stream_ctx[stream_index].enc_ctx->codec->capabilities &
                AV_CODEC_CAP_DELAY))
        return 0;

    while (1) {
        av_log(NULL, AV_LOG_DEBUG, "Flushing stream #%u encoder\n", stream_index);
        ret = encode_write_frame(NULL, stream_index, &got_frame);
        if (ret < 0)
            break;
        if (!got_frame)
            return 0;
    }
    return ret;
}

static AVFilterGraph *init_crop_filter(const AVFrame *in, int left, int top, int width, int height)
{
    AVFilterGraph *filter_graph = avfilter_graph_alloc();
    AVFilterInOut *inputs = NULL, *outputs = NULL;
    char args[512];
    int ret;

    snprintf(args, sizeof(args),
             "buffer=video_size=%dx%d:pix_fmt=%d:time_base=1/1:pixel_aspect=0/1[in];"
             "[in]crop=%d:%d:%d:%d[out];"
             "[out]buffersink",
             in->width, in->height, in->format,
             width, height, left, top);

    ret = avfilter_graph_parse2(filter_graph, args, &inputs, &outputs);
    if (ret < 0) return NULL;
    assert(inputs == NULL && outputs == NULL);

    ret = avfilter_graph_config(filter_graph, NULL);
    if (ret < 0) return NULL;

    av_log(NULL, AV_LOG_DEBUG, "Filter:\n%s\n", args);

    return filter_graph;
}

static AVFrame *crop_frame(const AVFrame *in, AVFilterGraph *filter_graph)
{
    AVFrame *f = av_frame_alloc();
    AVFilterContext *buffersink_ctx;
    AVFilterContext *buffersrc_ctx;
    int ret;

    assert(filter_graph != NULL);

    // Crop the frame

    buffersrc_ctx = avfilter_graph_get_filter(filter_graph, "Parsed_buffer_0");
    buffersink_ctx = avfilter_graph_get_filter(filter_graph, "Parsed_buffersink_2");
    assert(buffersrc_ctx != NULL);
    assert(buffersink_ctx != NULL);

    av_frame_ref(f, in);

    ret = av_buffersrc_add_frame(buffersrc_ctx, f);
    if (ret < 0) return NULL;

    ret = av_buffersink_get_frame(buffersink_ctx, f);
    if (ret < 0) return NULL;

    return f;
}

// Get next cropping data. Returns -1 for frame number on EOF

FrameCrop getCropInfo(FILE *cropfile) {

    char line[256];
    FrameCrop crop;

    crop.frame = -1;
    crop.x = 0;
    crop.y = 0;

    // frame x y
    if (fgets(line, sizeof(line), cropfile) != NULL) {
      if (3 == sscanf(line, "%i %i %i", &crop.frame, &crop.x, &crop.y)) {
        av_log(NULL, AV_LOG_DEBUG, "Frame %i crop %i %i\n", crop.frame, crop.x, crop.y);
      }
    }

   return crop;
}

int main(int argc, char **argv)
{

    int ret;
    AVPacket packet = { .data = NULL, .size = 0 };
    AVFrame *frame = NULL;
    AVFilterGraph *crop_filter_graph = NULL;

    unsigned int stream_index;
    unsigned int i;
    int framecount;
    int cropframes;
    int pktcount;
    int eof;
    int got_frame;
    int found_keyframe;
    int pre_keyframe;
    int64_t pre_key_pts[128];
    int first_frame;
    int64_t first_pts, last_pts, current_pts, next_pts;

    int out_width, out_height;

    av_log(NULL, AV_LOG_INFO, "\ncropvid build %s %s\n", __DATE__, __TIME__);

    out_width = 1920;
    out_height = 1080;

    if (argc != 4) {
        av_log(NULL, AV_LOG_INFO, "\nUsage: %s <input file> <output file> <cropping file>\n\n", argv[0]);
        return 1;
    }

    // Cropping data
    FILE *cropfile = fopen(argv[3], "r"); /* should check the result */
    if (cropfile == NULL) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open cropping data file %s\n", argv[3]);
        goto end;
    }
    char line[256];
    fgets(line, sizeof(line), cropfile);

    // # track4k short.mkv 224 frames (frame top-left-x top-left-y) output frame size 1920 1080
    sscanf(line, "%*s %*s %*s %i %*s %*s %*s %*s %*s %*s %*s %i %i", &cropframes, &out_width, &out_height);

    av_log(NULL, AV_LOG_INFO, "\nCrop data: %i frames width %i height %i\n\n", cropframes, out_width, out_height);

    FrameCrop crop = getCropInfo(cropfile);
    FrameCrop next_crop = getCropInfo(cropfile);

    // Input and output videos
    av_register_all();
    avfilter_register_all();

    if ((ret = open_input_file(argv[1])) < 0)
        goto end;
    if ((ret = open_output_file(argv[2], out_width, out_height)) < 0)
        goto end;

    // Process
    framecount = 0;
    pktcount = 0;
    eof = 0;
    found_keyframe = 0;
    pre_keyframe = 0;
    first_frame = 1;
    first_pts = -1;
    last_pts = 0;
    current_pts = -1;
    next_pts = -1;

    /* read all packets */
    while (!eof) {

        if ((ret = av_read_frame(ifmt_ctx, &packet)) < 0) {
            av_log(NULL, AV_LOG_DEBUG, "Demuxer ret %i error %s for packet %u\n", ret, av_err2str(ret), pktcount);
            eof = 1;
        }

        stream_index = packet.stream_index;

        av_log(NULL, AV_LOG_DEBUG, "Demuxer gave packet %i of stream_index %u pts %li duration %li flags %i\n", 
          pktcount, stream_index, packet.pts, packet.duration, packet.flags);

        if (first_pts < 0) first_pts = packet.pts;
        if (packet.pts > last_pts) last_pts = packet.pts;

        // First keyframe
        if (packet.flags & AV_PKT_FLAG_KEY) {
            found_keyframe = 1;
        }

        // Record the packet PTS value
        if (!found_keyframe) {
          pre_key_pts[pre_keyframe++] = packet.pts;
        }

        pktcount++;

        if (stream_ctx[stream_index].dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {

            frame = av_frame_alloc();
            if (!frame) {
                ret = AVERROR(ENOMEM);
                av_log(NULL, AV_LOG_DEBUG, "  not a frame\n");
                break;
            }

            if (!eof) {
              ret = avcodec_send_packet(stream_ctx[stream_index].dec_ctx, &packet);
              if (ret < 0) {
                av_frame_free(&frame);
                av_log(NULL, AV_LOG_INFO, "Decoder send packet failed with ret err %s\n", av_err2str(ret));
                break;
              }
            } else {
              ret = avcodec_send_packet(stream_ctx[stream_index].dec_ctx, NULL);
            }

            ret = avcodec_receive_frame(stream_ctx[stream_index].dec_ctx, frame);

            if (ret < 0) {
                av_frame_free(&frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                  ret = 0;
                  av_log(NULL, AV_LOG_DEBUG, "  haven't got a frame\n");
                  av_frame_free(&frame);
                } else {
                  av_log(NULL, AV_LOG_INFO, "Decoding failed with ret err %s\n", av_err2str(ret));
                  break;
                }
            } else {
		if ((framecount % 300) == 0)
                    av_log(NULL, AV_LOG_INFO, "Frame %i pts %li\n", framecount, frame->pts);

		if (crop_filter_graph == NULL) {
		    crop_filter_graph = init_crop_filter(frame, crop.x, crop.y, out_width, out_height);
                }

                // New cropping data
                if ((framecount >= next_crop.frame) && (next_crop.frame > 0)) {
                    crop = next_crop;
                    next_crop = getCropInfo(cropfile);

                    // Recreate filter graph
                    avfilter_graph_free(&crop_filter_graph);
		    crop_filter_graph = init_crop_filter(frame, crop.x, crop.y, out_width, out_height);
                }

		AVFrame *cropped = crop_frame(frame, crop_filter_graph);

                // Write out frames for pre-keyframe packets to keep audio timesync for muxing
                if (first_frame && pre_keyframe) {
                    av_log(NULL, AV_LOG_INFO, "Writing %i frames from pts %li for pre-keyframe buffer\n", pre_keyframe, pre_key_pts[0]);
                    for (int i=0; i < pre_keyframe; i++) {
                      cropped->pts = pre_key_pts[i] - first_pts;
                      ret = encode_write_frame(cropped, stream_index, &got_frame);
                    }
                    first_frame = 0;
                }

                // Ensure that PTS timestamps are monotonic (always increase)
                next_pts = frame->pts;

                if (next_pts > current_pts) {
                    current_pts = next_pts;
                } else {
                    // Frame PTS has not incremented or gone backwards. This should never be the case except for corrupt media.
                    av_log(NULL, AV_LOG_ERROR, "Non-monotonic frame pts %li earlier than current pts %li : adjusting to %li\n", frame->pts, current_pts, current_pts+1);
                    current_pts++;
                    next_pts = current_pts;
                }

                cropped->pts = next_pts - first_pts;

                ret = encode_write_frame(cropped, stream_index, &got_frame);

                av_frame_free(&frame);
                av_frame_free(&cropped);

                framecount++;

                if (ret < 0)
                    goto end;
            }
        } else {
            if (!eof) {
              /* remux this packet without reencoding */
              av_log(NULL, AV_LOG_INFO, "  remuxing packet without encoding");
              av_packet_rescale_ts(&packet,
                                 ifmt_ctx->streams[stream_index]->time_base,
                                 ofmt_ctx->streams[stream_index]->time_base);

              ret = av_interleaved_write_frame(ofmt_ctx, &packet);
              if (ret < 0)
                  goto end;
              }
        }

        if (!eof) {
          av_packet_unref(&packet);
        }
    }

    /* flush filters and encoders */
    for (i = 0; i < ifmt_ctx->nb_streams; i++) {

        /* flush encoder */
        ret = flush_encoder(i);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Flushing encoder failed\n");
            goto end;
        }
    }

    av_write_trailer(ofmt_ctx);

end:
    av_packet_unref(&packet);
    av_frame_free(&frame);
    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        avcodec_free_context(&stream_ctx[i].dec_ctx);
        if (ofmt_ctx && ofmt_ctx->nb_streams > i && ofmt_ctx->streams[i] && stream_ctx[i].enc_ctx)
            avcodec_free_context(&stream_ctx[i].enc_ctx);
    }
    av_free(stream_ctx);
    avformat_close_input(&ifmt_ctx);
    if (ofmt_ctx && !(ofmt_ctx->oformat->flags & AVFMT_NOFILE))
        avio_closep(&ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);

    if (ret < 0)
        av_log(NULL, AV_LOG_ERROR, "Error occurred: %s\n", av_err2str(ret));

    fclose(cropfile);

    return ret ? 1 : 0;
}
