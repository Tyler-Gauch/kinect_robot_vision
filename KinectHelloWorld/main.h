#ifndef _MAIN_
#define _MAIN_
/*
	Open GL Header Files
	also required download of freeglut
*/
#include <GL/glew.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <GL/glut.h>
#include "KinectSensor.h"

#define kinect_width 640
#define kinect_height 480

//OpenGL Variables
GLuint textureId;								//ID of the texture to contain the Kinect RGB Data
GLubyte data[kinect_width*kinect_height * 4];	//BGRA array containing the texture data

GLuint vboId;
GLuint cboId;

float colorarray[kinect_width*kinect_height * 3];
float vertexarray[kinect_width*kinect_height * 3];

//speed to move the camera at
float cameraSpeed = 1;
// angle of rotation for the camera direction
float angle = 0.0;
float radius = 3;
// XZ position of the camera
float x = radius * sin(angle);
float z = radius * (1 - cos(angle)) - radius / 2;
float y = 1;
float lx = 0.0f, lz = 1.0f;
float deltaAngle = 0.0f;
float deltaYAngle = 0.0f;
int xOrigin = -1;
int yOrigin = -1;



KinectSensor kinect;
/*
	gets the data to draw to the point cloud
*/
void getKinectData();


/*
	Draws a kinect frame to the screen
*/
void drawKinectData();

/*
	the main GLUT rendering loop
*/
void draw();

/*
	main GLUT loop
*/
void execute();

/*
	GLUT Initialization code
*/
bool initGlut(int argc, char* argv[]);

/*
	OpenGL Initialization Code
*/
bool initOpenGL();

/*
	Handle Keyboard input
*/
void processKeys(int key, int xx, int yy);

void mouseButton(int button, int state, int x, int y);

void mouseMove(int x, int y);

#endif