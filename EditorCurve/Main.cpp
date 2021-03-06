// EditorCurve17Maggio.cpp : definisce il punto di ingresso dell'applicazione console.
//

#include "librerie.h"
#include "Bezier.h"
#include "hermite.h"
#include "Spline.h"
#include "NURBS.h"
#include "parametrizzazione.h"
#include "GestioneStringhe.h"
#include "GestioneMenu.h"

int M_I = 0;
int id1, id2;
int pval = 500;

int flagModMolte = 0;
int m = 3;
int estnodi = 1;
int flagC = 0;

void drawScene();
void resize(GLsizei, GLsizei);
void mouse(int, int, int, int);

//Per il picking
void gl_select(int, int, int*);
void processHits(GLint, GLuint[], int*);
#define SIZE_BUFF 512
int molte = 1;

int metodo = 0, visualizzaTg = 0, dim;
int mod_par_der = 0;
float Tens, Cont, Bias;
float*t;
// Per i menu

int submenu_Opzioni_I, menu_id, submenu_hermite, submenu_bezier;
string mod = "Modalita' di inserimento ";
int scelta_param;
int submenu_param;
int submenu_spline;
int submenu_nurbs;
string tipo_param;

typedef struct {
	float x;
	float y;
} Point2D;


Point2D newPoint, zero;

vector <Point2D> Punti;
vector <Point2D> DerivateMod;
vector<float> pesi;

vector <int>FlagP;
GLsizei wh = 800, ww = 800;
int numPoint = 0;

int selected = -1;
int flagG = 0;


