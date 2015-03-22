#include "HeightMap.h"



HeightMap::HeightMap(char *imagefile) {

	unsigned error;
	maxHeight = 0;
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



int HeightMap::getHeight(unsigned int w, unsigned int h) {
	if ( w>=0 && w<heightMap->width && h>=0 && h<heightMap->length) {
		return (int) heightMap->data[heightMap->width * h + w];
	} else {
		return 0;
	}
}



float HeightMap::getSmoothedHeight(unsigned int w, unsigned int l) {
	int iw, il;
	float height = 0.0;
	int smoothRadius = 2;
	int cellAmount = (2*smoothRadius+1) * (2*smoothRadius+1);

	for(iw=-smoothRadius; iw<=smoothRadius; ++iw) {
		for(il=-smoothRadius; il<=smoothRadius; ++il) {
			height += getHeight(w+iw, l+il);
		}
	}

	return height / cellAmount;
}



int HeightMap::getMaxHeight() {
	return (int) maxHeight;
}



bool HeightMap::wasSuccessful() {
	return !hasError;
}



unsigned HeightMap::loadImage(Image *image, char *filename) {
	return lodepng_decode32_file(&(image->data), &(image->width), &(image->height), filename);
}



void HeightMap::createHeightMapFromImage(Image *image) {
	heightMap = new Map();
	heightMap->width = image->width;
	heightMap->length = image->height;
	heightMap->data = (unsigned char*) malloc(heightMap->width * heightMap->length * sizeof(unsigned char));

	unsigned int iw, ih;
	unsigned char r,g,b,a, height;

	for(ih = 0; ih < image->height; ih++) {
		for(iw = 0; iw < image->width; iw++) {
			r = image->data[4 * image->width * ih + 4 * iw + 0];
			g = image->data[4 * image->width * ih + 4 * iw + 1];
			b = image->data[4 * image->width * ih + 4 * iw + 2];
			a = image->data[4 * image->width * ih + 4 * iw + 3];

			height = (unsigned) (1.0f / 3.0f * (r+g+b));

			heightMap->data[heightMap->width * ih + iw] = height;

			if(height > maxHeight) maxHeight = height;
		}
	}
}