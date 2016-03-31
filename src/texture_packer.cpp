#include <iostream>
#include <vector>
#include <algorithm>
#include <exception>
#include "image.h"

using namespace std;

struct data {

	data(image *img, rect min, rect orig) : img(img), min(min), orig(orig) {}

	image *img;
	rect min, orig;
};

bool compare(data *a, data *b) {
	return a->min.w * a->min.h < b->min.w * b->min.h;
}

int s_id = 0;

struct node {

	node() {
		dat = NULL;
		child[0] = NULL;
		child[1] = NULL;
		id = s_id++;
	}

	node(int w, int h) {
		r = {0, 0, w, h};
		dat = NULL;
		child[0] = NULL;
		child[1] = NULL;
		id = s_id++;
	}

	~node() {
		if(child[0])
			delete child[0];
		if(child[1])
			delete child[1];
	}

	int id;

	data* dat;
	rect r;

	node *child[2];
};

void print_node(node *n) {
	cout << "node " << n->id << ": {" << n->r.x << ", " << n->r.y << ", " << n->r.w << ", " << n->r.h << "}" << endl;
}

bool insert(node *n, data *dat) {
	if(!n->child[0] && !n->child[1]) {
		if(n->dat)
			return false;
		int dw = n->r.w - dat->min.w;
		int dh = n->r.h - dat->min.h;
		
		if(dw < 0 || dh < 0)
			return false;
		cout << "inserted data in node " << n->id << endl;
		n->dat = dat;
		if(dw == 0 && dh == 0) {
			return true;
		}
		n->child[0] = new node();
		n->child[1] = new node();
		if(dw > dh) {
			n->child[0]->r = {n->r.x + dat->min.w, n->r.y, dw, n->r.h};
			n->child[1]->r = {n->r.x, n->r.y + dat->min.h, dat->min.w, dh};
		} else {
			n->child[0]->r = {n->r.x + dat->min.w, n->r.y, dw, dat->min.h};
			n->child[1]->r = {n->r.x, n->r.y + dat->min.h, n->r.w, dh};
		}
		cout << "creating children nodes (" << n->child[0]->id << ", " << n->child[1]->id << ") for node " << n->id << endl;
		print_node(n->child[0]);
		print_node(n->child[1]);
		return true;
	} else {
		if(insert(n->child[0], dat))
			return true;
		return insert(n->child[1], dat);
	}
}

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

void draw_tree(node* n, image* to) {
	cout << "drawing " << n->id << endl;
	if(n->dat && n->dat->img)
		to->set_section(n->dat->img, n->r.x, n->r.y, n->dat->min);
	if(n->child[0])
		draw_tree(n->child[0], to);
	if(n->child[1])
		draw_tree(n->child[1], to);
}

void doit(vector<string> imageFiles, string outputFile) {
	int out_w = 2048, out_h = 2048;

	vector<data*> metadata;
	node *head = new node(out_w, out_h);

	for(int i = 0; i < imageFiles.size(); i++) {
		cout << imageFiles[i] << endl;
		image *img = new image(imageFiles[i].c_str());
		rect min = find_min_rect(img);
		rect orig = {0, 0, img->w, img->h};

		data *d = new data(img, min, orig);
		metadata.push_back(d);
	}

	sort(metadata.begin(), metadata.end(), compare);

	for(int i = 0; i < metadata.size(); i++) {
		if(!insert(head, metadata[i])) {
			cout << "failed to insert to tree" << endl;
		}
	}

	image *out = new image(out_w, out_h);
	draw_tree(head, out);
	out->write(outputFile.c_str());

	for(int i = 0; i < metadata.size(); i++) {
		delete metadata[i]->img;
		delete metadata[i];
	}
	delete head;
}

int main(int argc, char **argv)
{
	vector<string> args;
	string outputFile = "output.png";
	for(int i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], "-o") == 0 && i + 1 < argc)
		{
			outputFile = argv[i + 1];
			i += 2;
		}
		else
		{
			args.push_back(argv[i]);
		}
	}
	
	try
	{
		doit(args, outputFile);
	}
	catch(exception &e)
	{
		cerr << e.what() << endl;
	}
	catch(const char *e)
	{
		cerr << e << endl;
	}
	
	return 0;
}
