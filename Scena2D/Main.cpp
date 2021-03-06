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
#define FRAMETIME 30.0

#define SHADER_HEADER "#version 330 core\n"
#define SHADER_STR(x) #x
void mygl_GradientBackground(float top_r, float top_g, float top_b, float top_a,
    float bot_r, float bot_g, float bot_b, float bot_a)
{
    glDisable(GL_DEPTH_TEST);

    static GLuint background_vao = 0;
    static GLuint background_shader = 0;

    if (background_vao == 0)
    {
        glGenVertexArrays(1, &background_vao);

        const char* vs_src = (const char*)SHADER_HEADER SHADER_STR
        (
            out vec2 v_uv;
        void main()
        {
            uint idx = uint(gl_VertexID);
            gl_Position = vec4(idx & 1U, idx >> 1U, 0.0, 0.5) * 4.0 - 1.0;
            v_uv = vec2(gl_Position.xy * 0.5 + 0.5);
        }
        );

        const char* fs_src = (const char*)SHADER_HEADER SHADER_STR
        (
            uniform vec4 top_color;
        uniform vec4 bot_color;
        in vec2 v_uv;
        out vec4 frag_color;

        void main()
        {
            frag_color = bot_color * (1 - v_uv.y) + top_color * v_uv.y;
        }
        );
        GLuint vs_id, fs_id;
        vs_id = glCreateShader(GL_VERTEX_SHADER);
        fs_id = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(vs_id, 1, &vs_src, NULL);
        glShaderSource(fs_id, 1, &fs_src, NULL);
        glCompileShader(vs_id);
        glCompileShader(fs_id);
        background_shader = glCreateProgram();
        glAttachShader(background_shader, vs_id);
        glAttachShader(background_shader, fs_id);
        glLinkProgram(background_shader);
        glDetachShader(background_shader, fs_id);
        glDetachShader(background_shader, vs_id);
        glDeleteShader(fs_id);
        glDeleteShader(vs_id);
        glUseProgram(background_shader);
    }

    glUseProgram(background_shader);
    GLuint top_color_loc = glGetUniformLocation(background_shader, "top_color");
    GLuint bot_color_loc = glGetUniformLocation(background_shader, "bot_color");
    glUniform4f(top_color_loc, top_r, top_g, top_b, top_a);
    glUniform4f(bot_color_loc, bot_r, bot_g, bot_b, bot_a);

    glBindVertexArray(background_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
}

using namespace std;

typedef struct { float x, y; } Point;

float t = 0.0;
float bgR, bgG, bgB;
float xboat = 0.0, yboat = -44.0, boatAcceleration = 0.0, boatAngle = 0.0;
int dirboat = 0;
bool wireframe, daylight;

Point C1[30], C2[30];

void init()
{
	bgR = 0.2;
	bgG = 0.6;
	bgB = 1.0;

	wireframe = false;
	daylight = true;
}

void Sole(void)
{
    int i;
    float angolo;
    int nvertices_sole = 20;
    glBegin(GL_POLYGON);
    for (i = 0; i < nvertices_sole; i++)
    {
        angolo = (2 * PI)*i / nvertices_sole;
        C1[i].x = 3 * cos(angolo);
        C1[i].y = 3 * sin(angolo);
        glVertex2f(C1[i].x, C1[i].y);
    }
    glEnd();


    for (i = 0; i < nvertices_sole; i++)
    {
        angolo = (2 * PI)*i / nvertices_sole;
        C2[i].x = 5 * cos(angolo);
        C2[i].y = 5 * sin(angolo);
    }

    glBegin(GL_LINES);
    for (i = 0; i < nvertices_sole; i++)
    {
        glVertex2f(C1[i].x, C1[i].y);
        glVertex2f(C2[i].x, C2[i].y);
    }
    glEnd();


}

void luna()

{

    int i;
    float angolo;
    int nvertices = 30;
    glBegin(GL_POLYGON);
    for (i = 0; i < nvertices; i++)
    {
        angolo = (2 * PI)*i / nvertices;
        C1[i].x = 3 * sin(angolo);
        C1[i].y = (0.5 - cos(2 * angolo) - cos(angolo));
        glVertex3f(C1[i].x, C1[i].y, 0.0);
    }
    glEnd();
}

void drawBoat() {
	glColor3f(1.0, 0.95, 0.9);

	glBegin(GL_POLYGON);
	glVertex2f(-20.0, -5.0);
	glVertex2f(0.0, 22.5);
	glVertex2f(0.0, -5.0);
	glEnd();

	glLineWidth(4.0);
	glColor3f(0.55, 0.33, 0.2);

	glBegin(GL_LINES);
	glVertex2f(0.0, -7.5);
	glVertex2f(0.0, 22.5);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(-15.0, -22.5);
	glVertex2f(-20.0, -7.5);
	glVertex2f(20.0, -7.5);
	glVertex2f(15.0, -22.5);
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
    //mygl_GradientBackground(0.2, 0.6, 1.0, 1.0, 0.4, 0.8, 1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (!wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

    glColor3f(1.0, 1.0, 0.6);
    //Scalatura, Orientamento e Posizionamento della Luna
    glPushMatrix();
    if (daylight) {
        glTranslatef(-100.0, 80.0, 0.0);
        glRotatef(45.0f, 0.0, 0.0, 1.0);
        glScalef(10.0, 10.0, 1.0);
        Sole();
    } else {
        glTranslatef(100.0, 80.0, 0.0);
        glRotatef(45.0f, 0.0, 0.0, 1.0);
        glScalef(10.0, 10.0, 1.0);
        luna();
    }
    glPopMatrix();

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
		}
		else {
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
	//glutIdleFunc(idle);
	//glutMouseFunc(mouse);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyDown);
	//glutKeyboardFunc(keyUp);
	glutTimerFunc(FRAMETIME, timer, -1);

	glutCreateMenu(menu);
	glutAddMenuEntry("Giorno/Notte", 1);
	glutAddMenuEntry("Wireframe/Colori", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glewExperimental = GL_TRUE;
	glewInit();

	init();
	glutMainLoop();

	return 0;
}