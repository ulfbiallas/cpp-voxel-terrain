#include "Shader.h"



Shader::Shader() {

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
	glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);

	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, fragmentShader);
	glAttachShader(shaderProgram, vertexShader);
	glLinkProgram(shaderProgram);

}



GLuint Shader::getProgram() {
	return shaderProgram;
}