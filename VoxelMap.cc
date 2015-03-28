#include "VoxelMap.h"



VoxelMap::VoxelMap(HeightMap *heightMap) {

	width = heightMap->getWidth();
	length = heightMap->getLength();
	height = 32;
	voxelSize = 0.2f;

	data = (float*) malloc(width * height * length * sizeof(float));

	chunkWidth = 16;
	chunkHeight = 16;
	chunkLength = 16;

	chunksW = (int) ceil((float) width / chunkWidth) + 1;
	chunksH = (int) ceil((float) height / chunkHeight) + 1;
	chunksL = (int) ceil((float) length / chunkLength) + 1;

	int w,h,l;
	chunks = (Chunk**) malloc(chunksW * chunksH * chunksL * sizeof(Chunk*));
	for (w=0; w<chunksW; ++w) {
		for(h=0; h<chunksH; ++h) {
			for (l=0; l<chunksL; ++l) {	
				chunks[chunkIndex(w, h, l)] = new Chunk(w, h, l, heightMap, this);
			}
		}
	}


	marchingCuber = new MarchingCuber();
	//extractSurface();

	/*
	int chunkCount = chunksW * chunksH * chunksL;
	for (int k=0; k<chunkCount; ++k) {
		chunks[k]->extractSurface(marchingCuber, &triangles, voxelSize);
	}
	*/

	for (w=0; w<chunksW; ++w) {
		for(h=0; h<chunksH; ++h) {
			for (l=0; l<chunksL; ++l) {	

				Vec3f position = Vec3f((float) w * chunkWidth, (float) h * chunkHeight, (float) l * chunkLength);
				std::vector<TRIANGLE> chunkTriangles = marchingCuber->extractSurface(this, position, position.mult(voxelSize), chunkWidth+1, chunkHeight+1, chunkLength+1, voxelSize, 0.0f);

				for (int k=0; k<chunkTriangles.size(); ++k) {
					triangles.push_back(chunkTriangles[k]);
				}

			}
		}
	}


}



VoxelMap::~VoxelMap() {
	delete[] data;
}



void VoxelMap::extractSurface() {
//	triangles = marchingCuber->extractSurface(&data, Vec3f(0,0,0), width, height, length, voxelSize, 0.0f);
}



void VoxelMap::reduceDensityAtPoint(Vec3f point) {

	int radius = 5;
	float reductionFactor = 0.1f;

	int radius2 = radius * radius;
	float distance;
	float dDensity;
	int rx,ry,rz, w, h, l;
	Vec3f p = point.mult(1.0f / voxelSize);

	for(rx=-radius; rx<=radius; ++rx) {
		for(ry=-radius; ry<=radius; ++ry) {
			for(rz=-radius; rz<=radius; ++rz) {

				distance = sqrt((float) (rx*rx + ry*ry + rz*rz));

				dDensity = distance - radius;
				dDensity *= reductionFactor;
				if(dDensity > 0) dDensity = 0;
				if(dDensity < -1) dDensity = -1;
				
				w = (int) p.x + rx;
				h = (int) p.y + ry;
				l = (int) p.z + rz;

				if(w>=0 && w<width && h>=0 && h<height && l>=0 && l<length) {
					data[index(w, h, l)] += dDensity;
					if(data[index(w, h, l)] < -1) data[index(w, h, l)] = -1;
					if(data[index(w, h, l)] >  1) data[index(w, h, l)] =  1;
				}

			}
		}
	}
}



int VoxelMap::getWidth() {
	return width;
}



int VoxelMap::getHeight() {
	return height;
}



int VoxelMap::getLength() {
	return length;
}



int VoxelMap::getChunkWidth() {
	return chunkWidth;
}



int VoxelMap::getChunkHeight() {
	return chunkHeight;
}



int VoxelMap::getChunkLength() {
	return chunkLength;
}



std::vector<TRIANGLE> VoxelMap::getTriangles() {
	return triangles;
}



float VoxelMap::intersectRay(Vec3f origin, Vec3f direction) {
	float minDist = 99999;
	float t;

	int w, h, l;
	for (w=0; w<width; ++w) {
		for(h=0; h<height; ++h) {
			for (l=0; l<length; ++l) {	
				if(data[index(w, h, l)] >= 0) {
					if(isRayIntersectingVoxel(origin, direction, w, h, l)) {
						Vec3f p = Vec3f((float) w, (float) h, (float) l).mult(voxelSize);
						t = p.sub(origin).norm();
						if(t >= 0) {
							if(t < minDist) {
								minDist = t;
							}
						}
					}
				}
			}
		}
	}

	return minDist;
}



bool VoxelMap::isRayIntersectingVoxel(Vec3f origin, Vec3f direction, int w, int h, int l) {
	Vec3f p = Vec3f((float) w, (float) h, (float) l).mult(voxelSize);
	Vec3f op = p.sub(origin);
	float lambda = op.dot(direction);
	Vec3f p2 = origin.add(direction.mult(lambda));
	float dist = (p2.sub(p)).norm();
	
	if(dist*dist < 0.5 * voxelSize * 0.5 * voxelSize) {
		return true;
	} else {
		return false;
	}
}




float VoxelMap::getDensity(int w, int h, int l) {
	int pw = (int) floor((float) w / chunkWidth);
	int ph = (int) floor((float) h / chunkHeight);
	int pl = (int) floor((float) l / chunkLength);

	int cw = w % chunkWidth;
	int ch = h % chunkHeight;
	int cl = l % chunkLength;

	if(pw>=0 && pw<chunksW && ph>=0 && ph<chunksH && pl>=0 && pl<chunksL) {
		return chunks[chunkIndex(pw, ph, pl)]->getDensity(cw, ch, cl);
	} else {
		return -0.0001;
	}
	
}



int VoxelMap::index(int w, int h, int l) {
	return l * (width * height) + h * width + w;
}



int VoxelMap::chunkIndex(int w, int h, int l) {
	return l * (chunksW * chunksH) + h * chunksW + w;
}