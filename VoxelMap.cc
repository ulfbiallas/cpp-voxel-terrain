#include "VoxelMap.h"



VoxelMap::VoxelMap(HeightMap *heightMap) {

	width = heightMap->getWidth();
	length = heightMap->getLength();
	height = 32;

	data = (float*) malloc(width * height * length * sizeof(float));

	int w,h,l, iw, ih, il;
	float heightInHeightMap;
	int smoothRadius = 1;

	for (w=0; w<width; ++w) {
		for (l=0; l<length; ++l) {
			for(h=0; h<height; ++h) {

				float density = 0.0f;
				for(iw=-smoothRadius; iw<=smoothRadius; ++iw) {
					for(ih=-smoothRadius; ih<=smoothRadius; ++ih) {
						for(il=-smoothRadius; il<=smoothRadius; ++il) {
							density += getVerticalDistanceFromHeightMap(heightMap, w+iw, h+ih, l+il);
						}
					}
				}
				density /= (float) ((2*smoothRadius+1) * (2*smoothRadius+1) * (2*smoothRadius+1));

				data[index(w, h, l)] = density;
				if(data[index(w, h, l)] < -1) data[index(w, h, l)] = -1;
				if(data[index(w, h, l)] >  1) data[index(w, h, l)] =  1;

			}
		}
	}

	marchingCuber = new MarchingCuber();
	triangles = marchingCuber->extractSurface(&data, Vec3f(0,0,0), width, height, length, 0.2f, 0.0f);
}



float VoxelMap::getVerticalDistanceFromHeightMap(HeightMap *heightMap, int w, int h, int l) {
	float heightInHeightMap = height * heightMap->getSmoothedHeight(w, l) / 255.0f;
	float distance = heightInHeightMap - h;
	if(distance < -1) distance = -1;
	if(distance >  1) distance =  1;
	return distance;
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



std::vector<MarchingCuber::TRIANGLE> VoxelMap::getTriangles() {
	return triangles;
}



int VoxelMap::index(int w, int h, int l) {
	return l * (width * height) + h * width + w;
}