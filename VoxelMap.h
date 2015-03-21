#ifndef VOXELMAP_H
#define VOXELMAP_H

#include <cstdlib>
#include <iostream>
#include <vector>
#include "lodepng.h"
#include "HeightMap.h"
#include "MarchingCuber.h"



class VoxelMap {

    public:      

		VoxelMap(HeightMap *heightMap);
		~VoxelMap();

		float getDensity(int w, int h, int l);
		int getWidth();
		int getHeight();
		int getLength();
		std::vector<MarchingCuber::TRIANGLE> getTriangles();


	private:

		int width, height, length;
		float *data;
		MarchingCuber *marchingCuber;
		std::vector<MarchingCuber::TRIANGLE> triangles;

		int index(int w, int h, int l);

};



#endif