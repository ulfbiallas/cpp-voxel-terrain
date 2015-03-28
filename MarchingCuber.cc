#include "MarchingCuber.h"

/*
This implementation is based on the one by Paul Bourke
which can be found on his website.
http://paulbourke.net/geometry/polygonise/
*/



std::vector<TRIANGLE> MarchingCuber::extractSurface(VoxelMap *voxelMap, Vec3f start, Vec3f pos_, int dimX, int dimY, int dimZ, float h, float isolevel) {
	
	std::vector<TRIANGLE> triangles;
	
	GRIDCELL grid;
	TRIANGLE *temptriangles = (TRIANGLE*) malloc(5 * sizeof(TRIANGLE));

	int spx, spy, spz;
	spx = start.x;
	spy = start.y;
	spz = start.z;

	float sz = 0.5f*h;
	int k, ix, iy, iz;
	for(iz=0; iz<dimZ-1; ++iz) {
		for(iy=0; iy<dimY-1; ++iy) {
			for(ix=0; ix<dimX-1; ++ix) {
	
				Vec3f ref = pos_;
				
				float hx = h * ix;
				float hy = h * iy;
				float hz = h * iz;
				
				grid.p[0] = ref.add(Vec3f(hx - sz, hy - sz, hz - sz));
				grid.p[1] = ref.add(Vec3f(hx + sz, hy - sz, hz - sz));
				grid.p[2] = ref.add(Vec3f(hx + sz, hy - sz, hz + sz));
				grid.p[3] = ref.add(Vec3f(hx - sz, hy - sz, hz + sz));
				grid.p[4] = ref.add(Vec3f(hx - sz, hy + sz, hz - sz));
				grid.p[5] = ref.add(Vec3f(hx + sz, hy + sz, hz - sz));
				grid.p[6] = ref.add(Vec3f(hx + sz, hy + sz, hz + sz));
				grid.p[7] = ref.add(Vec3f(hx - sz, hy + sz, hz + sz));				
				
				grid.val[0] = voxelMap->getDensity(spx + ix + 0, spy + iy + 0, spz + iz + 0);
				grid.val[1] = voxelMap->getDensity(spx + ix + 1, spy + iy + 0, spz + iz + 0);
				grid.val[2] = voxelMap->getDensity(spx + ix + 1, spy + iy + 0, spz + iz + 1);
				grid.val[3] = voxelMap->getDensity(spx + ix + 0, spy + iy + 0, spz + iz + 1);
				grid.val[4] = voxelMap->getDensity(spx + ix + 0, spy + iy + 1, spz + iz + 0);
				grid.val[5] = voxelMap->getDensity(spx + ix + 1, spy + iy + 1, spz + iz + 0);
				grid.val[6] = voxelMap->getDensity(spx + ix + 1, spy + iy + 1, spz + iz + 1);
				grid.val[7] = voxelMap->getDensity(spx + ix + 0, spy + iy + 1, spz + iz + 1);
				
				grid.n[0] = getGradient(voxelMap, dimX, dimY, dimZ, spx + ix + 0, spy + iy + 0, spz + iz + 0);
				grid.n[1] = getGradient(voxelMap, dimX, dimY, dimZ, spx + ix + 1, spy + iy + 0, spz + iz + 0);
				grid.n[2] = getGradient(voxelMap, dimX, dimY, dimZ, spx + ix + 1, spy + iy + 0, spz + iz + 1);
				grid.n[3] = getGradient(voxelMap, dimX, dimY, dimZ, spx + ix + 0, spy + iy + 0, spz + iz + 1);
				grid.n[4] = getGradient(voxelMap, dimX, dimY, dimZ, spx + ix + 0, spy + iy + 1, spz + iz + 0);
				grid.n[5] = getGradient(voxelMap, dimX, dimY, dimZ, spx + ix + 1, spy + iy + 1, spz + iz + 0);
				grid.n[6] = getGradient(voxelMap, dimX, dimY, dimZ, spx + ix + 1, spy + iy + 1, spz + iz + 1);
				grid.n[7] = getGradient(voxelMap, dimX, dimY, dimZ, spx + ix + 0, spy + iy + 1, spz + iz + 1);
				
				int ntriang =  polygonise(grid, isolevel, temptriangles);

				if(ntriang>0) {
					for(k=0; k<ntriang; ++k) {
						triangles.push_back(temptriangles[k]);
					}
				}
						
			}
		}	
	}
	
	return triangles;
}



