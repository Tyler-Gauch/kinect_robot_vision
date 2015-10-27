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

KinectSensor kinect;
/*
	gets the data to draw to the point cloud
*/
void getKinectData();

/*
	Rotates the camera
*/
void rotateCamera();

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

#endif