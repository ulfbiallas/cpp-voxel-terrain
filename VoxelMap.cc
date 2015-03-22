#include "VoxelMap.h"



VoxelMap::VoxelMap(HeightMap *heightMap) {

	width = heightMap->getWidth();
	length = heightMap->getLength();
	height = 32;
	voxelSize = 0.2f;

	data = (float*) malloc(width * height * length * sizeof(float));

	int w,h,l;
	for (w=0; w<width; ++w) {
		for(h=0; h<height; ++h) {
			for (l=0; l<length; ++l) {	

				data[index(w, h, l)] = calculateDensityFromHeightMap(heightMap, w, h, l);
				if(data[index(w, h, l)] < -1) data[index(w, h, l)] = -1;
				if(data[index(w, h, l)] >  1) data[index(w, h, l)] =  1;

			}
		}
	}

	marchingCuber = new MarchingCuber();
	extractSurface();
}



VoxelMap::~VoxelMap() {
	delete[] data;
}



void VoxelMap::extractSurface() {
	triangles = marchingCuber->extractSurface(&data, Vec3f(0,0,0), width, height, length, voxelSize, 0.0f);
}



void VoxelMap::reduceDensityAtPoint(Vec3f point) {

	float radius = 5;
	float reductionFactor = 0.1;

	float radius2 = radius * radius;
	float distance;
	float dDensity;
	int rx,ry,rz, w, h, l;
	Vec3f p = point.mult(1.0 / voxelSize);

	for(rx=-radius; rx<=radius; ++rx) {
		for(ry=-radius; ry<=radius; ++ry) {
			for(rz=-radius; rz<=radius; ++rz) {

				distance = sqrt((float) (rx*rx + ry*ry + rz*rz));

				dDensity = distance - radius;
				dDensity *= reductionFactor;
				if(dDensity > 0) dDensity = 0;
				if(dDensity < -1) dDensity = -1;
				
				w = p.x + rx;
				h = p.y + ry;
				l = p.z + rz;

				if(w>=0 && w<width && h>=0 && h<height && l>=0 && l<length) {
					data[index(w, h, l)] += dDensity;
					if(data[index(w, h, l)] < -1) data[index(w, h, l)] = -1;
					if(data[index(w, h, l)] >  1) data[index(w, h, l)] =  1;
				}

			}
		}
	}
}



float VoxelMap::calculateDensityFromHeightMap(HeightMap *heightMap, int w, int h, int l) {
	int iw, ih, il;
	float density = 0.0f;
	int smoothRadius = 1;

	for(iw=-smoothRadius; iw<=smoothRadius; ++iw) {
		for(ih=-smoothRadius; ih<=smoothRadius; ++ih) {
			for(il=-smoothRadius; il<=smoothRadius; ++il) {
				density += getVerticalDistanceFromHeightMap(heightMap, w+iw, h+ih, l+il);
			}
		}
	}
	density /= (float) ((2*smoothRadius+1) * (2*smoothRadius+1) * (2*smoothRadius+1));

	return density;
}



float VoxelMap::getVerticalDistanceFromHeightMap(HeightMap *heightMap, int w, int h, int l) {
	float heightInHeightMap = height * heightMap->getSmoothedHeight(w, l) / 255.0f;
	float distance = heightInHeightMap - h;
	if(distance < -1) distance = -1;
	if(distance >  1) distance =  1;
	return distance;
}



float VoxelMap::getDensity(int w, int h, int l) {
	if(w>=0 && w<width && h>=0 && h<height && l>=0 && l<length) {
		return data[index(w, h, l)];
	} else {
		return 0;
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



std::vector<MarchingCuber::TRIANGLE> VoxelMap::getTriangles() {
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



int VoxelMap::index(int w, int h, int l) {
	return l * (width * height) + h * width + w;
}