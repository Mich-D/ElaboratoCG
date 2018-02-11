#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>

#include <GL\glew.h>
#include <GL\freeglut.h>

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 600

#define WORLD_WIDTH 400.0
#define WORLD_HEIGHT 250.0

#define PI 3.1415926535
#define FRAMETIME 13.33

using namespace std;

float t = 0.0;
float bgR, bgG, bgB;
float xboat = 0.0, yboat = -44.0, boatAcceleration = 0.0, boatAngle = 0.0;
int dirboat = 0;
bool wireframe, daylight;

void init()
{
	bgR = 0.2;
	bgG = 0.6;
	bgB = 1.0;

	wireframe = false;
	daylight = true;
}

void drawBoat() {
	glColor3f(1.0, 0.95, 0.9);

	glBegin(GL_POLYGON);
		glVertex2f(-20.0, -5.0);
		glVertex2f(  0.0, 22.5);
		glVertex2f(  0.0, -5.0);
	glEnd();

	glLineWidth(4.0);
	glColor3f(0.55, 0.33, 0.2);

	glBegin(GL_LINES);
		glVertex2f(0.0, -7.5);
		glVertex2f(0.0, 22.5);
	glEnd();

	glBegin(GL_POLYGON);
		glVertex2f(-15.0,  -22.5);
		glVertex2f(-20.0, -7.5);
		glVertex2f(20.0, -7.5);
		glVertex2f(15.0,  -22.5);
	glEnd();

	glLineWidth(1.0);
	glColor3f(0.0, 0.0, 0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_POLYGON);
		glVertex2f(-15.0, -22.5);
		glVertex2f(-20.0, -7.5);
		glVertex2f(20.0, -7.5);
		glVertex2f(15.0, -22.5);
	glEnd();

	if (!wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void drawWave(float yValue, float height, float speed, float blueAmount)
{
	float x, y, prevx, prevy;
	float k = 2 * PI / 100.0;
	float c, a;

	prevx = -(WORLD_WIDTH / 2);
	prevy = 2.0 * sin(k * (prevx + (t * speed))) + yValue;

	glColor3f(0.0, 0.0, blueAmount);
	glBegin(GL_TRIANGLE_STRIP);
	for (x = -(WORLD_WIDTH / 2); x <= (WORLD_WIDTH / 2); x += 5.0) {
		y = 2.0 * sin(k * (x + (t * speed))) + yValue;
		glVertex2f(x, y - height);
		glVertex2f(x, y);

		if (xboat >= prevx && xboat <= x && yValue == -57.5) {
			c = sqrt((x - prevx)*(x - prevx) + (y - prevy)*(y - prevy));
			a = y - prevy;
			boatAngle = ((float)asin(a / c)) * 180 / PI;
			yboat += (a * 0.1);
		}
		prevx = x;
		prevy = y;
	}
	glEnd();
}

void drawScene(void)
{
	glClearColor(bgR, bgG, bgB, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (!wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	//drawWave(-50, 50, 0.50, 0.8);
	glPushMatrix();
		glTranslatef(xboat, yboat, 0.0);
		glRotatef(boatAngle, 0.0, 0.0, 1.0);
		drawBoat();
	glPopMatrix();
	drawWave(-57.5, 50, 0.75, 0.7);
	drawWave(-70, 50, 1.00, 0.6);
	drawWave(-90, 50, 1.25, 0.5);

	glutSwapBuffers();

	t++;
}

void keyDown(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	case 'a':
		dirboat = -1;
		if (boatAcceleration < 1.0) {
			boatAcceleration += 0.1;
		}
		xboat -= (1.0 * boatAcceleration);
		if (xboat <= (-WORLD_WIDTH / 2) + 20) {
			xboat = -WORLD_WIDTH / 2 + 20;
		}
		break;
	case 'd':
		dirboat = 1;
		if (boatAcceleration < 1.0) {
			boatAcceleration += 0.1;
		}
		xboat += (1.0 * boatAcceleration);
		if (xboat >= WORLD_WIDTH / 2 - 20) {
			xboat = WORLD_WIDTH / 2 - 20;
		}
		break;
	default:
		break;
	}
}

void keyUp(unsigned char key, int x, int y) {


}

void resize(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-(WORLD_WIDTH / 2), WORLD_WIDTH / 2, -(WORLD_HEIGHT / 2), WORLD_HEIGHT / 2);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void timer(int value) {
	t++;
	
	glutPostRedisplay();
	glutTimerFunc(FRAMETIME, timer, -1);
}

void idle(void) {
	glutPostRedisplay();
}

void menu(int id) {
	switch (id) {
		case 1:
			if (daylight) {
				bgR = 0.0;
				bgG = 0.0;
				bgB = 0.4;
			} else {
				bgR = 0.2;
				bgG = 0.6;
				bgB = 1.0;
			}
			daylight = !daylight;
			glutPostRedisplay();
			break;
		case 2:
			wireframe = !wireframe;
			glutPostRedisplay();
			break;
		default:
			break;
	}
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
	glutIdleFunc(idle);
	//glutMouseFunc(mouse);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyDown);
	//glutKeyboardFunc(keyUp);
	//glutTimerFunc(FRAMETIME, timer, -1);

	glutCreateMenu(menu);
	glutAddMenuEntry("Giorno/Notte", 1);
	glutAddMenuEntry("Wireframe", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glewExperimental = GL_TRUE;
	glewInit();

	init();
	glutMainLoop();

	return 0;
}