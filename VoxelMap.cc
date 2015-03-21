#include "VoxelMap.h"



VoxelMap::VoxelMap(HeightMap *heightMap) {

	width = heightMap->getWidth();
	length = heightMap->getLength();
	height = 32;

	data = (float*) malloc(width * height * length * sizeof(float));

	int w,h,l;
	float heightInHeightMap;
	
	for (w=0; w<width; ++w) {
		for (l=0; l<length; ++l) {

			heightInHeightMap = height * heightMap->getHeight(w, l) / 255.0;

			for(h=0; h<height; ++h) {
				if(h<heightInHeightMap) {
					data[index(w, h, l)] = 1;
				} else {
					data[index(w, h, l)] = 0;
				}
			}

		}
	}

}



VoxelMap::~VoxelMap() {

}



float VoxelMap::getDensity(int w, int h, int l) {
	if(w>=0 && w<width && h>=0 && h<height && l>=0 && l<length) {
		return data[index(w, h, l)];
	} else {
		return 0;
	}
}



int VoxelMap::getWidth() {
	return width;
}



int VoxelMap::getHeight() {
	return height;
}



int VoxelMap::getLength() {
	return length;
}



int VoxelMap::index(int w, int h, int l) {
	return l * (width * height) + h * width + w;
}