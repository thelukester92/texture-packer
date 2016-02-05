#include <iostream>
#include "image.h"

using namespace std;

struct rect {
	unsigned int x, y;
	unsigned int w, h;

	void translate(unsigned int dx, unsigned int dy) {
		x += dx;
		y += dy;
	}
};

rect find_min_rect(const image *img) {
	unsigned int min_x = img->w, max_x = 0;
	unsigned int min_y = img->h, max_y = 0;

	unsigned int mask = 0xff000000;

	for(unsigned int y = 0; y < img->h; y++) {
		for(unsigned int x = 0; x < img->w; x++) {
			if((mask & (img->pixels[x + y * img->w])) != 0) {
				if(x > max_x)
					max_x = x;
				if(x < min_x)
					min_x = x;
				if(y > max_y)
					max_y = y;
				if(y < min_y)
					min_y = y;
			}
		}
	}

	return {min_x, min_y, max_x - min_x, max_y - min_y};
}

int main(int argc, char **argv) {
	image *img;
	img = load_png(argv[1]);

	if(!img)
		return 1;

	rect min = find_min_rect(img);

	cout << min.x << " " << min.y << " " << min.w << " " << min.h << endl;
}