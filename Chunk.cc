#include "Chunk.h"



Chunk::Chunk(int pw, int ph, int pl, HeightMap *heightMap, VoxelMap *voxelMap) {
	this->pw = pw;
	this->ph = ph;
	this->pl = pl;
	this->voxelMap = voxelMap;

	width = voxelMap->getChunkWidth();
	height = voxelMap->getChunkHeight();
	length = voxelMap->getChunkLength();

	data = (float*) malloc(width * height * length * sizeof(float));

	int w,h,l;
	for (w=0; w<width; ++w) {
		for(h=0; h<height; ++h) {
			for (l=0; l<length; ++l) {
				data[index(w, h, l)] = calculateDensityFromHeightMap(heightMap, pw * width + w, ph * height + h, pl * length + l);
				if(data[index(w, h, l)] < -1) data[index(w, h, l)] = -1;
				if(data[index(w, h, l)] >  1) data[index(w, h, l)] =  1;
			}
		}
	}

	hasChanged = true;
}



Chunk::~Chunk() {
	delete[] data;
}



void Chunk::calculateSurface(MarchingCuber *marchingCuber, float voxelSize) {
	if(hasChanged) {
		Vec3f position = Vec3f((float) pw * width, (float) ph * height, (float) pl * length);
		triangles = marchingCuber->extractSurface(voxelMap, position, position.mult(voxelSize), width+1, height+1, length+1, voxelSize, 0.0f);
		hasChanged = false;
	}
}



std::vector<TRIANGLE>* Chunk::getTriangles() {
	return &triangles;
}



float Chunk::getDensity(int w, int h, int l) {
	return data[index(w, h, l)];
}



void Chunk::setDensity(int w, int h, int l, float value) {
	data[index(w, h, l)] = value;
	if(data[index(w, h, l)] < -1) data[index(w, h, l)] = -1;
	if(data[index(w, h, l)] >  1) data[index(w, h, l)] =  1;
	hasChanged = true;
}



float Chunk::calculateDensityFromHeightMap(HeightMap *heightMap, int w, int h, int l) {
	int iw, ih, il;
	float density = 0.0f;
	int smoothRadius = 1;

	for(iw=-smoothRadius; iw<=smoothRadius; ++iw) {
		for(ih=-smoothRadius; ih<=smoothRadius; ++ih) {
			for(il=-smoothRadius; il<=smoothRadius; ++il) {
				density += getVerticalDistanceFromHeightMap(heightMap, w+iw, h+ih, l+il);
			}
		}
	}
	density /= (float) ((2*smoothRadius+1) * (2*smoothRadius+1) * (2*smoothRadius+1));

	return density;
}



float Chunk::getVerticalDistanceFromHeightMap(HeightMap *heightMap, int w, int h, int l) {
	float heightInHeightMap = voxelMap->getHeight() * heightMap->getSmoothedHeight(w-0.5*width, l-0.5*length) / 255.0f;
	float distance = heightInHeightMap - h;
	if(distance < -1) distance = -1;
	if(distance >  1) distance =  1;
	return distance;
}



inline int Chunk::index(int w, int h, int l) {
	return l * (width * height) + h * width + w;
}