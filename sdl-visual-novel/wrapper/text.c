#include "text.h"
#include "../misc.h"
#include "../window.h"
#include "../util.h"

static text_t       texts[TEXT_CAPACITY];

static SDL_Texture* text_textures[TEXT_CAPACITY];
static SDL_Surface* surfaces[TEXT_CAPACITY];
static SDL_Rect     rects[TEXT_CAPACITY];

static TTF_Font*    fonts[FONT_COUNT];

static SDL_Renderer* renderer;

const char* text_error_str(err_text_t err) {
	switch (err) {
		case INVALID_FONT: return "Invalid font.";
		case STRING_NULL: return "The string parameter is null. (or empty)";
		case FONT_TEXTURE_NULL: return "Font texture allocation failed.";
		case CAPACITY_EXCEEDED: return "Text capacity exceeded.";
		case INVALID_INDEX: return "Invalid index.";
		case FONT_LOAD_FAILED: return "Font load failed.";
		case TEXT_SURFACE_NULL: return "Text surface allocation failed.";

		default: return "Unknown error.";
	}
}

err_text_t text_init(SDL_Renderer* _renderer) {
	renderer = _renderer;

	memset(texts, 0, sizeof(text_t) * TEXT_CAPACITY);

	char ss[48] = "";
	for (int i = 0; i < FONT_COUNT; i++) {

		sprintf(ss, "font/%d.ttf", i);

		if (!(fonts[i] = TTF_OpenFont(ss, 60))) {
			return FONT_LOAD_FAILED;
		}
	}

	return 0;
}

void text_end(void) {
	for (int i = 0; i < FONT_COUNT; i++) {
		if (fonts[i]) TTF_CloseFont(fonts[i]);
	}
}

// 현재 텍스트 수 반환하는 함수
static int text_count(void) {
	int text_count = 0;

	for (int i = 0; i < TEXT_CAPACITY; i++) {
		if (texts[i].text) text_count++;
	}

	return text_count;
}

// 사용 가능한 인덱스 (중 맨 앞 인덱스)를 반환하는 함수
static int text_able_index(void) {
	for (int i = 0; i < TEXT_CAPACITY; i++) {
		if (!(texts[i].text)) return i;
	}

	return -1;
}

// 성공 시 (인덱스 * -1)을 반환
err_text_t text_add_o(const char* text, SDL_Color color, font_t font, int x, int y, int w) {
	int index = text_able_index();

	if (index < 0) return CAPACITY_EXCEEDED;
	if (!text) return STRING_NULL;
	if (font < 0 || font >= FONT_COUNT) return INVALID_FONT;

	if(!strlen(text)) return STRING_NULL;

	text_t tx = { font, color, text, x, y, w };
	SDL_Rect rt = { x, y, 0, 0 };

	texts[index] = tx;

	surfaces[index] = TTF_RenderUTF8_Blended_Wrapped(fonts[tx.font], tx.text, tx.color, w);
#if VERBOSE
	if (!surfaces[index])
		printf("SDL Error @ %s: %s\n", __func__, SDL_GetError());
#endif
	if (!surfaces[index]) return TEXT_SURFACE_NULL;

	text_textures[index] = SDL_CreateTextureFromSurface(renderer, surfaces[index]);
#if VERBOSE
	if (!text_textures[index])
		printf("SDL Error @ %s: %s\n", __func__, SDL_GetError());
#endif
	if (!text_textures[index]) return FONT_TEXTURE_NULL;

	iclamp(&(rt.w), surfaces[index]->w, w);
	rt.h = surfaces[index]->h;
	rects[index] = rt;

	return -index;
}

err_text_t text_add(const char* text, font_t font, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	SDL_Color color = { r, g, b, a };

	return text_add_o(text, color, font, x, y, WINDOW_WIDTH - x);
}

err_text_t text_add_r(const char* text, font_t font, int x, int y, int w, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	SDL_Color color = { r, g, b, a };

	return text_add_o(text, color, font, x, y, w);
}

err_text_t text_remove(int index) {
	if (index < 0 || index >= TEXT_CAPACITY) return INVALID_INDEX;

	if (!texts[index].text) return 0;

	text_t tx = { texts[index].font, texts[index].color, NULL, -1, -1 }; // NULL 대신 쓰는 쓰레기 값

	SDL_FreeSurface(surfaces[index]);
	SDL_DestroyTexture(text_textures[index]);

	texts[index] = tx;
	text_textures[index] = NULL;
	surfaces[index] = NULL;

	return 0;
}

void text_render(void) {
	for (int i = 0; i < TEXT_CAPACITY; i++) {
		if (!(texts[i].text)) continue;

		SDL_RenderCopy(renderer, text_textures[i], NULL, rects + i);
	}
}