void drawScene() {

	string  NumeroPunti, s, coordXins, coordYins, stringaAscIns, stringaOrdIns;
	string coordinate, xpS, ypS, ascissa, ordinata;
    int i;
    float** B = new float*[pval];

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glPointSize(6.0);
	for (int i = 0; i<Punti.size(); i++) {
		if (FlagP[i] == 0) {
			glColor3f(1.0, 0.0, 0.0);
			glBegin(GL_POINTS);
			glVertex2f(Punti[i].x, Punti[i].y);
			glEnd();
		}
		else
		{
			glColor3f(0.0, 1.0, 0.0);
			glBegin(GL_POINTS);
			glVertex2f(Punti[i].x, Punti[i].y);
			glEnd();
		}

	}
	if (selected > -1)
	{

		glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_POINTS);
		glVertex2f(Punti[selected].x, Punti[selected].y);
		glEnd();
		glColor3f(1.0, 0.0, 0.0);
		FlagP[selected] = 1;

	}

	//abilito la modalità di disegno primitive in maniera tratteggiata

	glEnable(GL_LINE_STIPPLE);
	glLineStipple(4, 0x3F07);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i<Punti.size(); i++)
		glVertex2f(Punti[i].x, Punti[i].y);
	glEnd();
	//abilito la modalità di sisegno primitive in maniera tratteggiata
	glDisable(GL_LINE_STIPPLE);

    t = new float[Punti.size()];
    if (scelta_param == 0)
        parametrizzazione_uniforme(t);
    else
        parametrizzazione_corde(t);
    /*
    else {
        if (Punti.size() >= m) {
            t = new float[Punti.size() - m];
            parametrizzazione_uniforme_bspline(t);
        }
    }
    */

	if (Punti.size() >= m)
	{

		if (metodo == 0)
		{

            for (i = 0; i < pval; i++) {
                B[i] = new float[Punti.size()+1];
            }

            /*
			float *cx = new float[pval];
			float *cy = new float[pval];
			CostruisciBezier(cx, cy);
            */

            DisegnaBasebezier(B);

			glBegin(GL_LINE_STRIP);
            for (int i = 0; i < pval - 1; i++) {
                float x = 0.0, y = 0.0;
                for (int j = 0; j < Punti.size(); j++) {
                    x = x + B[i][j] * Punti.at(j).x;
                    y = y + B[i][j] * Punti.at(j).y;
                }
                glVertex3f(x, y, 0);
            }
			glEnd();

		}

		if (metodo == 1)
		{
			float *px = new float[pval];
			float *py = new float[pval];

			InterpolazioneHermite(t, px, py);
			glColor3f(0.0, 0.0, 1.0);
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i < pval; i++)
				glVertex3f(px[i], py[i], 0);
			glEnd();

		}

        if (metodo == 2)
        {
            /*
            float** B = new float*[Punti.size()];

            for (i = 0; i < pval; i++) {
                B[i] = new float[Punti.size()];
            }
            */

            //parametrizzazione_uniforme(t);

            for (i = 0; i < Punti.size(); i++) {
                printf("t[%d] = %f\n", i, t[i]);
            }
            printf("\n");

            int dimPartNodEstesa = Punti.size() + m;
            float *Nodi = new float[dimPartNodEstesa];

            Partizione_nodale_estesa(t, Nodi);
            for (i = 0; i < dimPartNodEstesa; i++) {
                printf("Nodi[%d] = %f\n", i, Nodi[i]);
            }
            printf("\n");

            float *sx = new float[pval];
            float *sy = new float[pval];

            ValutaSpline(Nodi, sx, sy);
            glBegin(GL_LINE_STRIP);
            for (int i = 0; i < pval; i++)
                glVertex3f(sx[i], sy[i], 0);
            glEnd();

        }

        if (metodo == 3) {

            for (i = 0; i < pval; i++) {
                B[i] = new float[Punti.size()];
            }

            int dimPartNodEstesa = Punti.size() + m;
            float *Nodi = new float[dimPartNodEstesa];
            Partizione_nodale_estesa(t, Nodi);

            for (i = 0; i < Punti.size(); i++) {
                printf("pesi[%d] = %f\n", i, pesi[i]);
            }

            DisegnaBaseSplineRazionale(Nodi, Punti.size(), B);

            glBegin(GL_LINE_STRIP);
            for (int i = 0; i < pval - 1; i++) {
                float x = 0.0, y = 0.0;
                for (int j = 0; j < Punti.size(); j++) {
                    x = x + B[i][j] * Punti.at(j).x;
                    y = y + B[i][j] * Punti.at(j).y;
                }
                glVertex3f(x, y, 0);
            }
            glEnd();
        }


	}

	

	glFlush();



	
	//Sulla finestra delle informazioni

	glutSetWindow(id2);



	glClear(GL_COLOR_BUFFER_BIT);

    if (Punti.size() >= m) {

        if (metodo != 1 && metodo != 2) {
            glPushMatrix();
            glTranslatef(50, 450, 0);
            glScalef(500, 300, 1);
            for (int k = 0; k < Punti.size(); k++)
            {
                float ti = 0;
                float dt = 1.0 / (float)(pval - 1);
                glBegin(GL_LINE_STRIP);
                for (int i = 0; i < pval; i++, ti += dt)
                    glVertex3f(ti, B[i][k], 0.0);
                glEnd();

            }
            glPopMatrix();
        }
    }

	if (M_I == 0)
	{
		if (Punti.size() >= 0)
		{
			stringaAscIns = "Ascissa punto inserito ";
			stringaOrdIns = "Ordinata punto inserito ";
			coordXins = floatToString(newPoint.x);
			coordYins = floatToString(newPoint.y);
			stringaAscIns.append(coordXins.substr(0, 7));
			stringaOrdIns.append(coordYins.substr(0, 7));
			bitmap_output(10, 50, 0, stringaAscIns, GLUT_BITMAP_HELVETICA_18);
			bitmap_output(10, 70, 0, stringaOrdIns, GLUT_BITMAP_HELVETICA_18);
		}
	}

	//Scrivo il numero di punti sulla finestra
	NumeroPunti = intToString(Punti.size());
	s = "Numero di punti : ";
	s.append(NumeroPunti);
	glColor3f(1.0, 0.0, 0.0);
	bitmap_output(10, 30, 0, s, GLUT_BITMAP_HELVETICA_18);

	//Scrivo la modalità della finestra
	glColor3f(1.0, 1.0, 0.0);
	bitmap_output(200, 30, 0, mod, GLUT_BITMAP_HELVETICA_18);

	

	glFlush();

}



