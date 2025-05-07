// #define _CRT_SECURE_NO_WARNINGS

#include "ending.h"

#include "../wrapper/input.h"
#include "../wrapper/image.h"
#include "../wrapper/text.h"

#include "../window.h"

#include <SDL2/SDL_mixer.h>

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

SDL_Renderer* sc_ending_renderer = NULL;

static char s8[63] = ""; // 21이 적절함

static AVFormatContext* fmt_ctx = NULL;
static struct SwsContext* sws_ctx = NULL;
static SDL_Texture* v_tex = NULL;

static AVBufferRef* hw_device_ctx = NULL;

static AVCodecContext* dec_ctx;
AVCodec* codec;

static int video_stream_idx;

static AVPacket* pkt;
static AVFrame* frame;
static AVFrame* yuv_frame;
static AVFrame* sw_frame;

static unsigned char* out_buffer;

static int v_img;
static int v_txt;

static Mix_Music* v_music;

static int E = 1;

static enum AVPixelFormat get_dxva2_format(AVCodecContext* ctx,
	const enum AVPixelFormat* pix_fmts)
{
	for (const enum AVPixelFormat* p = pix_fmts; *p != AV_PIX_FMT_NONE; p++) {
		if (*p == AV_PIX_FMT_DXVA2_VLD)
			return *p;
	}
	printf("DXVA2 pixel format not found.\n");
	return AV_PIX_FMT_NONE;
}

static void sc_ending_initialize(void) {
	//avformat_network_init();
	av_log_set_level(AV_LOG_ERROR);

	fmt_ctx = avformat_alloc_context();

	if (avformat_open_input(&fmt_ctx, u8"image/video/V.mp4", NULL, NULL) != 0) {
		printf("Couldn't open input stream.\n");
		return -1;
	}
	if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
		printf("Couldn't find stream information.\n");
		return -1;
	}

	video_stream_idx = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);

	if (video_stream_idx < 0) {
		printf("Didn't find a video stream.\n");
		return -1;
	}

	// 3) DXVA2 디바이스 생성
	int err = av_hwdevice_ctx_create(&hw_device_ctx,
		AV_HWDEVICE_TYPE_DXVA2,
		NULL, NULL, 0);
	if (err < 0) {
		printf("Failed to create DXVA2 device.\n");
		return -1;
	}

	// 2) 내부 AVHWFramesContext 포인터 얻기
	AVHWFramesContext* frames_ctx = (AVHWFramesContext*)(hw_device_ctx->data);

	// 3) DXVA2 픽셀 포맷 및 SW 복사 포맷 지정
	frames_ctx->format = AV_PIX_FMT_DXVA2_VLD;
	frames_ctx->sw_format = AV_PIX_FMT_NV12;  // 또는 필요 SW 포맷

	// 4) 영상 크기 지정 (디코더 컨텍스트에서 복사)
	frames_ctx->width = WINDOW_WIDTH;
	frames_ctx->height = WINDOW_HEIGHT;

	// 5) 풀 초기화 (GPU 메모리 풀 생성)
	if ((err = av_hwframe_ctx_init(hw_device_ctx)) < 0) {
		printf("Failed to initialize HW frame pool\n");
		return;
	}

	codec = avcodec_find_decoder_by_name("h264_dxva2");
	if (!codec) {
		// fallback 일반 디코더
		codec = avcodec_find_decoder(fmt_ctx->streams[video_stream_idx]->codecpar->codec_id);
	}

	dec_ctx = avcodec_alloc_context3(codec);

	// 코덱 파라미터 복사 - 최신 FFmpeg 방식
	if (avcodec_parameters_to_context(dec_ctx, fmt_ctx->streams[video_stream_idx]->codecpar) < 0) {
		printf("Could not copy codec parameters.\n");
		return -1;
	}
	dec_ctx->hw_device_ctx = av_buffer_ref(hw_device_ctx);
	dec_ctx->get_format = get_dxva2_format;

	if (avcodec_open2(dec_ctx, codec, NULL) < 0) {
		printf("Coult not open codec.\n");
		return -1;
	}

	frame = av_frame_alloc();
	yuv_frame = av_frame_alloc();

	out_buffer = (unsigned char*)av_malloc(
		av_image_get_buffer_size(AV_PIX_FMT_YUV420P, WINDOW_WIDTH, WINDOW_HEIGHT, 1));
	int rs = av_image_fill_arrays(yuv_frame->data, yuv_frame->linesize, out_buffer,
		AV_PIX_FMT_YUV420P, WINDOW_WIDTH, WINDOW_HEIGHT, 1);
	if (rs < 0) {
		printf("FUCK\n");
		return -1;
	}

	pkt = (AVPacket*)av_malloc(sizeof(AVPacket));

	//Output Info-----------------------------
	printf("--------------- File Information ----------------\n");
	av_dump_format(fmt_ctx, 0, u8"image/video/V.mp4", 0);
	printf("-------------------------------------------------\n");

	sws_ctx = sws_getContext(WINDOW_WIDTH, WINDOW_HEIGHT, AV_PIX_FMT_YUV420P,
		WINDOW_WIDTH, WINDOW_HEIGHT, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL,
		NULL);

	v_tex = SDL_CreateTexture(sc_ending_renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH,
		WINDOW_HEIGHT);

	v_img = -image_add_tex(v_tex, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, 1.0f, 1.0f, LEFT, TOP);
	sprintf_s(s8, 63, "%d %d", 0, 0);
	v_txt = -text_add_as(
		s8,
		PRETENDARDJPMEDIUM,
		8, 8,
		254, 254, 254, 254,
		0.4f, 0.4f, LEFT, TOP
	);

	v_music = Mix_LoadMUS("sound/bgm/V.ogg");
	if (v_music == NULL) {
		printf("Failed to load music file: %s\n", Mix_GetError());
		return 1;
	}

	Mix_FadeInMusic(v_music, 0, 5000);

	// while (!Mix_PlayingMusic()); // for synchronize

	sw_frame = av_frame_alloc();

	E = 1;
}

