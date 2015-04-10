#include "Texture.h"



Texture::Texture(char filename_[]) {

	int texWidth, texHeight;
	unsigned error;
	unsigned char* image;
	unsigned texWidthc, texHeightc;
	error = lodepng_decode32_file(&image, &texWidthc, &texHeightc, filename_);
	texWidth = texWidthc;
	texHeight = texHeightc;
	std::vector<unsigned char> imgData(texWidth * texHeight * 4);
	int x, y, c;
	int r,g,b,a, idx;
	int grayval;
	for(y = 0; y < texHeight; y++) {
	  for(x = 0; x < texWidth; x++) {
	 	for(c = 0; c < 4; c++) {
		 imgData[4 * texWidth * y + 4 * x + c] = image[4 * texWidth * y + 4 * x + c];
  		}
	  }
	}
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures( 1, &data );
	glBindTexture( GL_TEXTURE_2D, data );
	glTexImage2D(GL_TEXTURE_2D, 0, 4, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &imgData[0]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

}



GLuint Texture::getData() {
	return data;
}