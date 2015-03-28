#include "Chunk.h"



Chunk::Chunk(int pw, int ph, int pl, VoxelMap *voxelMap) {
	this->pw = pw;
	this->ph = ph;
	this->pl = pl;

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
				data[index(w, h, l)] = voxelMap->getDensity(pw * width + w, ph * height + h, pl * length + l);
			}
		}
	}

}



Chunk::~Chunk() {
	delete[] data;
}



void Chunk::extractSurface(MarchingCuber *marchingCuber, std::vector<MarchingCuber::TRIANGLE>* triangles, float voxelSize) {
	Vec3f position = Vec3f(pw * width, ph * height, pl * length);
	std::cout << "extract surface at: " << position.x << ", " << position.y << ", " << position.z << "\n";
	std::vector<MarchingCuber::TRIANGLE> chunkTriangles = marchingCuber->extractSurface(&data, position.mult(voxelSize), width, height, length, voxelSize, 0.0f);

	for (int k=0; k<chunkTriangles.size(); ++k) {
		triangles->push_back(chunkTriangles[k]);
	}
}



int Chunk::index(int w, int h, int l) {
	return l * (width * height) + h * width + w;
}