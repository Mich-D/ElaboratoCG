#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <GL\glew.h>
#include <GL\freeglut.h>

#include "DataTypes.h"

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540

using namespace std;

void init() {

}

void drawScene(void) {
	float i;

	glClearColor(0.25, 0.5, 0.75, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0.1, 0.1, 1.0);
	glPushMatrix();
	glTranslatef(-100.0, 0.0, 0.0);
	glBegin(GL_LINE_STRIP);
	for (i = 0; i < 200; i++) {
		glVertex3f(i, sin(i), 0.0);
	}
	glEnd();
	glPopMatrix();

	glFlush();
}

void resize(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-100.0, 100.0, -100.0, 100.0, -1.0, 1.0);
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

	init();
	glutMainLoop();

	return 0;
}