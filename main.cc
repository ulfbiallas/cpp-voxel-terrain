

#include <windows.h>
#include <iostream>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <ctime>

#include "datatypes.h"
#include "HeightMap.h"


#define PI 3.1415926535

using namespace std;

bool flag_up = false;
bool flag_down = false;
bool flag_left = false;
bool flag_right = false;
bool flag_m = false;
bool mousedown = false;
bool mouserightdown = false;
float phi = 90;
float phi_alt = 0;
float theta = 0;
float theta_alt = 0;
float walkspeed = 0.02f;
Vec3f mouse_pos;
Vec3f mouse_pos_alt;
Vec3f viewer_pos = Vec3f(0, 0, -10);
Vec3f viewer_dir;

GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat light_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat light_position[] = { 100.0, 100.0, 100.0, 1.0 };

HeightMap *heightMap;



int main(int argc, char ** argv);
void display();
void reshape(int width, int height);

void mouse(int button, int state, int mx, int my);
void motion (int x, int y);
void keyboardDown(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);


float inline random() {
	return rand() / double(RAND_MAX);
}



int main(int argc, char ** argv) {

	srand((unsigned)time(NULL));

	heightMap = new HeightMap("heightmap.png");

	glutInit(&argc, argv);
	glutInitWindowSize(640, 480);
    glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow ("voxel terrain");
    glEnable(GL_DEPTH_TEST);

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
	glColor3f(0,0,0);



	if(heightMap->wasSuccessful()) {
		int ix, iy;
		float blockSize = 0.2f;
		float heightFactor = 0.01;

		glBegin(GL_QUADS);
		for (iy=0; iy<heightMap->getLength(); ++iy) {
			for (ix=0; ix<heightMap->getWidth(); ++ix) {

				glVertex3f(blockSize * (ix + 0), heightFactor * heightMap->getHeight(ix+0, iy+0), blockSize * (iy + 0));
				glVertex3f(blockSize * (ix + 1), heightFactor * heightMap->getHeight(ix+1, iy+0), blockSize * (iy + 0));
				glVertex3f(blockSize * (ix + 1), heightFactor * heightMap->getHeight(ix+1, iy+1), blockSize * (iy + 1));
				glVertex3f(blockSize * (ix + 0), heightFactor * heightMap->getHeight(ix+0, iy+1), blockSize * (iy + 1));

			}
		}
		glEnd();
	}



	glutSwapBuffers();
	glutPostRedisplay();
}



void reshape(int width, int height) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, width, height);
	gluPerspective(70, (float) width/ (float) height, 2 , 2000);
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
	if(mousedown) {
		mouse_pos.x = x;
		mouse_pos.y = y;
		Vec3f delta = mouse_pos.sub(mouse_pos_alt);
		phi = phi_alt + 0.2f * delta.x;
		theta = theta_alt - 0.2f * delta.y;
	}
}


void mouse(int button, int state, int x, int y) {
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
	}
}

