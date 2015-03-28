#ifndef CHUNK_H
#define CHUNK_H

#include <cstdlib>
#include <iostream>
#include <vector>
#include "MarchingCuber.h"
#include "VoxelMap.h"

class VoxelMap;
class MarchingCuber;



class Chunk {

    public:      

		Chunk(int pw, int ph, int pl, HeightMap *heightMap, VoxelMap *voxelMap);
		~Chunk();
		void calculateSurface(MarchingCuber *marchingCuber, float voxelSize);
		float getDensity(int w, int h, int l);
		void setDensity(int w, int h, int l, float value);
		std::vector<TRIANGLE>* getTriangles();



	private:
		int pw, ph, pl; // position
		int width, height, length;
		float *data;
		VoxelMap *voxelMap;
		std::vector<TRIANGLE> triangles;

		float calculateDensityFromHeightMap(HeightMap *heightMap, int w, int h, int l);
		float getVerticalDistanceFromHeightMap(HeightMap *heightMap, int w, int h, int l);
		int index(int w, int h, int l);

};



#endif