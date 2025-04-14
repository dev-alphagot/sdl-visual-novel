#include "image.h"
#include "../misc.h"

static image_t images[IMAGE_CAPACITY];
//static SDL_Texture* image_tex_orig[IMAGE_CAPACITY];

static SDL_Renderer* renderer;

err_image_t image_init(SDL_Renderer* _renderer) {
	if (!_renderer) return IMAGE_INVALID_RENDERER;

	renderer = _renderer;

	return 0;
}

const char* image_error_str(err_image_t err) {
	switch (err) {
		case IMAGE_LOAD_FAILED: return "Image load failed.";
		case IMAGE_INVALID_RENDERER: return "Invalid renderer.";
		case IMAGE_INVALID_PATH: return "Invalid path.";
		case IMAGE_CAPACITY_EXCEEDED: return "Image capacity exceeded.";
		case IMAGE_INVALID_INDEX: return "Invalid index.";
		case IMAGE_SURFACE_NULL: return "Image surface allocation failed.";
		default: return "";
	}
}

// 현재 텍스트 수 반환하는 함수
static int image_count(void) {
	int image_count = 0;

	for (int i = 0; i < IMAGE_CAPACITY; i++) {
		if (images[i].tex) image_count++;
	}

	return image_count;
}

// 사용 가능한 인덱스 (중 맨 앞 인덱스)를 반환하는 함수
static int image_able_index(void) {
	for (int i = 0; i < IMAGE_CAPACITY; i++) {
		if (!(images[i].tex)) return i;
	}

	return -1;
}

err_image_t image_add(
	const char* path,
	int x,
	int y,
	float scale_x,
	float scale_y,
	halign_t halign,
	valign_t valign
) {
	int index;

	if ((index = image_able_index()) < 0) return IMAGE_CAPACITY_EXCEEDED;

	if (!path) return IMAGE_INVALID_PATH;

	SDL_Surface* imageSurface;
	if(!(imageSurface = IMG_Load(path))) return IMAGE_INVALID_PATH;

	SDL_Texture* texture;
	if (!(texture = SDL_CreateTextureFromSurface(renderer, imageSurface))) return IMAGE_SURFACE_NULL;

	int w1 = (int)(ceil(imageSurface->w * scale_x));
	int h1 = (int)(ceil(imageSurface->h * scale_y));

	SDL_FreeSurface(imageSurface);

	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	SDL_Rect rc = {
		x - ((int)(ceil(w1 * (halign / 2.0f)))),
		y - ((int)(ceil(h1 * (valign / 2.0f)))),
		w1, h1
	};

	if (x == IMAGE_RECT_REUSE) {
		rc = images[index].rect;

		rc.x += ((int)(ceil(rc.w * (halign / 2.0f))));
		rc.y += ((int)(ceil(rc.h * (valign / 2.0f))));

		rc.x -= ((int)(ceil(w1 * (halign / 2.0f))));
		rc.y -= ((int)(ceil(h1 * (valign / 2.0f))));

		rc.w = w1;
		rc.h = h1;
	}

	image_t im = {
		texture, rc, scale_x, scale_y, halign, valign, x == IMAGE_RECT_REUSE ? images[index].color : (SDL_Color) { 255, 255, 255, 255 }
	};
	images[index] = im;

	return -index;
}

err_image_t image_add_tex(
	SDL_Texture* tex, int width, int height,
	int x,
	int y,
	float scale_x,
	float scale_y,
	halign_t halign,
	valign_t valign
) {
	int index;

	if ((index = image_able_index()) < 0) return IMAGE_CAPACITY_EXCEEDED;

	int w1 = (int)(ceil(width * scale_x));
	int h1 = (int)(ceil(height * scale_y));

	SDL_Rect rc = {
		x - ((int)(ceil(w1 * (halign / 2.0f)))),
		y - ((int)(ceil(h1 * (valign / 2.0f)))),
		w1, h1
	};

	if (x == IMAGE_RECT_REUSE) {
		rc = images[index].rect;

		rc.x += ((int)(ceil(rc.w * (halign / 2.0f))));
		rc.y += ((int)(ceil(rc.h * (valign / 2.0f))));

		rc.x -= ((int)(ceil(w1 * (halign / 2.0f))));
		rc.y -= ((int)(ceil(h1 * (valign / 2.0f))));

		rc.w = w1;
		rc.h = h1;
	}

	image_t im = {
		tex, rc, scale_x, scale_y, halign, valign, x == IMAGE_RECT_REUSE ? images[index].color : (SDL_Color) { 255, 255, 255, 255 }
	};
	images[index] = im;

	return -index;
}

err_image_t image_remove(int index) {
	if (index < 0 || index >= IMAGE_CAPACITY) return IMAGE_INVALID_INDEX;

	if(!(images[index].tex)) return IMAGE_INVALID_INDEX;

	SDL_DestroyTexture(images[index].tex);
	images[index].tex = NULL;
	
	return 0;
}

