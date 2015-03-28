#include "Chunk.h"



Chunk::Chunk(int pw, int ph, int pl, HeightMap *heightMap, VoxelMap *voxelMap) {
	this->pw = pw;
	this->ph = ph;
	this->pl = pl;
	this->voxelMap = voxelMap;

	std::cout << "create chunk at: " << pw << ", " << ph << ", " << pl << "\n";

	width = voxelMap->getChunkWidth();
	height = voxelMap->getChunkHeight();
	length = voxelMap->getChunkLength();

	std::cout << "size: " << width << ", " << height << ", " << length << "\n";

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

}



Chunk::~Chunk() {
	delete[] data;
}



void Chunk::extractSurface(MarchingCuber *marchingCuber, std::vector<MarchingCuber::TRIANGLE>* triangles, float voxelSize) {
	Vec3f position = Vec3f((float) pw * width, (float) ph * height, (float) pl * length);
	std::cout << "extract surface at: " << position.x << ", " << position.y << ", " << position.z << "\n";
	std::vector<MarchingCuber::TRIANGLE> chunkTriangles = marchingCuber->extractSurface(&data, position.mult(voxelSize), width, height, length, voxelSize, 0.0f);

	for (int k=0; k<chunkTriangles.size(); ++k) {
		triangles->push_back(chunkTriangles[k]);
	}
}



float Chunk::getDensity(int w, int h, int l) {
	return data[index(w, h, l)];
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
	float heightInHeightMap = height * heightMap->getSmoothedHeight(w, l) / 255.0f;
	float distance = heightInHeightMap - h;
	if(distance < -1) distance = -1;
	if(distance >  1) distance =  1;
	return distance;
}



int Chunk::index(int w, int h, int l) {
	return l * (width * height) + h * width + w;
}