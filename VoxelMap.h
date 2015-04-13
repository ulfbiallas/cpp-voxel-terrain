#ifndef VOXELMAP_H
#define VOXELMAP_H

#include <cstdlib>
#include <iostream>
#include <vector>
#include "lodepng.h"
#include "HeightMap.h"
#include "Chunk.h"

class Chunk;
class MarchingCuber;



class VoxelMap {

	

    public:

		enum DENSITY_CHANGE_METHOD {INCREASE, DECREASE};

		VoxelMap(HeightMap *heightMap);
		~VoxelMap();

		int getWidth();
		int getHeight();
		int getLength();
		int getChunkWidth();
		int getChunkHeight();
		int getChunkLength();
		std::vector<TRIANGLE> getTriangles(Vec3f viewerPosition, Vec3f viewerDirection);
		float intersectRay(Vec3f origin, Vec3f direction);
		void extractSurface();
		void changeDensityAtPoint(Vec3f point, DENSITY_CHANGE_METHOD densityChangeMethod);
		float getDensity(int w, int h, int l);
		void setDensity(int w, int h, int l, float value);

	private:

		int width, height, length;
		int chunkWidth, chunkHeight, chunkLength;
		int chunksW, chunksH, chunksL;

		Chunk **chunks;
		float *data;
		float voxelSize;
		MarchingCuber *marchingCuber;
		std::vector<TRIANGLE> triangles;

		inline int index(int w, int h, int l);
		inline int chunkIndex(int w, int h, int l);
		inline bool isRayIntersectingVoxel(Vec3f origin, Vec3f direction, int w, int h, int l);

};



#endif