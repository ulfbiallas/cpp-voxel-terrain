#ifndef MARCHINGCUBER_H
#define MARCHINGCUBER_H

#include <cstdlib>
#include <iostream>
#include <vector>
#include "datatypes.h"



class MarchingCuber {
	
    public:      

		struct TRIANGLE {

			Vec3f p[3], n[3];

			TRIANGLE() {
				p[0] = Vec3f();
				p[1] = Vec3f();
				p[2] = Vec3f();
				n[0] = Vec3f();
				n[1] = Vec3f();
				n[2] = Vec3f();
			};
		};

		struct GRIDCELL {
		   Vec3f p[8];
		   Vec3f n[8];
		   float val[8];
		};


		std::vector<TRIANGLE> marchingCubes(float **voxels, Vec3f pos, int dimX, int dimY, int dimZ, float h, float isolevel);



	private:

		int edgeTable[256];
		int triTable[256][16];
		
		Vec3f getGradient(float **voxels, int dimX, int dimY, int dimZ, int ix_, int iy_, int iz_);
		Vec3f VertexInterp(float isolevel, Vec3f p1, Vec3f p2, float valp1, float valp2);
		int Polygonise(GRIDCELL grid, float isolevel, TRIANGLE *triangles);
		int idx(int dimX, int dimY, int ix_, int iy_, int iz_);	

};

#endif
