/*
	Windows SDK files
*/
#include <cmath>
#include <cstdio>
#include <Windows.h>
#include <Ole2.h>

/*
	Open GL Header Files
	also required download of freeglut
*/
#include <GL/glew.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <GL/glut.h>

/*
	Kinect SDK header files
*/
#include <NuiApi.h>
#include <NuiImageCamera.h>
#include <NuiSensor.h>

#define kinect_width 640
#define kinect_height 480

//OpenGL Variables
GLuint textureId;								//ID of the texture to contain the Kinect RGB Data
GLubyte data[kinect_width*kinect_height * 4];	//BGRA array containing the texture data

//Kinect Variables

HANDLE rgbStream;								//the identifier of the Kinects RGB Camera
HANDLE depthStream;								//the identifier of the kinects depth camera
INuiSensor * sensor;							//pointer to the kinect sensor

long depthToRgbMap[kinect_width*kinect_height*2];//holds the cross of rgb and depth pixels so we know which picels go with each other

GLuint vboId;
GLuint cboId;

float colorarray[kinect_width*kinect_height * 3];
float vertexarray[kinect_width*kinect_height * 3];

/*
	Initialize the Kinect sensor with the 
	flags and specifications we want
*/
bool initKinect();

/*
	Get a frame from the sensor
	this requiers us to fetch and lock the frame so
	that it doesn't get corrupted while reading it
*/
void getColorData(GLubyte * dest);

/*
	Gets the depth data and maps it to the color data
*/
void getDepthData(GLubyte * dest);

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