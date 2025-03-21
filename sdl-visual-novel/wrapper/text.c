#include "text.h"
#include "../window.h"

static text_t       texts[TEXT_CAPACITY];

static SDL_Texture* text_textures[TEXT_CAPACITY];
static SDL_Surface* surfaces[TEXT_CAPACITY];
static SDL_Rect     rects[TEXT_CAPACITY];

static TTF_Font*    fonts[FONT_COUNT];

static SDL_Renderer* renderer;

err_text_t text_init(SDL_Renderer* _renderer) {
	renderer = _renderer;

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
		if (!texts[i].text) return i;
	}

	return -1;
}

// 성공 시 (인덱스 * -1)을 반환
err_text_t text_add(const char* text, SDL_Color color, font_t font, int x, int y, int w, int h) {
	int index = text_able_index();

	if (index < 0) return CAPACITY_EXCEEDED;
	if (!text) return STRING_NULL;
	if (font < 0 || font >= FONT_COUNT) return INVALID_FONT;

	text_t tx = { font, color, text, x, y, w, h };
	SDL_Rect rt = { x, y, w, h };

	texts[index] = tx;
	rects[index] = rt;

	surfaces[index] = TTF_RenderUTF8_Blended_Wrapped(fonts[tx.font], tx.text, tx.color, w);
	if (!surfaces[index]) return TEXT_SURFACE_NULL;

	text_textures[index] = SDL_CreateTextureFromSurface(renderer, surfaces[index]);
	if (!text_textures[index]) return FONT_TEXTURE_NULL;

	return -index;
}

err_text_t text_add(const char* text, int r, int g, int b, font_t font, int x, int y) {
	SDL_Color color = { r, g, b };

	return text_add(text, color, font, x, y, WINDOW_WIDTH - x, WINDOW_HEIGHT - y);
}

err_text_t text_add(const char* text, int r, int g, int b, int a, font_t font, int x, int y) {
	SDL_Color color = { r, g, b, a };

	return text_add(text, color, font, x, y, WINDOW_WIDTH - x, WINDOW_HEIGHT - y);
}

err_text_t text_add(const char* text, int r, int g, int b, font_t font, int x, int y, int w, int h) {
	SDL_Color color = { r, g, b };

	return text_add(text, color, font, x, y, w, h);
}

err_text_t text_add(const char* text, int r, int g, int b, int a, font_t font, int x, int y, int w, int h) {
	SDL_Color color = { r, g, b, a };

	return text_add(text, color, font, x, y, w, h);
}

err_text_t text_remove(int index) {
	if (index < 0 || index >= TEXT_CAPACITY) return INVALID_INDEX;

	text_t tx = { texts[index].font, texts[index].color, NULL, -1, -1 }; // NULL 대신 쓰는 쓰레기 값

	SDL_FreeSurface(surfaces[index]);
	SDL_DestroyTexture(text_textures[index]);

	texts[index] = tx;
	text_textures[index] = NULL;
	surfaces[index] = NULL;
}

void text_render(void) {
	for (int i = 0; i < TEXT_CAPACITY; i++) {
		if (!texts[i].text) continue;

		SDL_RenderCopy(renderer, text_textures[i], NULL, rects + i);
	}
}
