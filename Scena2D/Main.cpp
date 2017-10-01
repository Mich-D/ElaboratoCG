#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <GL\glew.h>
#include <GL\freeglut.h>

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 600

#define WORLD_SIZE 200.0

#define PI 3.1415926535

using namespace std;

float t = 0;

void init()
{

}

void drawBoat() {
	glColor3f(1.0, 0.95, 0.9);

	glBegin(GL_POLYGON);
		glVertex3f(-15.0, 37.5, 0.0);
		glVertex3f(  0.0, 65.0, 0.0);
		glVertex3f(  0.0, 37.5, 0.0);
	glEnd();

	glLineWidth(4.0);
	glColor3f(0.55, 0.33, 0.2);

	glBegin(GL_LINES);
		glVertex3f(0.0, 35.0, 0.0);
		glVertex3f(0.0, 65.0, 0.0);
	glEnd();

	glBegin(GL_POLYGON);
		glVertex3f(-15.0, 20.0, 0.0);
		glVertex3f(-20.0, 35.0, 0.0);
		glVertex3f( 20.0, 35.0, 0.0);
		glVertex3f( 15.0, 20.0, 0.0);
	glEnd();

	glLineWidth(1.0);
	glColor3f(0.0, 0.0, 0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_POLYGON);
		glVertex3f(-15.0, 20.0, 0.0);
		glVertex3f(-20.0, 35.0, 0.0);
		glVertex3f(20.0, 35.0, 0.0);
		glVertex3f(15.0, 20.0, 0.0);
	glEnd();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}

void drawWave(float yValue, float height, float offset, float speed, float blueAmount)
{
	float x;

	glColor3f(0.0, 0.0, blueAmount);
	glBegin(GL_TRIANGLE_STRIP);
		for (x = -(WORLD_SIZE/2); x <= (WORLD_SIZE/2); x += 5) {
			float k = 2 * PI / 100.0;
			float y = 5.0 * sin(k * (x + (t * speed) + offset)) + yValue;

			glVertex3f(x, y - height, 0.0);
			glVertex3f(x, y, 0.0);
		}
	glEnd();
}

void drawScene(void)
{
	glClearColor(0.2, 0.6, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	drawBoat();

	drawWave(-25, 50,  0, 0.50, 0.8);
	drawWave(-50, 50, 25, 0.75, 0.6);
	drawWave(-75, 50, 50, 1.00, 0.4);

	glutSwapBuffers();
	glFlush();
}

void listenKeyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	}
}


void resize(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-(WORLD_SIZE / 2), WORLD_SIZE / 2, -(WORLD_SIZE / 2), WORLD_SIZE / 2, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void timer(int value) {
	t++;
	glutPostRedisplay();
	glutTimerFunc(20, timer, 1);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 2);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("Scena2D");

	glutDisplayFunc(drawScene);
	//glutMouseFunc(mouse);
	glutReshapeFunc(resize);
	glutKeyboardFunc(listenKeyboard);
	glutTimerFunc(20, timer, 1);

	glewExperimental = GL_TRUE;
	glewInit();

	init();
	glutMainLoop();

	return 0;
}