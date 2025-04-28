#define _CRT_SECURE_NO_WARNINGS

#include <math.h>
#include "text.h"
#include "../misc.h"
#include "../window.h"
#include "../util.h"

static text_t       texts[TEXT_CAPACITY];

static SDL_Texture* text_textures[TEXT_CAPACITY];
static SDL_Surface* surfaces[TEXT_CAPACITY];
static SDL_Rect     rects[TEXT_CAPACITY];
static int          wrapCache[TEXT_CAPACITY];

static TTF_Font*    fonts[FONT_COUNT];

static SDL_Renderer* renderer;

const char* text_error_str(err_text_t err) {
	switch (err) {
		case TEXT_INVALID_FONT: return "Invalid font.";
		case TEXT_STRING_NULL: return "The string parameter is null. (not empty)";
		case TEXT_FONT_TEXTURE_NULL: return "Font texture allocation failed.";
		case TEXT_CAPACITY_EXCEEDED: return "Text capacity exceeded.";
		case TEXT_INVALID_INDEX: return "Invalid index.";
		case TEXT_FONT_LOAD_FAILED: return "Font load failed.";
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

		if (!(fonts[i] = TTF_OpenFont(ss, 48))) {
			return TEXT_FONT_LOAD_FAILED;
		}
		TTF_SetFontHinting(fonts[i], TTF_HINTING_LIGHT);
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

SDL_Texture* text_create_texture(
	const char* text1, SDL_Color color, font_t font,
	int w1,
	float sx, float sy
) {
	if (!text1) return TEXT_STRING_NULL;

	char* text = strlen(text1) ? text1 : " ";

	if (font < 0 || font >= FONT_COUNT) return TEXT_INVALID_FONT;

	int w2 = 0;

	char* ss = strdup(text);

	int lfc = 0;
	int mx = 0;

	for (int i = 0; i < strlen(ss); i++) {
		if (ss[i] == '\n') lfc++;
	}

	if (lfc == 0) {
		TTF_MeasureUTF8(fonts[font], text, WINDOW_WIDTH / sx, &w2, NULL);
	}
	else {
		for (int i = 0; i <= lfc; i++) {
			ss = strtok(i == 0 ? ss : NULL, "\n");

			int r = 0;

			TTF_MeasureUTF8(fonts[font], ss, WINDOW_WIDTH / sx, &r, NULL);

			r += r % 2;
			r += 48;

			if (mx < r) mx = r;
		}

		w2 = mx;
	}

	w2 = (int)(ceil(w2 * sx));

#if VERBOSE
	if (!surfaces[index])
		printf("SDL Info @ %s: %d %d\n", __func__, w2, (int)ceil(w2 / sx));
#endif

	int w = (w1 <= -1 ? w2 : (int)(ceil(w1 * sx)));

	//tx.w = w;
	//texts[index] = tx;
	SDL_Surface* sf = TTF_RenderUTF8_Blended_Wrapped(fonts[font], text, color, (int)ceil(w / sx));
#if VERBOSE
	if (!surfaces[index])
		printf("SDL Error @ %s: %s\n", __func__, SDL_GetError());
#endif
	if (!sf) return TEXT_SURFACE_NULL;

	SDL_Texture* tx = SDL_CreateTextureFromSurface(renderer, sf);
#if VERBOSE
	if (!text_textures[index])
		printf("SDL Error @ %s: %s\n", __func__, SDL_GetError());
#endif
	if (!tx) return TEXT_FONT_TEXTURE_NULL;

	SDL_SetSurfaceBlendMode(sf, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(tx, SDL_BLENDMODE_BLEND);

	int wx = w;
	int wy = (int)ceil(sf->h * sy);

#if VERBOSE
	printf("SDL Info @ %s: #%d %ld %ld %ld %ld (%d %d %d %f %f)\n", __func__, index, rt.x, rt.y, rt.w, rt.h, w2, surfaces[index]->w, surfaces[index]->h, sx, sy);
#endif

	SDL_FreeSurface(sf);

	return tx;
}

// 성공 시 (인덱스 * -1)을 반환
err_text_t text_add_o(
	const char* text1, SDL_Color color, font_t font, 
	int x, int y, int w1, 
	float sx, float sy, 
	halign_t ha, valign_t va
) {
	int index = text_able_index();

	if (!text1) return TEXT_STRING_NULL;

	char* text = strlen(text1) ? text1 : " ";

	if (index < 0) return TEXT_CAPACITY_EXCEEDED;
	if (font < 0 || font >= FONT_COUNT) return TEXT_INVALID_FONT;

	text_t tx = { font, color, text, x, y, w1, sx, sy, ha, va };
	SDL_Rect rt = { x, y, 0, 0 };

	texts[index] = tx;

	int w2 = 0;

	char* ss = strdup(tx.text);

	int lfc = 0;
	int mx = 0;

	for (int i = 0; i < strlen(ss); i++) {
		if (ss[i] == '\n') lfc++;
	}
	
	if (lfc == 0) {
		TTF_MeasureUTF8(fonts[tx.font], tx.text, WINDOW_WIDTH / sx, &w2, NULL);
	}
	else {
		for (int i = 0; i <= lfc; i++) {
			ss = strtok(i == 0 ? ss : NULL, "\n");

			int r = 0;

			TTF_MeasureUTF8(fonts[tx.font], ss, WINDOW_WIDTH / sx, &r, NULL);

			r += r % 2;
			r += 48;

			if (mx < r) mx = r;
		}

		w2 = mx;
	}
	
	w2 = (int)(ceil(w2 * sx));

#if VERBOSE
	if (!surfaces[index])
		printf("SDL Info @ %s: %d %d\n", __func__, w2, (int)ceil(w2 / sx));
#endif

	int w = (w1 <= -1 ? w2 : (int)(ceil(w1 * sx)));

	//tx.w = w;
	//texts[index] = tx;
	wrapCache[index] = (int)ceil(w / sx);
	surfaces[index] = TTF_RenderUTF8_Blended_Wrapped(fonts[tx.font], tx.text, tx.color, (int)ceil(w / sx));
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
	if (!text_textures[index]) return TEXT_FONT_TEXTURE_NULL;

	SDL_SetSurfaceBlendMode(surfaces[index], SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(text_textures[index], SDL_BLENDMODE_BLEND);

	int wx = w;
	int wy = (int)ceil(surfaces[index]->h * sy);

	if (ha != LEFT) rt.x -= (int)ceil(w * (ha / 2.0f));
	if (va != TOP) rt.y -= (int)ceil((surfaces[index]->h * sy) * (va / 2.0f));

	rt.w = w;
	rt.h = wy;
	rects[index] = rt;

#if VERBOSE
	printf("SDL Info @ %s: #%d %ld %ld %ld %ld (%d %d %d %f %f)\n", __func__, index, rt.x, rt.y, rt.w, rt.h, w2, surfaces[index]->w, surfaces[index]->h, sx, sy);
#endif

	return -index;
}

err_text_t text_add(const char* text, font_t font, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	SDL_Color color = { r, g, b, a };

	return text_add_o(text, color, font, x, y, -1, 1.0f, 1.0f, LEFT, TOP);
}

err_text_t text_add_a(const char* text, font_t font, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a, halign_t ha, valign_t va) {
	SDL_Color color = { r, g, b, a };

	return text_add_o(text, color, font, x, y, -1, 1.0f, 1.0f, ha, va);
}

err_text_t text_add_s(const char* text, font_t font, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a, float sx, float sy) {
	SDL_Color color = { r, g, b, a };

	return text_add_o(text, color, font, x, y, -1, sx, sy, LEFT, TOP);
}

err_text_t text_add_as(const char* text, font_t font, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a, float sx, float sy, halign_t ha, valign_t va) {
	SDL_Color color = { r, g, b, a };

	return text_add_o(text, color, font, x, y, -1, sx, sy, ha, va);
}

err_text_t text_add_r(const char* text, font_t font, int x, int y, int w, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	SDL_Color color = { r, g, b, a };

	return text_add_o(text, color, font, x, y, w, 1.0f, 1.0f, LEFT, TOP);
}

err_text_t text_remove(int index) {
	if (index < 0 || index >= TEXT_CAPACITY) return TEXT_INVALID_INDEX;

	if (!texts[index].text) return 0;

	text_t tx = { texts[index].font, texts[index].color, NULL, -1, -1 }; // NULL 대신 쓰는 쓰레기 값

	SDL_FreeSurface(surfaces[index]);
	SDL_DestroyTexture(text_textures[index]);

	texts[index] = tx;
	text_textures[index] = NULL;
	surfaces[index] = NULL;

	return 0;
}

// 성공 시 (인덱스 * -1)을 반환. 원래 인덱스랑 같은 인덱스를 반환하지 않는 경우도 있음.
err_text_t text_content(int id, const char* content) {
	text_t tx = texts[id];

	if(text_remove(id)) return TEXT_INVALID_INDEX;
	return text_add_o(
		content, tx.color, tx.font, tx.x, tx.y, tx.w, tx.scale_x, tx.scale_y, tx.halign, tx.valign
	);
}

// 성공 시 (인덱스 * -1)을 반환. 원래 인덱스랑 같은 인덱스를 반환하지 않는 경우도 있음.
err_text_t text_color(int id, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	if (id < 0 || id >= TEXT_CAPACITY) return TEXT_INVALID_INDEX;

	if (!texts[id].text) return TEXT_INVALID_INDEX;

	SDL_Color cc = { r, g, b, a };

	texts[id].color = cc;

	SDL_FreeSurface(surfaces[id]);
	SDL_DestroyTexture(text_textures[id]);

	surfaces[id] = TTF_RenderUTF8_Blended_Wrapped(fonts[texts[id].font], texts[id].text, cc, wrapCache[id]);
	text_textures[id] = SDL_CreateTextureFromSurface(renderer, surfaces[id]);

	return 0;
}

err_text_t text_move(int id, int x, int y) {
	if (id < 0 || id >= TEXT_CAPACITY) return TEXT_INVALID_INDEX;

	if (!(text_textures[id])) return TEXT_INVALID_INDEX;

	rects[id].x += x;
	rects[id].y += y;

	return 0;
}

err_text_t text_pos(int id, int x, int y) {
	if (id < 0 || id >= TEXT_CAPACITY) return TEXT_INVALID_INDEX;

	if (!(text_textures[id])) return TEXT_INVALID_INDEX;

	text_t im = texts[id];

	rects[id].x = x;
	rects[id].y = y;

	rects[id].x -= ((int)(ceil(rects[id].w * (im.halign / 2.0f))));
	rects[id].y -= ((int)(ceil(rects[id].h * (im.valign / 2.0f))));

	return 0;
}

err_text_t text_font(int id, font_t font) {
	if (id < 0 || id >= TEXT_CAPACITY) return TEXT_INVALID_INDEX;

	if (!(text_textures[id])) return TEXT_INVALID_INDEX;
	if (texts[id].font == font) return 0;

	texts[id].font = font;

	text_color(id, texts[id].color.r, texts[id].color.g, texts[id].color.b, texts[id].color.a);
	text_pos(id, texts[id].x, texts[id].y);

	return 0;
}

err_text_t text_get_rect(int id, SDL_Rect* const rc) {
	if (id < 0 || id >= TEXT_CAPACITY) return TEXT_INVALID_INDEX;

	if (!(text_textures[id])) return TEXT_INVALID_INDEX;

	memcpy(rc, rects + id, sizeof(SDL_Rect));

	return 0;
}

void text_render(void) {
	for (int i = 0; i < TEXT_CAPACITY; i++) {
		if (!(texts[i].text)) continue;

		SDL_RenderCopy(renderer, text_textures[i], NULL, rects + i);
	}
}

void text_clear(void) {
	for (int i = 0; i < TEXT_CAPACITY; i++) {
		text_remove(i);
	}
}
