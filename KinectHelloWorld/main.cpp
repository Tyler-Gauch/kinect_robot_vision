#include "main.h"

void getKinectData() {
	kinect.getDepthData((GLubyte*) vertexarray);
	kinect.getColorData((GLubyte*)colorarray);
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
	if (!kinect.init())
	{
		return -2;
	}

	initOpenGL();
	execute();
	return 0;
}