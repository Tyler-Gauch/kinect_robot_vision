#include "main.h"

void getKinectData() {
	kinect.getDepthData((GLubyte*) vertexarray);
	kinect.getColorData((GLubyte*)colorarray);
}
void drawKinectData() {
	getKinectData();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBegin(GL_POINTS);
	for (int i = 0; i < kinect_width*kinect_height; ++i) {
		glColor3f(colorarray[i * 3], colorarray[i * 3 + 1], colorarray[i * 3 + 2]);
		glVertex3f(vertexarray[i * 3], vertexarray[i * 3 + 1], vertexarray[i * 3 + 2]);
	}
	glEnd();
}

void draw()
{
	drawKinectData();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(x, y, z, 0, 0, radius / 2, 0, 1, 0);
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
	glutSpecialFunc(processKeys);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);
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
	gluPerspective(45, kinect_width / (GLdouble)kinect_height, 0.1, 1000);

	// OpenGL init
	glEnable(GL_DEPTH_TEST);

	return true;
}

void mouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_UP)
		{
			angle += deltaAngle;
			xOrigin = -1;
		}
		else
		{
			xOrigin = x;
		}
	}
}

void mouseMove(int xx, int yy)
{
	if (xOrigin >= 0)
	{
		//update the deltaAngle
		deltaAngle = ((xx - xOrigin)) * 0.001f;
		//update cameras direction
		x = radius * sin(angle + deltaAngle);
		z = radius * (1 - cos(angle + deltaAngle)) - radius / 2;
	}
}

void processKeys(int key, int xx, int yy)
{
	float fraction = .1f;
	switch (key)
	{
		case GLUT_KEY_RIGHT:
			kinect.move(5);
			break;
		case GLUT_KEY_LEFT:
			kinect.move(-5);
			break;
		case GLUT_KEY_UP:
			x += lx * fraction;
			z += lz * fraction;
			break;
		case GLUT_KEY_DOWN:
			x -= lx * fraction;
			z -= lz * fraction;
			break;
	}
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