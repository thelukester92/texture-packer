#include <iostream>
#include <cstring>
#include <cstdio>
#include <png.h>

using namespace std;

struct rect {
	int x, y;
	int w, h;

	void translate(int dx, int dy) {
		x += dx;
		y += dy;
	}
};

struct image {
	int *pixels;
	int w, h;

	image(char *filename);
	image(int w, int h);
	void write(char *filename);

	~image() {
		delete [] pixels;
	}

	void set_section(const image *sub, int x, int y, rect min);
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

void image::write(char *filename) {
	png_structp png_ptr;
	png_infop info_ptr;
	FILE *fp;
	png_byte **row_pointers;

	if((fp = fopen(filename, "wb")) == NULL)
		throw "unable to open file";

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if(!png_ptr) {
		fclose(fp);
		throw "filed to write to file";
	}

	info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr) {
		fclose(fp);
		png_destroy_write_struct(&png_ptr, NULL);
		throw "failed to create info struct";
	}

	if(setjmp(png_jmpbuf(png_ptr))) {
		fclose(fp);
		png_destroy_write_struct(&png_ptr, &info_ptr);
		throw "unable to set error check";
	}

	png_set_IHDR(png_ptr, info_ptr, w, h, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	
	row_pointers = (png_byte**) png_malloc(png_ptr, h * sizeof(png_byte*));
	for(int y = 0; y < h; y++) {
		png_byte *row = (png_byte*) png_malloc(png_ptr, sizeof(int) * w);
		row_pointers[y] = row;
		for(int x = 0; x < w; x++) {
			int pix = pixels[x + y * w];

			*row++ = (pix >> 0 & 0xff);
			*row++ = (pix >> 8 & 0xff);
			*row++ = (pix >> 16 & 0xff);
			*row++ = (pix >> 24 & 0xff);
		}
	}

	png_init_io(png_ptr, fp);
	png_set_rows(png_ptr, info_ptr, row_pointers);
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	for(int y = 0; y < h; y++) {
		png_free(png_ptr, row_pointers[y]);
	}
	png_free(png_ptr, row_pointers);

	png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(fp);
}

void image::set_section(const image* sub, int x, int y, rect min) {
	for(int yp = 0; yp < min.h; yp++) {
		for(int xp = 0; xp < min.w; xp++) {
			this->pixels[(xp + x) + (yp + y) * this->w] = sub->pixels[(xp + min.x) + (yp + min.y) * sub->w];
		}
	}
}