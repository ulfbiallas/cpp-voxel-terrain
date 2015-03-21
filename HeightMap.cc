#include "HeightMap.h"



HeightMap::HeightMap(char *imagefile) {

	unsigned error;
	hasError = false;
	Image *image = new Image();

	error = loadImage(image, imagefile);

	if(error) {
		hasError = true;
	} else {
		createHeightMapFromImage(image);
	}

	delete[] image;
}



HeightMap::~HeightMap() {
	delete[] heightMap;
}



int HeightMap::getWidth() {
	return heightMap->width;
}



int HeightMap::getLength() {
	return heightMap->length;
}



bool HeightMap::wasSuccessful() {
	return !hasError;
}



int HeightMap::getHeight(int x, int y) {
	if ( x>=0 && x<heightMap->width && y>=0 && y<heightMap->length) {
		return (int) heightMap->data[heightMap->width * y + x];
	} else {
		return 0;
	}
}



unsigned HeightMap::loadImage(Image *image, char *filename) {
	return lodepng_decode32_file(&(image->data), &(image->width), &(image->height), filename);
}



void HeightMap::createHeightMapFromImage(Image *image) {
	heightMap = new Map();
	heightMap->width = image->width;
	heightMap->length = image->height;
	heightMap->data = (unsigned char*) malloc(heightMap->width * heightMap->length * sizeof(unsigned char));

	int iw, ih;
	unsigned char r,g,b,a, grayvalue;

	for(ih = 0; ih < image->height; ih++) {
		for(iw = 0; iw < image->width; iw++) {
			r = image->data[4 * image->width * ih + 4 * iw + 0];
			g = image->data[4 * image->width * ih + 4 * iw + 1];
			b = image->data[4 * image->width * ih + 4 * iw + 2];
			a = image->data[4 * image->width * ih + 4 * iw + 3];

			grayvalue = 1.0 / 3.0f * (r+g+b);

			heightMap->data[heightMap->width * ih + iw] = grayvalue;
		}
	}
}