#include <iostream>
#include <cstring>
#include <cstdio>
#include <png.h>

using namespace std;

struct image {
	int *pixels;
	int w, h;

	image(char *filename);
	image(int w, int h);

	~image() {
		delete [] pixels;
	}

	void set_section(const image *sub, int x, int y);
};

image::image(char *filename) {
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	int color_type, interlace_type;
	FILE *fp;

	if((fp = fopen(filename, "rb")) == NULL)
		throw "unable to open file";

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if(!png_ptr) {
		fclose(fp);
		throw "file not recognized as png image";
	}

	info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		throw "unable to read data";
	}

	if(setjmp(png_jmpbuf(png_ptr))) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		throw "unable to set error check";
	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, sig_read);

	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);
	int bit_depth;
	png_uint_32 width, height;
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);
	w = width;
	h = height;
	pixels = new int[w * h];

	unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
	png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
	
	for(int i = 0; i < h; i++) {
		memcpy(pixels + (w * i), row_pointers[i], row_bytes);
	}

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(fp);
}

image::image(int w, int h) {
	this->w = w;
	this->h = h;

	pixels = new int[w * h];

	for(int i = 0; i < w * h; i++) {
		pixels[i] = 0;
	}
}

void image::set_section(const image* sub, int x, int y) {
	if(x + sub->w >= this->w || y + sub->h >= this->h || x < 0 || y <= 0)
		throw "unable to draw image section";

	for(int yp = 0; yp < sub->h; yp++) {
		for(int xp = 0; xp < sub->w; xp++) {
			this->pixels[(xp + x) + (yp + y) * this->w] = pixels[xp + yp * sub->w];
		}
	}
}