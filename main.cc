

#include <windows.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <ctime>

#include "lodepng.h"
#include "datatypes.h"
#include "HeightMap.h"
#include "VoxelMap.h"


#define Shader(shaderCode) #shaderCode
#define PI 3.1415926535

using namespace std;

bool flag_up = false;
bool flag_down = false;
bool flag_left = false;
bool flag_right = false;
bool flag_m = false;
bool mousedown = false;
bool mouserightdown = false;
int mouseX = 0;
int mouseY = 0;
float phi = 90;
float phi_alt = 0;
float theta = 0;
float theta_alt = 0;
float walkspeed = 0.05f;
Vec3f mouse_pos;
Vec3f mouse_pos_alt;
Vec3f viewer_pos = Vec3f(0, 0, -10);
Vec3f viewer_dir;

VoxelMap::DENSITY_CHANGE_METHOD densityChangeMethod = VoxelMap::DECREASE;

GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat light_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat light_position[] = { 100.0, 100.0, 100.0, 1.0 };

GLdouble matProjection[16];
GLdouble matModelview[16];
GLint viewport[4];

HeightMap *heightMap;
VoxelMap *voxelMap;

GLuint texture;
GLuint vertexShader, fragmentShader, shaderProgram;



const char* vertexShaderSrc = Shader(
	varying vec4 position;
	varying vec3 normal;
	varying vec3 transformedNormal;

	void main() {
		position = gl_Vertex;
		normal = gl_Normal;
		gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
		transformedNormal = normalize(gl_NormalMatrix * gl_Normal);
	}
);



const char* fragmentShaderSrc = Shader(
	uniform sampler2D texture0;
	varying vec4 position;
	varying vec3 normal;
	varying vec3 transformedNormal;

	vec4 triplanarMapping(sampler2D texture, vec4 position, vec3 normal) {
		vec3 n = abs(normal);
		vec4 colorX = texture2D(texture, position.yz);
		vec4 colorY = texture2D(texture, position.xz);
		vec4 colorZ = texture2D(texture, position.xy);
		return colorX * n.x + colorY * n.y + colorZ * n.z;
	}

	void main(void) {
		vec4 diffuse = triplanarMapping(texture0, position, normal);

		int numLightSources = 1;
		vec4 finalColor = diffuse * 0.2;

		for(int i = 0; i < numLightSources; i++) {
			vec3 lightDir = normalize(vec3(gl_LightSource[i].position));
			float  NdotL = max(dot(transformedNormal, lightDir), 0.0);
			vec4 diffuseC = diffuse * gl_LightSource[i].diffuse * NdotL;
			finalColor += diffuseC;
		}	  

		gl_FragColor = finalColor;
	}
);



int main(int argc, char ** argv);
void display();
void reshape(int width, int height);
void mouse(int button, int state, int mx, int my);
void motion (int x, int y);
void keyboardDown(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
Vec3f getViewDirectionForScreenCoordinates(int x, int y);
void loadTexture(char filename_[], GLuint* texture_);
void createShader();



float inline random() {
	return rand() / double(RAND_MAX);
}



int main(int argc, char ** argv) {

	srand((unsigned)time(NULL));

	heightMap = new HeightMap("heightmap.png");
	voxelMap = new VoxelMap(heightMap);


    glutInit(&argc, argv);
    glutInitWindowSize(640, 480);
    glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow ("voxel terrain");
	glEnable(GL_DEPTH_TEST);

	glewInit();
	if (glewIsSupported("GL_VERSION_2_1")) {
		if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader && GL_EXT_geometry_shader4) {
			createShader();
		}
	}

    loadTexture("texture.png", &texture);

	glLightfv(GL_LIGHT0,GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE, light_diffuse);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
	glutKeyboardFunc(keyboardDown);
	glutKeyboardUpFunc(keyboardUp);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

    glutMainLoop();

    return 0;
}



