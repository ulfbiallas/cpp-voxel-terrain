

#include <windows.h>
#include <iostream>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <ctime>

#include "datatypes.h"
#include "HeightMap.h"
#include "VoxelMap.h"



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

GLdouble matProjection[16];
GLdouble matModelview[16];
GLint viewport[4];

HeightMap *heightMap;
VoxelMap *voxelMap;

Vec3f rayDirection;
float rayIntersection = 0;



int main(int argc, char ** argv);
void display();
void reshape(int width, int height);
void mouse(int button, int state, int mx, int my);
void motion (int x, int y);
void keyboardDown(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
Vec3f getViewDirectionForScreenCoordinates(int x, int y);



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

		int t, v;
		vector<MarchingCuber::TRIANGLE> triangles = voxelMap->getTriangles();

		glBegin(GL_TRIANGLES);
		MarchingCuber::TRIANGLE triangle;
		for(t=0; t<triangles.size(); ++t) {
			triangle = triangles[t];
			for(v=0; v<3; ++v) {
				glNormal3f(triangle.n[v].x, triangle.n[v].y, triangle.n[v].z);
				glVertex3f(triangle.p[v].x, triangle.p[v].y, triangle.p[v].z);
			}
			
		}
		glEnd();

	}


	if(mouserightdown) {
		Vec3f intersectionPos = viewer_pos.add(rayDirection.mult(rayIntersection));
		voxelMap->reduceDensityAtPoint(intersectionPos);
		voxelMap->extractSurface();
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
	if(mouserightdown) {
		rayDirection = getViewDirectionForScreenCoordinates(x,y);
		rayIntersection = voxelMap->intersectRay(viewer_pos, rayDirection);
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

		case GLUT_RIGHT_BUTTON:
			if( state == GLUT_DOWN ) {
				mouserightdown = true;
				rayDirection = getViewDirectionForScreenCoordinates(x,y);
				rayIntersection = voxelMap->intersectRay(viewer_pos, rayDirection);
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