void mouse(int button, int state, int x, int y) {


	newPoint.x = float(x);
	newPoint.y = wh - float(y);
	zero.x = 0.0;
	zero.y = 0.0;
	if (state == GLUT_DOWN)
	{
		switch (button)
		{


		case GLUT_LEFT_BUTTON:
			//se viene premuto il pulsante sinistro del mouse e Modalità inserimento punti (MI=0)
			if (M_I == 0)
			{
				Punti.push_back(newPoint);
				DerivateMod.push_back(zero);
				pesi.push_back(1.0f);
				FlagP.push_back(0);
			}

			//se viene premuto il pulsante sinsitro del mouse  in  Modalità Modifica punti (MI=1)

			if (M_I == 1 )
			{

                if (selected != -1) {
                    FlagP[selected] = 0;
                }

				//... individuo l'oggetto selezionato dall'utente 
				//(select conterrà l'id dell'punto relativo alla lista) ...
				gl_select(x, y, &selected);
				//... se l'utente ha selezionato un punto ...
				if (selected >= 0) {
					printf("Punto Selezionato: %d\n", selected);
				}

				Tens = 0.0;
				Cont = 0.0;
				Bias = 0.0;
			}

			//se viene premuto il pulsante sinsitro del mouse  in  Modalità Elimina punti (Mi=2)
			if (M_I == 2)
			{
                if (Punti.size() > 0) {
                    Punti.pop_back();
                    DerivateMod.pop_back();
                    pesi.pop_back();
                    FlagP.pop_back();
                }
			}

			break;
		}


	}
	glutPostRedisplay();
}

void keyDown(unsigned char key, int x, int y)
{
    switch (key) {
    case 27:
        exit(0);
        break;
    case 't':
        if (metodo == 1) {

        }
    case '+':
        if (selected != -1) {
            pesi[selected] += 0.1f;
        }
        break;
    case '-':
        if (selected != -1 && pesi[selected] >= 0.1f) {
            pesi[selected] -= 0.1f;
        }
        break;
    default:
        break;
    }

    glutPostRedisplay();
}

/* funzioni per la fase di picking */
void gl_select(int x, int y, int *selected) {

	GLuint selectBuf[SIZE_BUFF];
	GLint hits;
	GLint viewport[4];

	glGetIntegerv(GL_VIEWPORT, viewport);
	//inizializzo il buffer per gli oggetti selezionati
	glSelectBuffer(SIZE_BUFF, selectBuf);
	//abilito il rendering GL_SELECT
	glRenderMode(GL_SELECT);
	//inizializzo lo stack degli id
	glInitNames();
	//aggiungo almeno id = 0
	glPushName(0);
	//lavoro con le marici di proiezione e salvo i dati nello stack
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	//seleziono una regione attorno alla posizione (x,y) 
	//del puntatore 5x5
	gluPickMatrix((GLdouble)x, (GLdouble)(viewport[3] - y), 5.0, 5.0, viewport);
	//reimposto le coordinate della finestra sul mondo
	gluOrtho2D(0.0, (GLdouble)ww, 0.0, (GLdouble)wh);
	//ridisegno gli oggetti assegnando un id ad ognuno
	for (int i = 0; i<Punti.size(); i++) {
		glPushName(i);
		glBegin(GL_POINTS);
		glVertex2f(Punti[i].x, Punti[i].y);
		glEnd();
	}
	//lavoro con le matrici di proiezione e riprendo
	//tutti i dati salvati nello stack
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glFlush();
	//ritorno al rendering normale
	hits = glRenderMode(GL_RENDER);
	//processo gli oggetti selezionati
	processHits(hits, selectBuf, selected);

}

void processHits(GLint hits, GLuint buffer[], int *selected) {
	unsigned int ii, jj;
	GLuint names, *ptr;

	ptr = (GLuint *)buffer;

	for (ii = 0; ii < hits; ii++) { /*  for each hit  */
		names = *ptr;
		ptr += 3;
		for (jj = 0; jj < names; jj++) { /*  for each name */
			*selected = *ptr;
			ptr++;
		}
	}
}
// fine picking

void resize(GLsizei w, GLsizei h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	ww = w;
	wh = h;
}
void mouseMotion(int x, int y)
{
	Point2D newPoint;
	newPoint.x = x;
	newPoint.y = wh - y;

	if (selected >= 0)
	{
		if (M_I == 1)
		{
			Punti.at(selected) = newPoint;


		}
	}
	glutPostRedisplay();
}


void  main(int argc, char **argv)
{

	GLboolean GlewInitResult;

	glutInit(&argc, argv);

	glutInitContextVersion(4, 2);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(900, 1);
	id1 = glutCreateWindow("Editor Curve");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
    glutKeyboardFunc(keyDown);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	
	createMenu();

	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 1);
	id2 = glutCreateWindow("Informazioni");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	GlewInitResult = glewInit();
	if (GLEW_OK != GlewInitResult) {
		fprintf(
			stderr,
			"ERROR: %s\n",
			glewGetErrorString(GlewInitResult)
		);
		exit(EXIT_FAILURE);
	}

	fprintf(
		stdout,
		"INFO: OpenGL Version: %s\n",
		glGetString(GL_VERSION)
	);



	glutMainLoop();
}