static void sc_ending_render(void) {
	static int ret, got_picture;
	got_picture = 1;

	static double delta_ms = 0;
	static int delta_samples = 0;

	if (!E) return;

	if (!fmt_ctx || !pkt || !dec_ctx || !frame || !yuv_frame) return;

	if(Mix_PausedMusic()) Mix_ResumeMusic();

	while (Mix_PausedMusic());

	if (av_read_frame(fmt_ctx, pkt) >= 0) {
		if (pkt->stream_index == video_stream_idx) {
			avcodec_send_packet(dec_ctx, pkt);
			ret = avcodec_receive_frame(dec_ctx, frame);
			//ret = avcodec_decode_video2(dec_ctx, frame, &got_picture, pkt);
			if (ret < 0) {
				printf("Decode Error.\n");
				sprintf_s(s8, 63, "ERR");
				text_content(v_txt, s8);
				return;
			}

			if (got_picture) {
				int64_t pts = pkt->pts;
				double seconds = pts * av_q2d(fmt_ctx->streams[video_stream_idx]->time_base);

				double delta = seconds - Mix_GetMusicPosition(v_music);

				if (delta > 0.06) {
					SDL_Delay((delta * 1000));
				}

				delta = seconds - Mix_GetMusicPosition(v_music);

				//sprintf_s(s8, 21, "%.2fs / %.2fs", Mix_GetMusicPosition(v_music), Mix_MusicDuration(NULL));
				sprintf_s(s8, 63, "%.2fs %.2fs (%.2fs %.2fms %d)", seconds, Mix_GetMusicPosition(v_music), delta, delta_ms / delta_samples, delta_samples);
				text_content(v_txt, s8);

				delta_ms += delta * 1000;
				delta_samples++;

				av_hwframe_transfer_data(sw_frame, frame, 0);

				sws_scale(sws_ctx, (const unsigned char* const*)sw_frame->data, sw_frame->linesize, 0,
					dec_ctx->height,
					yuv_frame->data, yuv_frame->linesize);

				SDL_UpdateYUVTexture(v_tex, &(SDL_Rect){ 0, 0, 1280, 720 },
					yuv_frame->data[0], yuv_frame->linesize[0],
					yuv_frame->data[1], yuv_frame->linesize[1],
					yuv_frame->data[2], yuv_frame->linesize[2]);

				//image_content(v_img, v_tex);
			}
			av_frame_unref(frame);
		}
		av_packet_unref(pkt);
	}
	else {
		screen_change("title");
	}

	if (input_is_keydown(SDLK_x)) {
		screen_change("title");
	}
}

static void sc_ending_music_free(void) {
	if (v_music) Mix_FreeMusic(v_music);
	Mix_HookMusicFinished(NULL);
}

static void sc_ending_dispose(void) {
	E = 0;

	//av_log_set_level(AV_LOG_DEBUG);

	av_freep(&out_buffer);
	//avcodec_free_context(&dec_ctx);
	sws_freeContext(sws_ctx);
	av_frame_free(&frame);
	av_frame_free(&yuv_frame);
	av_frame_free(&sw_frame);
	av_packet_free(&pkt);
	avcodec_free_context(&dec_ctx);
	avformat_close_input(&fmt_ctx);

	SDL_DestroyTexture(v_tex);

	Mix_FadeOutMusic(2000);
	Mix_HookMusicFinished(sc_ending_music_free);
}

static void sc_ending_focus_lost(void) {
	Mix_PauseMusic();
	while (!Mix_PausedMusic());
	E = 0;
}

static void sc_ending_focus_restore(void) {
	E = 1;
}

screen_t sc_ending = {
	"ending", sc_ending_initialize, sc_ending_render, sc_ending_dispose, sc_ending_focus_lost, sc_ending_focus_restore
};
