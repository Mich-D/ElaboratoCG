#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <GL\glew.h>
#include <GL\freeglut.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

using namespace std;

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 600

#define WORLD_WIDTH 400.0
#define WORLD_HEIGHT 250.0

#define PI 3.1415926535
#define FRAMETIME 13.33

GLuint LoadShaders(const char*, const char*);

GLuint programID, vao[1];
float ratio = WINDOW_WIDTH / WINDOW_HEIGHT;

void init() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	programID = LoadShaders("color.vert", "color.frag");

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);

	GLint posAttrib = glGetAttribLocation(programID, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glm::mat4 projMat = glm::frustum(-1.0 * ratio, 1.0 * ratio, -1.0, 1.0, 1.0, 100.0);
	GLuint projMatLoc = glGetUniformLocation(programID, "projMat");
	glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, glm::value_ptr(projMat));

	
}

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}
	/*
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
	*/

	return ProgramID;
}

void drawScene(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glUseProgram(programID);

	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	glutSolidDodecahedron();

	glutSwapBuffers();
}

void keyDown(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	default:
		break;
	}
}

void resize(int w, int h) {
	ratio = (float)w / (float)h;

	glViewport(0, 0, w, h);

	/*
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glFrustum(-1.0 * ratio, 1.0 * ratio, -1.0, 1.0, 1.0, 100.0);
	*/
}

int main(int argc, char** argv) {

	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("Scena3D");

	glutDisplayFunc(drawScene);
	//glutIdleFunc(drawScene);
	//glutMouseFunc(mouse);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyDown);
	//glutKeyboardFunc(keyUp);
	//glutTimerFunc(FRAMETIME, timer, -1);

	/*
	glutCreateMenu(menu);
	glutAddMenuEntry("Giorno/Notte", 1);
	glutAddMenuEntry("Wireframe", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	*/

	glewExperimental = GL_TRUE;
	glewInit();

	init();
	glutMainLoop();

	return 0;
}