void display(void) {

	viewer_dir.x = cos(2*PI * phi / 360) * cos(2*PI * theta / 360);
	viewer_dir.y = sin(2*PI * theta / 360);
	viewer_dir.z = sin(2*PI * phi / 360) * cos(2*PI * theta / 360);

	if(flag_up) viewer_pos = viewer_pos.add(viewer_dir.mult(walkspeed));
	if(flag_down) viewer_pos = viewer_pos.sub(viewer_dir.mult(walkspeed));

	if(flag_left) {
		viewer_pos.x += walkspeed * cos(2*PI * (phi-90) / 360);
		viewer_pos.z += walkspeed * sin(2*PI * (phi-90) / 360);
	}
	if(flag_right) {
		viewer_pos.x += walkspeed * cos(2*PI * (phi+90) / 360);
		viewer_pos.z += walkspeed * sin(2*PI * (phi+90) / 360);
	}



	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	if(flag_m) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	gluLookAt(viewer_pos.x, viewer_pos.y, viewer_pos.z,
				viewer_pos.x+viewer_dir.x, viewer_pos.y+viewer_dir.y, viewer_pos.z+viewer_dir.z,
				0, 1, 0);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);



	if(heightMap->wasSuccessful()) {

		if(mouserightdown) {
			Vec3f rayDirection = getViewDirectionForScreenCoordinates(mouseX, mouseY);
			float rayIntersection = voxelMap->intersectRay(viewer_pos, rayDirection);
			Vec3f intersectionPos = viewer_pos.add(rayDirection.mult(rayIntersection));
			voxelMap->changeDensityAtPoint(intersectionPos, densityChangeMethod);
		}

		voxelMap->extractSurface();



		int t, v;
		vector<TRIANGLE> triangles = voxelMap->getTriangles();

		glUseProgram(shaderProgram);

		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		
		glUniform1iARB(glGetUniformLocationARB(shaderProgram, "texture0"), 0); 

		glBegin(GL_TRIANGLES);

		TRIANGLE triangle;
		for(t=0; t<triangles.size(); ++t) {
			triangle = triangles[t];
			for(v=0; v<3; ++v) {
				//glTexCoord2f(triangle.p[v].x, triangle.p[v].z);
				glNormal3f(triangle.n[v].x, triangle.n[v].y, triangle.n[v].z);
				glVertex3f(triangle.p[v].x, triangle.p[v].y, triangle.p[v].z);
			}
			
		}

		glEnd();

		glUseProgram(0); 

		glDisable(GL_TEXTURE_2D);
	}
	
		
	glGetDoublev(GL_PROJECTION_MATRIX, matProjection);
	glGetDoublev(GL_MODELVIEW_MATRIX, matModelview);
	glGetIntegerv(GL_VIEWPORT, viewport);

	glutSwapBuffers();
	glutPostRedisplay();
}



void reshape(int width, int height) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, width, height);
	gluPerspective(70, (float) width/ (float) height, 0.1 , 200);
}



void keyboardDown(unsigned char key, int x, int y) {
    switch (key) {
		case 'w':
		{
			flag_up = true;
			break;
		}
		case 's':
		{
			flag_down = true;
			break;
		}
		case 'a':
		{
			flag_left = true;
			break;
		}
		case 'd':
		{
			flag_right = true;
			break;
		}
		case 'm':
		{
			flag_m = true;
			break;
		}
		case '+':
		{
			densityChangeMethod = VoxelMap::INCREASE;
			break;
		}
		case '-':
		{
			densityChangeMethod = VoxelMap::DECREASE;
			break;
		}
	}
}



void keyboardUp(unsigned char key, int x, int y) {
    switch (key) {
		case 'w':
		{
			flag_up = false;
			break;
		}
		case 's':
		{
			flag_down = false;
			break;
		}
		case 'a':
		{
			flag_left = false;
			break;
		}
		case 'd':
		{
			flag_right = false;
			break;
		}
		case 'm':
		{
			flag_m = false;
			break;
		}
	}
}



void motion (int x, int y) {
	mouseX = x;
	mouseY = y;
	if(mousedown) {
		mouse_pos.x = x;
		mouse_pos.y = y;
		Vec3f delta = mouse_pos.sub(mouse_pos_alt);
		phi = phi_alt + 0.2f * delta.x;
		theta = theta_alt - 0.2f * delta.y;
	}
}



void mouse(int button, int state, int x, int y) {
	mouseX = x;
	mouseY = y;
	switch( button ) {
		case GLUT_LEFT_BUTTON:
			if( state == GLUT_DOWN ) {
				mousedown = true;

				phi_alt = phi;
				theta_alt = theta;
				mouse_pos_alt.x = x;
				mouse_pos_alt.y = y;

			} else if( state == GLUT_UP )  {
				mousedown = false;
			}
			break;

		case GLUT_RIGHT_BUTTON:
			if( state == GLUT_DOWN ) {
				mouserightdown = true;
			} else if( state == GLUT_UP )  {
				mouserightdown = false;
			}

			break;
	}
}



Vec3f getViewDirectionForScreenCoordinates(int x, int y) {
	double realx = x;
	double realy = viewport[3] - (int) y - 1;

	GLdouble wNearX, wNearY, wNearZ;
	gluUnProject( realx, realy, 0.0, matModelview, matProjection, viewport, &wNearX, &wNearY, &wNearZ);

	GLdouble wFarX, wFarY, wFarZ; 
	gluUnProject( realx,  realy, 1.0, matModelview, matProjection, viewport, &wFarX, &wFarY, &wFarZ);

    return Vec3f(wFarX-wNearX, wFarY-wNearY, wFarZ-wNearZ).normalize();
}



void loadTexture(char filename_[], GLuint* texture_) {
	int texWidth, texHeight;
	unsigned error;
	unsigned char* image;
	unsigned texWidthc, texHeightc;
	error = lodepng_decode32_file(&image, &texWidthc, &texHeightc, filename_);
	texWidth = texWidthc;
	texHeight = texHeightc;
	std::vector<unsigned char> data(texWidth * texHeight * 4);
	int x, y, c;
	int r,g,b,a, idx;
	int grayval;
	for(y = 0; y < texHeight; y++) {
	  for(x = 0; x < texWidth; x++) {
	 	for(c = 0; c < 4; c++) {
		 data[4 * texWidth * y + 4 * x + c] = image[4 * texWidth * y + 4 * x + c];
  		}
	  }
	}
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures( 1, texture_ );
	glBindTexture( GL_TEXTURE_2D, *texture_ );
	glTexImage2D(GL_TEXTURE_2D, 0, 4, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}



void createShader() {
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