int MarchingCuber::polygonise(GRIDCELL grid, float isolevel, TRIANGLE *triangles) {

	int i,ntriang;
	int cubeindex;
	Vec3f vertlist[12];
	Vec3f normallist[12];
	
	cubeindex = 0;
	if (grid.val[0] < isolevel) cubeindex |= 1;
	if (grid.val[1] < isolevel) cubeindex |= 2;
	if (grid.val[2] < isolevel) cubeindex |= 4;
	if (grid.val[3] < isolevel) cubeindex |= 8;
	if (grid.val[4] < isolevel) cubeindex |= 16;
	if (grid.val[5] < isolevel) cubeindex |= 32;
	if (grid.val[6] < isolevel) cubeindex |= 64;
	if (grid.val[7] < isolevel) cubeindex |= 128;

	if (edgeTable[cubeindex] == 0)
		return(0);

	if (edgeTable[cubeindex] & 1) {
		vertlist[0]   = interpolateVertex(isolevel,grid.p[0],grid.p[1],grid.val[0],grid.val[1]);
		normallist[0] = interpolateVertex(isolevel,grid.n[0],grid.n[1],grid.val[0],grid.val[1]);
	}
    
	if (edgeTable[cubeindex] & 2) {
		vertlist[1] = interpolateVertex(isolevel,grid.p[1],grid.p[2],grid.val[1],grid.val[2]);
		normallist[1] = interpolateVertex(isolevel,grid.n[1],grid.n[2],grid.val[1],grid.val[2]);
	}
      
	if (edgeTable[cubeindex] & 4) {
		vertlist[2] = interpolateVertex(isolevel,grid.p[2],grid.p[3],grid.val[2],grid.val[3]);	
		normallist[2] = interpolateVertex(isolevel,grid.n[2],grid.n[3],grid.val[2],grid.val[3]);	
	}
      
	if (edgeTable[cubeindex] & 8) {
		vertlist[3] = interpolateVertex(isolevel,grid.p[3],grid.p[0],grid.val[3],grid.val[0]);
		normallist[3] = interpolateVertex(isolevel,grid.n[3],grid.n[0],grid.val[3],grid.val[0]);
	}
      
	if (edgeTable[cubeindex] & 16) {
		vertlist[4] = interpolateVertex(isolevel,grid.p[4],grid.p[5],grid.val[4],grid.val[5]);
		normallist[4] = interpolateVertex(isolevel,grid.n[4],grid.n[5],grid.val[4],grid.val[5]);
	}
      
	if (edgeTable[cubeindex] & 32) {
		vertlist[5] = interpolateVertex(isolevel,grid.p[5],grid.p[6],grid.val[5],grid.val[6]);
		normallist[5] = interpolateVertex(isolevel,grid.n[5],grid.n[6],grid.val[5],grid.val[6]);
	}
      
	if (edgeTable[cubeindex] & 64) {
		vertlist[6] = interpolateVertex(isolevel,grid.p[6],grid.p[7],grid.val[6],grid.val[7]);
		normallist[6] = interpolateVertex(isolevel,grid.n[6],grid.n[7],grid.val[6],grid.val[7]);
	}
      
	if (edgeTable[cubeindex] & 128) {
		vertlist[7] = interpolateVertex(isolevel,grid.p[7],grid.p[4],grid.val[7],grid.val[4]);
		normallist[7] = interpolateVertex(isolevel,grid.n[7],grid.n[4],grid.val[7],grid.val[4]);
	}
      
	if (edgeTable[cubeindex] & 256) {
		vertlist[8] = interpolateVertex(isolevel,grid.p[0],grid.p[4],grid.val[0],grid.val[4]);
		normallist[8] = interpolateVertex(isolevel,grid.n[0],grid.n[4],grid.val[0],grid.val[4]);
	}
      
	if (edgeTable[cubeindex] & 512) {
		vertlist[9] = interpolateVertex(isolevel,grid.p[1],grid.p[5],grid.val[1],grid.val[5]);
		normallist[9] = interpolateVertex(isolevel,grid.n[1],grid.n[5],grid.val[1],grid.val[5]);
	}
      
	if (edgeTable[cubeindex] & 1024) {
		vertlist[10] = interpolateVertex(isolevel,grid.p[2],grid.p[6],grid.val[2],grid.val[6]);
		normallist[10] = interpolateVertex(isolevel,grid.n[2],grid.n[6],grid.val[2],grid.val[6]);
	}
      
	if (edgeTable[cubeindex] & 2048) {
		vertlist[11] = interpolateVertex(isolevel,grid.p[3],grid.p[7],grid.val[3],grid.val[7]);
		normallist[11] = interpolateVertex(isolevel,grid.n[3],grid.n[7],grid.val[3],grid.val[7]);
	}

	ntriang = 0;
	for (i=0; triTable[cubeindex][i]!=-1; i+=3) {
		triangles[ntriang].p[0] = vertlist[triTable[cubeindex][i  ]];
		triangles[ntriang].p[1] = vertlist[triTable[cubeindex][i+1]];
		triangles[ntriang].p[2] = vertlist[triTable[cubeindex][i+2]];
		triangles[ntriang].n[0] = normallist[triTable[cubeindex][i  ]];
		triangles[ntriang].n[1] = normallist[triTable[cubeindex][i+1]];
		triangles[ntriang].n[2] = normallist[triTable[cubeindex][i+2]];
		ntriang++;
	}

	return(ntriang);
}



