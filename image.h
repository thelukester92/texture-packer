#include <iostream>
#include <cstring>
#include <cstdio>
#include <png.h>

using namespace std;

struct image {
	unsigned int *pixels;
	unsigned int w, h;

	image(unsigned int w, unsigned int h) {
		this->w = w;
		this->h = h;
		pixels = new unsigned int[w * h];
	}
	~image() {
		delete [] pixels;
	}
};

image* load_png(char *filename) {
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	int color_type, interlace_type;
	FILE *fp;

	if((fp = fopen(filename, "rb")) == NULL)
		return NULL;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if(!png_ptr) {
		fclose(fp);
		return NULL;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return NULL;
	}

	if(setjmp(png_jmpbuf(png_ptr))) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return NULL;
	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, sig_read);

	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);
	unsigned int width, height;
	int bit_depth;
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);

	image *img = new image(width, height);

	unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
	png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
	
	for(int i = 0; i < height; i++) {
		memcpy(img->pixels + (width * i), row_pointers[i], row_bytes);
	}

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(fp);

	return img;
}
