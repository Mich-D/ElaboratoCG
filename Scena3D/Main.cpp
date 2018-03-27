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


#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540

#define WORLD_WIDTH 400.0
#define WORLD_HEIGHT 250.0

#define PI 3.1415926535
#define FRAMETIME 13.33

using namespace std;

GLuint LoadShaders(const char*, const char*);

GLuint programID, mvpID, vbo;
glm::mat4 MVP;

static GLfloat cube_v[] =
{
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f
};

static GLfloat cube_n[] =
{
	 0.0f,  0.0f,  1.0f,
	 1.0f,  0.0f,  0.0f,
	 0.0f,  1.0f,  0.0f,
	-1.0f,  0.0f,  0.0f,
	 0.0f, -1.0f,  0.0f,
	 0.0f,  0.0f, -1.0f
};

#define DODECAHEDRON_NUM_VERT           20
#define DODECAHEDRON_NUM_FACES          12
#define DODECAHEDRON_NUM_EDGE_PER_FACE  5
#define DODECAHEDRON_VERT_PER_OBJ       (DODECAHEDRON_NUM_FACES*DODECAHEDRON_NUM_EDGE_PER_FACE)

static GLfloat dodecahedron_v[DODECAHEDRON_NUM_VERT * 3] =
{
     0.0f,            1.61803398875f,  0.61803398875f,
    -1.0f,            1.0f,            1.0f,
    -0.61803398875f,  0.0f,            1.61803398875f,
     0.61803398875f,  0.0f,            1.61803398875f,
     1.0f,            1.0f,            1.0f,
     0.0f,            1.61803398875f, -0.61803398875f,
     1.0f,            1.0f,           -1.0f,
     0.61803398875f,  0.0f,           -1.61803398875f,
    -0.61803398875f,  0.0f,           -1.61803398875f,
    -1.0f,            1.0f,           -1.0f,
     0.0f,           -1.61803398875f,  0.61803398875f,
     1.0f,           -1.0f,            1.0f,
    -1.0f,           -1.0f,            1.0f,
     0.0f,           -1.61803398875f, -0.61803398875f,
    -1.0f,           -1.0f,           -1.0f,
     1.0f,           -1.0f,           -1.0f,
     1.61803398875f, -0.61803398875f,  0.0f,
     1.61803398875f,  0.61803398875f,  0.0f,
    -1.61803398875f,  0.61803398875f,  0.0f,
    -1.61803398875f, -0.61803398875f,  0.0f
};

/* Normal Vectors */
static GLfloat dodecahedron_n[DODECAHEDRON_NUM_FACES * 3] =
{
     0.0f,             0.525731112119f,  0.850650808354f,
     0.0f,             0.525731112119f, -0.850650808354f,
     0.0f,            -0.525731112119f,  0.850650808354f,
     0.0f,            -0.525731112119f, -0.850650808354f,

     0.850650808354f,  0.0f,             0.525731112119f,
    -0.850650808354f,  0.0f,             0.525731112119f,
     0.850650808354f,  0.0f,            -0.525731112119f,
    -0.850650808354f,  0.0f,            -0.525731112119f,

     0.525731112119f,  0.850650808354f,  0.0f,
     0.525731112119f, -0.850650808354f,  0.0f,
    -0.525731112119f,  0.850650808354f,  0.0f,
    -0.525731112119f, -0.850650808354f,  0.0f
};

/* Vertex indices */
static GLubyte dodecahedron_vi[DODECAHEDRON_VERT_PER_OBJ] =
{
    0,  1,  2,  3,  4,
    5,  6,  7,  8,  9,
    10, 11,  3,  2, 12,
    13, 14,  8,  7, 15,

    3, 11, 16, 17,  4,
    2,  1, 18, 19, 12,
    7,  6, 17, 16, 15,
    8, 14, 19, 18,  9,

    17,  6,  5,  0,  4,
    16, 11, 10, 13, 15,
    18,  1,  0,  5,  9,
    19, 14, 13, 10, 12
};

void init() {
	glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    programID = LoadShaders("color.vert", "color.frag");

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(dodecahedron_v), dodecahedron_v, GL_STATIC_DRAW);

    GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

	mvpID = glGetUniformLocation(programID, "MVP");

	glm::mat4 projMat = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);

	glm::mat4 viewMat = glm::lookAt(
		glm::vec3(4, 3, -3), // Camera is at (4,3,-3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	glm::mat4 modelMat = glm::mat4(1.0f);

    MVP = projMat * viewMat * modelMat;
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

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

void drawScene(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glColor4f(1.0, 0.0, 0.0, 1.0);

	glUseProgram(programID);

	glUniformMatrix4fv(mvpID, 1, GL_FALSE, &MVP[0][0]);

    GLint posAttrib = glGetAttribLocation(programID, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, DODECAHEDRON_NUM_VERT*3);

    glDisableVertexAttribArray(posAttrib);

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
	float ratio = (float)w / (float)h;

    glViewport(0, 0, w, h);
	
}

int main(int argc, char** argv) {

	glutInit(&argc, argv);

	glutInitContextVersion(4, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	//glutInitContextFlags(GLUT_DEBUG);

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

	fprintf(stdout, "%s\n", glGetString(GL_VERSION));

	init();
	glutMainLoop();

	return 0;
}