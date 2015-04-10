#ifndef TEXTURE_H
#define TEXTURE_H

#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include "lodepng.h"



class Texture {

    public:      

		Texture(char filename_[]);
		GLuint getData();

	private:
		GLuint data;

};



#endif