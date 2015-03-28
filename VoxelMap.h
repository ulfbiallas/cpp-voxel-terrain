#ifndef VOXELMAP_H
#define VOXELMAP_H

#include <cstdlib>
#include <iostream>
#include <vector>
#include "lodepng.h"
#include "HeightMap.h"
#include "Chunk.h"
#include "MarchingCuber.h"

class Chunk;



class VoxelMap {

    public:      

		VoxelMap(HeightMap *heightMap);
		~VoxelMap();

		float getDensity(int w, int h, int l);
		int getWidth();
		int getHeight();
		int getLength();
		int getChunkWidth();
		int getChunkHeight();
		int getChunkLength();
		std::vector<MarchingCuber::TRIANGLE> getTriangles();
		float intersectRay(Vec3f origin, Vec3f direction);
		void extractSurface();
		void reduceDensityAtPoint(Vec3f point);



	private:

		int width, height, length;
		int chunkWidth, chunkHeight, chunkLength;
		int chunksW, chunksH, chunksL;

		Chunk **chunks;
		float *data;
		float voxelSize;
		MarchingCuber *marchingCuber;
		std::vector<MarchingCuber::TRIANGLE> triangles;

		int index(int w, int h, int l);
		int chunkIndex(int w, int h, int l);
		float calculateDensityFromHeightMap(HeightMap *heightMap, int w, int h, int l);
		float getVerticalDistanceFromHeightMap(HeightMap *heightMap, int w, int h, int l);
		bool isRayIntersectingVoxel(Vec3f origin, Vec3f direction, int w, int h, int l);

};



#endif