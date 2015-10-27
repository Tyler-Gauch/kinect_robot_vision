#include "main.h"

bool initKinect()
{
	int numSensors;

	//stop if we don't find a sensor
	if (NuiGetSensorCount(&numSensors) < 0 || numSensors < 1)
	{
		return false;
	}

	//stop if we were not able to create the sensor
	//Assumes that the user wants to use the first kinect found
	//or that there is only one sensor connected
	if (NuiCreateSensorByIndex(0, &sensor) < 0)
	{
		return false;
	}

	//Initialize the sensor
	sensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_COLOR);
	sensor->NuiImageStreamOpen(
		NUI_IMAGE_TYPE_COLOR,			//get the color images
		NUI_IMAGE_RESOLUTION_640x480,	//Image resolution
		0,								//Image stream flags e.g near mode
		2,								//number of frames to buffer
		NULL,							//event handle
		&rgbStream);
	
	sensor->NuiImageStreamOpen(
		NUI_IMAGE_TYPE_DEPTH,			//Get the depth images
		NUI_IMAGE_RESOLUTION_640x480,	//Image resolution
		0,								//Image stream flags e.g near mode
		2,								//number of frames to buffer
		NULL,							//Event handle
		&depthStream);

	return true;
}

void getColorData(GLubyte * dest)
{
	float* fdest = (float*)dest;
	memset(fdest, 0, sizeof(dest));
	long* depth2rgb = (long*)depthToRgbMap;
	NUI_IMAGE_FRAME imageFrame;
	NUI_LOCKED_RECT LockedRect;
	if (sensor->NuiImageStreamGetNextFrame(rgbStream, 0, &imageFrame) < 0) return;
	INuiFrameTexture* texture = imageFrame.pFrameTexture;
	texture->LockRect(0, &LockedRect, NULL, 0);
	if (LockedRect.Pitch != 0) {
		const BYTE* start = (const BYTE*)LockedRect.pBits;
		for (int j = 0; j < kinect_height; ++j) {
			for (int i = 0; i < kinect_width; ++i) {
				// Determine rgb color for each depth pixel
				long x = *depth2rgb++;
				long y = *depth2rgb++;
				// If out of bounds, then don't color it at all
				if (x < 0 || y < 0 || x > kinect_width || y > kinect_height) {
					for (int n = 0; n < 3; ++n) *(fdest++) = 0.0f;
				}
				else {
					const BYTE* curr = start + (x + kinect_width*y) * 4;
					for (int n = 0; n < 3; ++n) *(fdest++) = curr[2 - n] / 255.0f;
				}

			}
		}
	}
	texture->UnlockRect(0);
	sensor->NuiImageStreamReleaseFrame(rgbStream, &imageFrame);
}

void getDepthData(GLubyte * dest)
{
	float * fdest = (float *)dest;
	memset(fdest, 0, sizeof(dest));
	long * depth2Rgb = (long *)depthToRgbMap;

	NUI_IMAGE_FRAME imageFrame;
	NUI_LOCKED_RECT lockedRect;

	//if we can't get the image stop
	if (sensor->NuiImageStreamGetNextFrame(depthStream, 0, &imageFrame) < 0)
	{
		return;
	}

	INuiFrameTexture * texture = imageFrame.pFrameTexture;
	texture->LockRect(0,&lockedRect, NULL, 0);
	if (lockedRect.Pitch != 0)
	{
		const USHORT * curr = (const USHORT *)lockedRect.pBits;
		for (int j = 0; j < kinect_height; ++j)
		{
			for (int i = 0; i < kinect_width; ++i)
			{
				//get depth in millimeters
				USHORT depth = NuiDepthPixelToDepth(*curr++);

				//Store coords of the point
				Vector4 pos = NuiTransformDepthImageToSkeleton(i, j, depth<<3, NUI_IMAGE_RESOLUTION_640x480);
				*fdest++ = pos.x / pos.w;
				*fdest++ = pos.y / pos.w;
				*fdest++ = pos.z / pos.w;

				//store index in the color array
				NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(
					NUI_IMAGE_RESOLUTION_640x480, NUI_IMAGE_RESOLUTION_640x480, NULL,
					i, j, depth<<3, depth2Rgb, depth2Rgb+1
					);

				depth2Rgb += 2;
			}
		}
	}

	texture->UnlockRect(0);
	sensor->NuiImageStreamReleaseFrame(depthStream, &imageFrame);
}

/*void getKinectData() {
	const int dataSize = kinect_width*kinect_height * 3 * 4;
	GLubyte* ptr;
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, dataSize, 0, GL_DYNAMIC_DRAW);
	ptr = (GLubyte*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (ptr) {
		getDepthData(ptr);
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, cboId);
	glBufferData(GL_ARRAY_BUFFER, dataSize, 0, GL_DYNAMIC_DRAW);
	ptr = (GLubyte*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (ptr) {
		getColorData(ptr);
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void drawKinectData()
{
	getKinectData();
	rotateCamera();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, cboId);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glPointSize(1.0f);
	glDrawArrays(GL_POINTS, 0, kinect_width*kinect_height);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}*/


void getKinectData() {
	getDepthData((GLubyte*) vertexarray);
	getColorData((GLubyte*)colorarray);
}
void drawKinectData() {
	getKinectData();
	rotateCamera();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBegin(GL_POINTS);
	for (int i = 0; i < kinect_width*kinect_height; ++i) {
		glColor3f(colorarray[i * 3], colorarray[i * 3 + 1], colorarray[i * 3 + 2]);
		glVertex3f(vertexarray[i * 3], vertexarray[i * 3 + 1], vertexarray[i * 3 + 2]);
	}
	glEnd();
}

void rotateCamera()
{
	static double angle = 0;
	static double radius = 3;
	double x = radius*sin(angle);
	double z = radius*(1 - cos(angle)) - radius / 2;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(x,0,z,0,0,radius/2,0,1,0);
	angle += 0.05;
}

void draw()
{
	drawKinectData();
	glutSwapBuffers();
}

void execute()
{
	glutMainLoop();
}

bool initGlut(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(kinect_width, kinect_height);
	glutCreateWindow("Kinect SDK Tutorial");
	glewInit();
	glutDisplayFunc(draw);
	glutIdleFunc(draw);
	return true;
}

bool initOpenGL()
{
	// OpenGL setup
	glClearColor(0, 0, 0, 0);
	glClearDepth(1.0f);

	//set up array buffers
	glGenBuffers(1, &vboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glGenBuffers(1, &cboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);

	// Camera setup
	glViewport(0, 0, kinect_width, kinect_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, kinect_width/(GLdouble) kinect_height, 0.1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0,0,0,0,0,1,0,1,0);

	return true;
}

int main(int argc, char * argv[])
{
	if (!initGlut(argc, argv))
	{
		return -1;
	}
	if (!initKinect())
	{
		return -2;
	}

	initOpenGL();
	execute();
	return 0;
}