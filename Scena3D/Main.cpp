#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>

#include <GL\glew.h>
#include <GL\freeglut.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#define PI 3.1415926535

using namespace std;
using namespace std::chrono;

const int WINDOW_WIDTH = 720;
const int WINDOW_HEIGHT = 450;
const int FRAMETIME = 1000.0 / 75.0;

GLuint LoadShaders(const char*, const char*);

GLuint programID, VertexArrayID;

GLint modelMtxID, viewMtxID, projMtxID;
glm::mat4 modelMtx, viewMtx, projMtx;

//high_resolution_clock::time_point t_start;

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

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}


void init() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	programID = LoadShaders("color.vert", "color.frag");

	glUseProgram(programID);

	glGenVertexArrays(1, &VertexArrayID);

	GLint posAttrib = glGetAttribLocation(programID, "aPos");
	glutSetVertexAttribCoord3(posAttrib);

	GLint normalAttrib = glGetAttribLocation(programID, "aNormal");
	glutSetVertexAttribNormal(normalAttrib);

	glBindVertexArray(VertexArrayID);

	projMtx = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
	projMtxID = glGetUniformLocation(programID, "projMtx");
	glUniformMatrix4fv(projMtxID, 1, GL_FALSE, &projMtx[0][0]);

	glm::vec3 cameraPos = glm::vec3(4.0, 3.0, -3.0);
	viewMtx = glm::lookAt(
		cameraPos, // Camera worldspace position
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	viewMtxID = glGetUniformLocation(programID, "viewMtx");
	glUniformMatrix4fv(viewMtxID, 1, GL_FALSE, &viewMtx[0][0]);

	modelMtx = glm::mat4(1.0f);
	modelMtxID = glGetUniformLocation(programID, "modelMtx");

	//MVP = projMtx * viewMtx * modelMtx;

	//GLuint lightVAO;
	//glGenVertexArrays(1, &lightVAO);
	//glBindVertexArray(lightVAO);

	GLint lightID = glGetUniformLocation(programID, "lightPos");
	glm::vec3 lightPos = glm::vec3(6.0, 6.0, -6.0);
	glUniform3f(lightID, lightPos.x, lightPos.y, lightPos.z);

	GLint viewPos = glGetUniformLocation(programID, "viewPos");
	glUniform3f(viewPos, cameraPos.x, cameraPos.y, cameraPos.z);

	GLint lightColor = glGetUniformLocation(programID, "lightColor");
	glUniform3f(lightColor, 0.4f, 0.4f, 0.4f);

	GLint objectColor = glGetUniformLocation(programID, "objectColor");
	glUniform3f(objectColor, 0.6f, 0.4f, 0.4f);
}
void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programID);

	// Calculate transformation
	//auto t_now = high_resolution_clock::now();
	//float time = duration_cast<std::chrono::duration<float>>(t_now - t_start).count();

	/*
	glm::mat4 objMtx1 = glm::translate(idMtx, glm::vec3(0.0f, 0.0f, -2.0f));
	glUniformMatrix4fv(modelMtxID, 1, GL_FALSE, &objMtx1[0][0]);

	glutSolidCube(1.0);

	glm::mat4 objMtx2 = glm::translate(idMtx, glm::vec3(2.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelMtxID, 1, GL_FALSE, &objMtx2[0][0]);

	glutSolidCube(1.0);
	*/

	//glm::mat4 scaleMtx = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));

	modelMtx = glm::rotate(
		modelMtx,
		glm::radians(1.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	//glm::mat4 finalMtx = scaleMtx * modelMtx;
	glUniformMatrix4fv(modelMtxID, 1, GL_FALSE, &modelMtx[0][0]);

	glBindVertexArray(VertexArrayID);

	//GLdouble offset[3] = {0.0, 0.0, 0.0};
	//glutSolidSierpinskiSponge(3, offset, 1.0);

	glutSolidOctahedron();
	

	glutSwapBuffers();
}

void timerFunc(int t) {
	glutPostRedisplay();

	glutTimerFunc(FRAMETIME, timerFunc, 0);
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

	glutPostRedisplay();
}

void resize(int w, int h) {
	double ratio = (double)w / (double)h;
	glViewport(0, 0, w, h);
}

int main(int argc, char** argv) {

	//t_start = high_resolution_clock::now();

	glutInit(&argc, argv);

	glutInitContextVersion(4, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	//glutInitContextFlags(GLUT_DEBUG);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("Scena3D");

	glutDisplayFunc(drawScene);
	//glutMouseFunc(mouse);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyDown);
	//glutKeyboardFunc(keyUp);
	glutTimerFunc(FRAMETIME, timerFunc, 0);

	/*
	glutCreateMenu(menu);
	glutAddMenuEntry("Giorno/Notte", 1);
	glutAddMenuEntry("Wireframe", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	*/

	glewExperimental = GL_TRUE;
	glewInit();

	fprintf(stdout, "%s\n", glGetString(GL_VERSION));

	init();
	glutMainLoop();
}