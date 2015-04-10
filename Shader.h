#ifndef SHADER_H
#define SHADER_H

#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include "ShaderCode.h"



class Shader {

    public:      

		Shader();
		GLuint getProgram();


	private:

		GLuint vertexShader, fragmentShader, shaderProgram;

};



#endif