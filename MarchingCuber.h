#ifndef MARCHINGCUBER_H
#define MARCHINGCUBER_H

#include <cstdlib>
#include <iostream>
#include <vector>
#include "datatypes.h"
#include "MarchingCuberConstants.h"
#include "VoxelMap.h"

class VoxelMap;

/*
This implementation is based on the one by Paul Bourke
which can be found on his website.
http://paulbourke.net/geometry/polygonise/
*/




class MarchingCuber {

	public:

		std::vector<TRIANGLE> extractSurface(VoxelMap *voxelMap, Vec3f start, Vec3f pos, int dimX, int dimY, int dimZ, float h, float isolevel);



	private:
		
		Vec3f getGradient(VoxelMap *voxelMap, int dimX, int dimY, int dimZ, int ix_, int iy_, int iz_);
		Vec3f interpolateVertex(float isolevel, Vec3f p1, Vec3f p2, float valp1, float valp2);
		int polygonise(GRIDCELL grid, float isolevel, TRIANGLE *triangles);
		int index(int dimX, int dimY, int ix_, int iy_, int iz_);	

		
};

#endif
