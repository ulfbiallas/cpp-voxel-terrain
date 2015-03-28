#ifndef CHUNK_H
#define CHUNK_H

#include <cstdlib>
#include <iostream>
#include <vector>
#include "VoxelMap.h"
#include "MarchingCuber.h"

class VoxelMap;



class Chunk {

    public:      

		Chunk(int pw, int ph, int pl, VoxelMap *voxelMap);
		~Chunk();
		void extractSurface(MarchingCuber *marchingCuber, std::vector<MarchingCuber::TRIANGLE>* triangles, float voxelSize);



	private:
		int pw, ph, pl; // position
		int width, height, length;
		float *data;

		int index(int w, int h, int l);
};



#endif