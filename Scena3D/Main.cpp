#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <map>

#include <GL\glew.h>
#include <GL\freeglut.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#define PI 3.1415926535

void processKeys();
void deallocate();

using namespace std;

const int WINDOW_WIDTH = 960;
const int WINDOW_HEIGHT = 540;
const int FRAMETIME = 1000.0 / 75.0;

float FOV = 45.0f;

map <unsigned char, bool> keyMap = {
    { 27, false },
    { 'w', false },
    { 'a', false },
    { 's', false },
    { 'd', false },
    { 'c', false },
    { '+', false },
    { '-', false },
    { 'f', false }
};

GLuint LoadShaders(const char*, const char*);

// Attribute location IDs
GLint viewMtxID, projMtxID;

glm::mat4 viewMtx, projMtx;

vector<glm::vec3> vertices;
vector<glm::vec2> uvs;
vector<glm::vec3> normals;

typedef struct {
    GLuint VAO, VBO, EBO;
    GLuint shaderID;
    GLint modelMtxID, posAttrib, normalAttrib, objColorID, uvAttrib;
    glm::mat4 modelMtx;
} Planes;

unsigned int texture;

typedef struct {
    GLuint VAO;
    GLuint shaderID;
    GLint modelMtxID, posAttrib, normalAttrib, objColorID;
    glm::mat4 modelMtx;
} Objects;

typedef struct {
    GLuint VAO;
    GLuint shaderID;
    GLint modelMtxID, posAttrib;

    float r, g, b;
} Light;

Planes planes;
Objects objects;
Light light;

glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, -40.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = 90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;

float lightAngle = 90.0f;

glm::vec3 lightPos;

/*
GLfloat plane_vn[] = {
    -5.0f, 0.0f, -5.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
     5.0f, 0.0f, -5.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
     5.0f, 0.0f,  5.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
     5.0f, 0.0f,  5.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -5.0f, 0.0f,  5.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -5.0f, 0.0f, -5.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
};
*/

GLfloat plane_vn[] = {
     5.0f, 0.0f, -5.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
     5.0f, 0.0f,  5.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -5.0f, 0.0f,  5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    -5.0f, 0.0f, -5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
};
unsigned int plane_indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

glm::vec3 planePositions[] = {
    glm::vec3(-10.0f, 0.0f, -10.0f),
    glm::vec3( 10.0f, 0.0f, -10.0f),
    glm::vec3( 10.0f, 0.0f,  10.0f),
    glm::vec3(-10.0f, 0.0f,  10.0f)
};