Vec3f MarchingCuber::interpolateVertex(float isolevel, Vec3f p1, Vec3f p2, float valp1, float valp2) {

   float mu;
   Vec3f  p;
   
   if (abs(isolevel-valp1) < 0.00001) return(p1);
   if (abs(isolevel-valp2) < 0.00001) return(p2);
   if (abs(valp1-valp2) < 0.00001) return(p1);
   
   mu = (isolevel - valp1) / (valp2 - valp1);
   
   p.x = p1.x + mu * (p2.x - p1.x);
   p.y = p1.y + mu * (p2.y - p1.y);
   p.z = p1.z + mu * (p2.z - p1.z);

   return p;
}



Vec3f MarchingCuber::getGradient(VoxelMap *voxelMap, int dimX, int dimY, int dimZ, int ix_, int iy_, int iz_) {

	float dx = ( voxelMap->getDensity(ix_ - 1, iy_, iz_ ) - voxelMap->getDensity(ix_ + 1, iy_, iz_ ) );
	float dy = ( voxelMap->getDensity(ix_, iy_ - 1, iz_ ) - voxelMap->getDensity(ix_, iy_ + 1, iz_ ) );
	float dz = ( voxelMap->getDensity(ix_, iy_, iz_ - 1 ) - voxelMap->getDensity(ix_, iy_, iz_ + 1 ) );
		
	Vec3f n = Vec3f(dx, dy, dz);
	if(n.norm() < 0.01) {
		return Vec3f();
	} else {
		return n.normalize();
	}

}



int MarchingCuber::index(int dimX, int dimY, int ix_, int iy_, int iz_) {
	return ix_ + iy_*dimX + iz_*dimX*dimY;
}