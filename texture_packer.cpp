#include <iostream>
#include <cstring>
#include <cstdio>
#include <png.h>
#include <cstdint>

using namespace std;

struct rect {
	int x, y;
	int w, h;
};

bool load_png(char *filename, unsigned int &w, unsigned int &h, uint32_t *&pixel_data) {
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	int color_type, interlace_type;
	FILE *fp;

	if((fp = fopen(filename, "rb")) == NULL)
		return false;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if(!png_ptr) {
		fclose(fp);
		return false;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return false;
	}

	if(setjmp(png_jmpbuf(png_ptr))) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return false;
	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, sig_read);

	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);
	png_uint_32 width, height;
	int bit_depth;
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);
	w = width;
	h = height;

	pixel_data = new uint32_t[width * height];

	unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
	png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
	
	for(int i = 0; i < height; i++) {
		memcpy(pixel_data + (width * i), row_pointers[i], row_bytes);
	}

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(fp);

	return true;
}

int main(int argc, char **argv) {
	unsigned int w, h;
	uint32_t *pixel_data;

	cout << load_png(argv[1], w, h, pixel_data) << endl;

	cout << w << ", " << h << endl;
	cout << pixel_data[w * h - 1] << endl;
}