err_image_t image_remove_nofree(int index) {
	if (index < 0 || index >= IMAGE_CAPACITY) return IMAGE_INVALID_INDEX;

	if (!(images[index].tex)) return IMAGE_INVALID_INDEX;

	// SDL_DestroyTexture(images[index].tex);
	images[index].tex = NULL;

	return 0;
}

// 성공 시 (인덱스 * -1)을 반환. 원래 인덱스랑 같은 인덱스를 반환하지 않는 경우도 있음.
err_image_t image_content(int id, const char* new_path) {
	SDL_Surface* imageSurface;
	if (!(imageSurface = IMG_Load(new_path))) return IMAGE_INVALID_PATH;

	SDL_Texture* texture;
	if (!(texture = SDL_CreateTextureFromSurface(renderer, imageSurface))) return IMAGE_SURFACE_NULL;

	SDL_FreeSurface(imageSurface);

	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	if(images[id].tex) SDL_DestroyTexture(images[id].tex);
	images[id].tex = texture;

	return 0;
}

err_image_t image_move(int id, int x, int y) {
	if (id < 0 || id >= IMAGE_CAPACITY) return IMAGE_INVALID_INDEX;

	if (!(images[id].tex)) return IMAGE_INVALID_INDEX;

	images[id].rect.x += x;
	images[id].rect.y += y;

	return 0;
}

err_image_t image_pos(int id, int x, int y) {
	if (id < 0 || id >= IMAGE_CAPACITY) return IMAGE_INVALID_INDEX;

	if (!(images[id].tex)) return IMAGE_INVALID_INDEX;

	image_t im = images[id];

	images[id].rect.x = x;
	images[id].rect.y = y;

	images[id].rect.x -= ((int)(ceil(images[id].rect.w * (im.halign / 2.0f))));
	images[id].rect.y -= ((int)(ceil(images[id].rect.h * (im.valign / 2.0f))));

	return 0;
}

err_image_t image_scale(int id, float sx, float sy) {
	if (id < 0 || id >= IMAGE_CAPACITY) return IMAGE_INVALID_INDEX;

	if (!(images[id].tex)) return IMAGE_INVALID_INDEX;

	image_t im = images[id];

	printf("sc %d %d %f %f\n", im.rect.w, im.rect.h, im.scale_x, im.scale_y);

	int wl, hl;

	SDL_QueryTexture(images[id].tex, NULL, NULL, &wl, &hl);

	printf("wh %d %d\n", wl, hl);

	images[id].rect.x += ((int)(ceil(images[id].rect.w * (im.halign / 2.0f))));
	images[id].rect.y += ((int)(ceil(images[id].rect.h * (im.valign / 2.0f))));

	images[id].rect.w = wl * sx;
	images[id].rect.h = hl * sy;

	images[id].rect.x -= ((int)(ceil(images[id].rect.w * (im.halign / 2.0f))));
	images[id].rect.y -= ((int)(ceil(images[id].rect.h * (im.valign / 2.0f))));

	images[id].scale_x = sx;
	images[id].scale_y = sy;

	return 0;
}

err_image_t image_get_rect(int id, SDL_Rect* const rc) {
	if (id < 0 || id >= IMAGE_CAPACITY) return IMAGE_INVALID_INDEX;

	if (!(images[id].tex)) return IMAGE_INVALID_INDEX;

	memcpy(rc, &images[id].rect, sizeof(SDL_Rect));

	return 0;
}

err_image_t image_alpha(int id, uint8_t a) {
	if (id < 0 || id >= IMAGE_CAPACITY) return IMAGE_INVALID_INDEX;

	if (!(images[id].tex)) return IMAGE_INVALID_INDEX;
	images[id].color.a = a;

	return;

	int res = SDL_SetTextureAlphaMod(images[id].tex, a);

	if (res) {
		printf("SDL Error @ %s: %d / %s", __func__, res, SDL_GetError());
	}
}

err_image_t image_color(int id, uint8_t r, uint8_t g, uint8_t b) {
	if (id < 0 || id >= IMAGE_CAPACITY) return IMAGE_INVALID_INDEX;

	if (!(images[id].tex)) return IMAGE_INVALID_INDEX;
	images[id].color = (SDL_Color){ r, g, b, images[id].color.a };

	return;

	int res = SDL_SetTextureColorMod(images[id].tex, r, g, b);

	if (res) {
		printf("SDL Error @ %s: %d / %s", __func__, res, SDL_GetError());
	}
}

void image_render(void) {
	for (int i = 0; i < IMAGE_CAPACITY; i++) {
		if (!(images[i].tex)) continue;

		SDL_SetTextureColorMod(images[i].tex, images[i].color.r, images[i].color.g, images[i].color.b);
		SDL_SetTextureAlphaMod(images[i].tex, images[i].color.a);
		SDL_RenderCopy(renderer, images[i].tex, NULL, &images[i].rect);
	}
}

void image_end(void) {
	for (int i = 0; i < IMAGE_CAPACITY; i++) {
		if (!(images[i].tex)) continue;

		SDL_DestroyTexture(images[i].tex);
	}
}

void image_clear(void) {
	for (int i = 0; i < IMAGE_CAPACITY; i++) {
		image_remove(i);
	}
}
