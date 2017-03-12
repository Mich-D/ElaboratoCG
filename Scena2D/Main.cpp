#include <cstdio>
#include <cstdlib>
#include <GL\glew.h>
#include <GL\freeglut.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

int main(int argc, char** argv) {

	glutInit(&argc, argv);

	glutInitContextVersion(4, 2);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("Scena2D");

	/*
	glutDisplayFunc(drawScene);
	glutMouseFunc(mouse);
	glutReshapeFunc(resize);
	glutKeyboardFunc(mykeyboard);
	*/

	glewExperimental = GL_TRUE;
	glewInit();

	return 0;
}