glm::vec3 objPositions[] = {
    glm::vec3(-10.0f, 5.0f, -10.0f),
    glm::vec3( 10.0f, 5.0f, -10.0f),
    glm::vec3( 10.0f, 5.0f,  10.0f),
    glm::vec3(-10.0f, 5.0f,  10.0f)
};

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
{

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

void init()
{

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glEnable(GL_CULL_FACE);

    projMtx = glm::perspective(glm::radians(FOV), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);

    viewMtx = glm::lookAt(
        cameraPos, cameraPos + cameraFront, cameraUp
    );

    light.r = 1.0f;
    light.g = 1.0f;
    light.b = 0.85f;

    /* PIANI */

    glGenVertexArrays(1, &planes.VAO);
    glBindVertexArray(planes.VAO);

    glGenBuffers(1, &planes.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, planes.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vn), plane_vn, GL_STATIC_DRAW);

    glGenBuffers(1, &planes.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planes.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(plane_indices), plane_indices, GL_STATIC_DRAW);

    planes.shaderID = LoadShaders("texture.vert", "texture.frag");
    glUseProgram(planes.shaderID);

    planes.posAttrib = glGetAttribLocation(planes.shaderID, "aPos");
    glVertexAttribPointer(planes.posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(planes.posAttrib);

	planes.normalAttrib = glGetAttribLocation(planes.shaderID, "aNormal");
    glVertexAttribPointer(planes.normalAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(planes.normalAttrib);

    planes.uvAttrib = glGetAttribLocation(planes.shaderID, "aTexCoord");
    glVertexAttribPointer(planes.uvAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(planes.uvAttrib);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *data = stbi_load("tile.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
	projMtxID = glGetUniformLocation(planes.shaderID, "projMtx");
	glUniformMatrix4fv(projMtxID, 1, GL_FALSE, &projMtx[0][0]);

	viewMtxID = glGetUniformLocation(planes.shaderID, "viewMtx");
	glUniformMatrix4fv(viewMtxID, 1, GL_FALSE, &viewMtx[0][0]);

    planes.modelMtx = glm::mat4(1.0f);
	planes.modelMtxID = glGetUniformLocation(planes.shaderID, "modelMtx");

	glUniform3f(glGetUniformLocation(planes.shaderID, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);

    glUniform3f(glGetUniformLocation(planes.shaderID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	glUniform3f(glGetUniformLocation(planes.shaderID, "lightColor"), light.r, light.g, light.b);

    planes.objColorID = glGetUniformLocation(planes.shaderID, "objectColor");
	glUniform3f(planes.objColorID, 1.0f, 1.0f, 1.0f);

    /* SOLIDI (6-8-12-20 FACCE) */

    glGenVertexArrays(1, &objects.VAO);
    glBindVertexArray(objects.VAO);

    objects.shaderID = LoadShaders("shader.vert", "shader.frag");
    glUseProgram(objects.shaderID);

    projMtxID = glGetUniformLocation(objects.shaderID, "projMtx");
    glUniformMatrix4fv(projMtxID, 1, GL_FALSE, &projMtx[0][0]);

    viewMtxID = glGetUniformLocation(objects.shaderID, "viewMtx");
    glUniformMatrix4fv(viewMtxID, 1, GL_FALSE, &viewMtx[0][0]);

    objects.modelMtx = glm::mat4(1.0f);
    objects.modelMtxID = glGetUniformLocation(objects.shaderID, "modelMtx");

    glUniform3f(glGetUniformLocation(objects.shaderID, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);

    glUniform3f(glGetUniformLocation(objects.shaderID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

    glUniform3f(glGetUniformLocation(objects.shaderID, "lightColor"), light.r, light.g, light.b);

    objects.objColorID = glGetUniformLocation(objects.shaderID, "objectColor");
    glUniform3f(objects.objColorID, 0.5f, 0.15f, 0.1f);

    /* LUCE */

    glGenVertexArrays(1, &light.VAO);
    glBindVertexArray(light.VAO);

    light.shaderID = LoadShaders("light.vert", "light.frag");
    glUseProgram(light.shaderID);

    projMtxID = glGetUniformLocation(light.shaderID, "projMtx");
    glUniformMatrix4fv(projMtxID, 1, GL_FALSE, &projMtx[0][0]);

    viewMtxID = glGetUniformLocation(light.shaderID, "viewMtx");
    glUniformMatrix4fv(viewMtxID, 1, GL_FALSE, &viewMtx[0][0]);

    light.modelMtxID = glGetUniformLocation(light.shaderID, "modelMtx");

}

void drawScene(void)
{
    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    processKeys();

    projMtx = glm::perspective(glm::radians(FOV), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

    viewMtx = glm::lookAt(
        cameraPos, cameraPos + cameraFront, cameraUp
    );

    // PIANI
    glBindTexture(GL_TEXTURE_2D, texture);

	glUseProgram(planes.shaderID);

    glUniformMatrix4fv(glGetUniformLocation(planes.shaderID, "projMtx"), 1, GL_FALSE, &projMtx[0][0]);

    glUniform3f(glGetUniformLocation(planes.shaderID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
    glUniformMatrix4fv(glGetUniformLocation(planes.shaderID, "viewMtx"), 1, GL_FALSE, &viewMtx[0][0]);

	glBindVertexArray(planes.VAO);
    for (unsigned int i = 0; i < 4; i++) {
        glm::mat4 mtx = glm::mat4(1.0f);
        mtx = glm::translate(planes.modelMtx, planePositions[i]);
        glUniformMatrix4fv(planes.modelMtxID, 1, GL_FALSE, &mtx[0][0]);

        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    // OGGETTI
    glUseProgram(objects.shaderID);

    glUniformMatrix4fv(glGetUniformLocation(objects.shaderID, "projMtx"), 1, GL_FALSE, &projMtx[0][0]);

    glUniform3f(glGetUniformLocation(objects.shaderID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
    glUniformMatrix4fv(glGetUniformLocation(objects.shaderID, "viewMtx"), 1, GL_FALSE, &viewMtx[0][0]);

    glBindVertexArray(objects.VAO);
    glutSetVertexAttribCoord3(glGetAttribLocation(objects.shaderID, "aPos"));
    glutSetVertexAttribNormal(glGetAttribLocation(objects.shaderID, "aNormal"));

    objects.modelMtx = glm::rotate(
        objects.modelMtx,
        glm::radians(1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    for (unsigned int i = 0; i < 4; i++) {
        glm::mat4 mtx = glm::translate(glm::mat4(1.0f), objPositions[i]);
        glm::mat4 finalmtx = mtx * objects.modelMtx * glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
        glUniformMatrix4fv(objects.modelMtxID, 1, GL_FALSE, &finalmtx[0][0]);

        switch (i) {
        case 0: glutSolidCube(1.0); break;
        case 1: glutSolidOctahedron(); break;
        case 2: glutSolidDodecahedron(); break;
        case 3: glutSolidIcosahedron(); break;
        default: break;
        }
    }

    // LUCE
    glUseProgram(light.shaderID);

    glUniformMatrix4fv(glGetUniformLocation(light.shaderID, "projMtx"), 1, GL_FALSE, &projMtx[0][0]);

    glUniformMatrix4fv(glGetUniformLocation(light.shaderID, "viewMtx"), 1, GL_FALSE, &viewMtx[0][0]);

    glUniform3f(glGetUniformLocation(light.shaderID, "lightColor"), light.r, light.g, light.b);

    glBindVertexArray(light.VAO);
    glutSetVertexAttribCoord3(glGetAttribLocation(light.shaderID, "aPos"));

    glm::vec3 tmp;
    tmp.x = cos(glm::radians(lightAngle));
    tmp.y = sin(glm::radians(lightAngle));
    tmp.z = 0.0f;
    lightPos = glm::normalize(tmp) * 20.0f;

    lightAngle -= 1.0f;

    glm::mat4 mtx = glm::translate(glm::mat4(1.0f), lightPos);
    glUniformMatrix4fv(light.modelMtxID, 1, GL_FALSE, &mtx[0][0]);

    glutSolidSphere(1.0f, 8, 8);

	glutSwapBuffers();
}

void processKeys() {
    glm::vec3 front;

    for (const auto& kv : keyMap) {

        if (kv.second) {

            switch (kv.first) {
            case 27:
                deallocate();
                exit(0);
                break;
            case 'w':
                cameraPos += 0.5f * cameraFront;
                break;
            case 's':
                cameraPos -= 0.5f * cameraFront;
                break;
            case 'a':
                yaw -= 2.0f;

                front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                front.y = sin(glm::radians(pitch));
                front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                cameraFront = glm::normalize(front);
                break;
            case 'd':
                yaw += 2.0f;

                front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                front.y = sin(glm::radians(pitch));
                front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                cameraFront = glm::normalize(front);
                break;
            case 'c':
                cameraPos = cameraPos = glm::vec3(0.0f, 5.0f, -40.0f);

                yaw = 90.0f;

                front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                front.y = sin(glm::radians(pitch));
                front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                cameraFront = glm::normalize(front);
                break;
            case '+':
                FOV += 1.0f;
                if (FOV > 120.0f) {
                    FOV = 120.f;
                }
                break;
            case '-':
                FOV -= 1.0f;
                if (FOV < 5.0f) {
                    FOV = 5.f;
                }
                break;
            case 'f':
                FOV = 45.0f;
                break;
            default:
                break;
            }
        }
    }
}

void deallocate()
{
    glDeleteVertexArrays(1, &planes.VAO);
    glDeleteBuffers(1, &planes.VBO);

    glDeleteVertexArrays(1, &objects.VAO);

    glDeleteVertexArrays(1, &light.VAO);
}

void timerFunc(int t)
{
	glutPostRedisplay();

	glutTimerFunc(FRAMETIME, timerFunc, 0);
}

void keyDown(unsigned char key, int x, int y)
{
    auto it = keyMap.find(key);
    if (it != keyMap.end()) {
        keyMap.at(key) = true;
    }

	glutPostRedisplay();
}

void keyUp(unsigned char key, int x, int y) {
    auto it = keyMap.find(key);
    if (it != keyMap.end()) {
        keyMap.at(key) = false;
    }

    glutPostRedisplay();
}

void resize(int w, int h)
{
	double ratio = (double)w / (double)h;
	glViewport(0, 0, w, h);
}

int main(int argc, char** argv)
{
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
	glutKeyboardUpFunc(keyUp);
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