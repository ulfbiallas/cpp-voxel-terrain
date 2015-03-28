#ifndef CHUNK_H
#define CHUNK_H

#include <cstdlib>
#include <iostream>
#include <vector>
#include "MarchingCuber.h"
#include "VoxelMap.h"

class VoxelMap;



class Chunk {

    public:      

		Chunk(int pw, int ph, int pl, HeightMap *heightMap, VoxelMap *voxelMap);
		~Chunk();
		void extractSurface(MarchingCuber *marchingCuber, std::vector<MarchingCuber::TRIANGLE>* triangles, float voxelSize);
		float getDensity(int w, int h, int l);



	private:
		int pw, ph, pl; // position
		int width, height, length;
		float *data;
		VoxelMap *voxelMap;

		float calculateDensityFromHeightMap(HeightMap *heightMap, int w, int h, int l);
		float getVerticalDistanceFromHeightMap(HeightMap *heightMap, int w, int h, int l);
		int index(int w, int h, int l);

};



#endif