#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <cstdlib>
#include <iostream>
#include <vector>
#include "lodepng.h"
#include "datatypes.h"



class HeightMap {

    public:      

		HeightMap(char *heightMapFile_);
		~HeightMap();

		int getWidth();
		int getLength();
		int getHeight(int w, int l);
		int getMaxHeight();
		bool wasSuccessful();



	private:

		struct Image {
			unsigned char* data;
			unsigned width;
			unsigned height;
		};

		struct Map {
			unsigned char* data;
			unsigned width;
			unsigned length;
		};

		Map *heightMap;
		bool hasError;
		unsigned char maxHeight;

		unsigned loadImage(Image *image, char *filename);
		void createHeightMapFromImage(Image *image);

};



#endif