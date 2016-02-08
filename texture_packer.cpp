#include <iostream>
#include <vector>
#include "image.h"

using namespace std;

struct rect {
	int x, y;
	int w, h;

	void translate(int dx, int dy) {
		x += dx;
		y += dy;
	}
};

struct data {

	data(image *img, rect min, rect orig) : img(img), min(min), orig(orig) {}

	image *img;
	rect min, orig;
};

struct node {

	node(data *dat) : dat(dat) {}

	data* dat;

	node *down;
	node *right;
};

rect find_min_rect(const image *img) {
	int min_x = img->w, max_x = 0;
	int min_y = img->h, max_y = 0;

	int mask = 0xff000000;

	for(int y = 0; y < img->h; y++) {
		for(int x = 0; x < img->w; x++) {
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

	vector<data*> metadata;

	node *head;

	for(int i = 1; i < argc; i++) {
		
		image *img = new image(argv[i]);
		rect min = find_min_rect(img);
		rect orig = {0, 0, img->w, img->h};

		metadata.push_back(new data(img, min, orig));
	}

	
	for(int i = 0; i < metadata.size(); i++) {
		delete metadata[i]->img;
		delete metadata[i];
	}
}