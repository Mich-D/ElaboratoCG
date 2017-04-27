#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <GL\glew.h>
#include <GL\freeglut.h>

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540

#define PI 3.1415926535

void makeWave(float width, float height, int vertices) {
	int vert = vertices < 3 ? 3 : vertices;
	float i, step = width / vert;

	// Riguardare
	glBegin(GL_LINE_STRIP);
	for (i = 0; i < width; i+=step) {
		glVertex3f(i, height*sin(i), 0.0);
	}
	glEnd();
}

void drawScene(void) {
	glClearColor(0.5, 0.5, 0.5, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1.0, 1.0, 1.0);
	glPushMatrix();
		glBegin(GL_POLYGON);
			glVertex3f(-50, -50, 0.0);
			glVertex3f(50, -50, 0.0);
			glVertex3f(0, 66.7, 0.0);
		glEnd();
	glPopMatrix();

	glColor3f(0.0, 0.0, 1.0);
	glPushMatrix();
		makeWave(100.0, 50.0, 50.0);
	glPopMatrix();

	glFlush();
}

void resize(int w, int h) {
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-100.0, 100.0, -100.0, 100.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char** argv) {

	glutInit(&argc, argv);

	glutInitContextVersion(4, 2);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("Scena2D");

	glutDisplayFunc(drawScene);
	//glutMouseFunc(mouse);
	glutReshapeFunc(resize);
	//glutKeyboardFunc(mykeyboard);

	glewExperimental = GL_TRUE;
	glewInit();

	glutMainLoop();

	return 0;
}