/*
Windows SDK files
*/
#ifndef _KS_
#define _KS_
#include <cmath>
#include <cstdio>
#include <Windows.h>
#include <Ole2.h>

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

class KinectSensor
{
private:
	//Kinect Variables
	HANDLE rgbStream;								//the identifier of the Kinects RGB Camera
	HANDLE depthStream;								//the identifier of the kinects depth camera
	INuiSensor * sensor;							//pointer to the kinect sensor
	long * depthToRgbMap;//holds the cross of rgb and depth pixels so we know which picels go with each other
	int height;
	int width;
public:
	KinectSensor();
	~KinectSensor();

	HANDLE getRgbStream();
	HANDLE getDepthStream();
	INuiSensor * getSensor();
	int getHeight();
	int getWidth();
	long * getDepthToRgbMap();
	bool init();

	/*
	Get a frame from the sensor
	this requiers us to fetch and lock the frame so
	that it doesn't get corrupted while reading it
	*/
	void getColorData(GLubyte * dest);
	void getDepthData(GLubyte * dest);

	void move(LONG newAngle);
};
#endif