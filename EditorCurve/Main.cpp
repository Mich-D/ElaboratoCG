#include <cstdio>
#include <cstdlib>
#include <vector>
#include <sstream>

#include <GL\glew.h>
#include <GL\freeglut.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

using namespace std;

typedef struct {
	float x;
	float y;
} POINT2D;

typedef struct {
    float* t;
    float* x;
    float* y;
    float* px;
    float* py;
} Bezier;

typedef struct {
	float* t;
	float* Nodi;
	float* x;
	float* y;
	float* alpha;
	float* beta;
	float* sx;
	float* sy;
} BSpline;

typedef struct {
    float* t;
    float* Nodi;
    float* x;
    float* y;
    float* alpha;
    float* beta;
    float* sx;
    float* sy;
} BSplineM;

vector<POINT2D> Punti;
Bezier bezier;
BSpline bSpline;
BSplineM bSplineM;

int selected_point = -1;
int M_P = 0;
int M_C = 0;
int m = 3;
int pval = 300;

/******************************************************
cp è un array di 4 elementi dove:
cp[0] è il punto iniziale
cp[1] è il primo punto di controllo
cp[2] è il secondo punto di controllo
cp[3] è il punto finale

t è il valore del parametro, 0 <= t <= 1
*******************************************************/

POINT2D PointOnCubicBezier(POINT2D* cp, float t)
{
    float   ax, bx, cx;
    float   ay, by, cy;
    float   tSquared, tCubed;
    POINT2D result;

    /* calcolo dei coefficienti del polinomio */

    cx = 3.0 * (cp[1].x - cp[0].x);
    bx = 3.0 * (cp[2].x - cp[1].x) - cx;
    ax = cp[3].x - cp[0].x - cx - bx;

    cy = 3.0 * (cp[1].y - cp[0].y);
    by = 3.0 * (cp[2].y - cp[1].y) - cy;
    ay = cp[3].y - cp[0].y - cy - by;

    /* calcolo del punto della curva in relazione a t */

    tSquared = t * t;
    tCubed = tSquared * t;

    result.x = (ax * tCubed) + (bx * tSquared) + (cx * t) + cp[0].x;
    result.y = (ay * tCubed) + (by * tSquared) + (cy * t) + cp[0].y;

    return result;
}

/*****************************************************************************
ComputeBezier riempe un array di strutture Point2D  con i punti della curva
generati dai punti di controllo cp. Il chiamante deve allocare memoria
sufficiente per il risultato che è <sizeof(Point2D) * numeroDiPunti>
******************************************************************************/

void ComputeBezier(POINT2D* cp, int numberOfPoints, POINT2D* curve)
{
    float dt;
    int   i;

    dt = 1.0 / (numberOfPoints - 1);

    for (i = 0; i < numberOfPoints; i++)
        curve[i] = PointOnCubicBezier(cp, i*dt);
}

// BEZIER

// A utility function to return minimum of two integers
int min(int a, int b)
{
    return (a<b) ? a : b;
}

// Returns value of Binomial Coefficient C(n, k)
int binomialCoeff(int n, int k)
{
    int** C;
    int i, j;

    C = new int*[n + 1];
    for (i = 0; i < n + 1; i++) {
        C[i] = new int[k + 1];
    }

    // Caculate value of Binomial Coefficient in bottom up manner
    for (i = 0; i <= n; i++)
    {
        for (j = 0; j <= min(i, k); j++)
        {
            // Base Cases
            if (j == 0 || j == i)
                C[i][j] = 1;

            // Calculate value using previosly stored values
            else
                C[i][j] = C[i - 1][j - 1] + C[i - 1][j];
        }
    }

    return C[n][k];
}

void valuta_bezier(float* t, float* x, float* y, float* px, float* py)
{
    float dtg = 1.0 / (float)(pval - 1);
    float tg = 0;
    int k, i, n = Punti.size();
    float *B = new float[Punti.size()];

    for (k = 0; k < pval; k++, tg += dtg)
    {
        px[k] = 0;
        py[k] = 0;

        for (i = 0; i < n; i++) {
            B[i] = (float)binomialCoeff((n-1), i) * pow((1 - tg), n - i - 1) * pow(tg, i);
        }

        for (i = 0; i < n; i++)
        {
            px[k] = px[k] + x[i] * B[i];
            py[k] = py[k] + y[i] * B[i];

        }
    }
}

// CURVA A NODI SEMPLICI

void parametrizzazione_uniforme(float *t)
{
	int i;
	float step = 1.0 / (float)(Punti.size() - 1);
	for (i = 0; i < Punti.size(); i++) {
		t[i] = i * step;
	}
}

void Partizione_nodale_estesa(float *t, float *Nodi)
{
	int k;  //k numero dei nodi veri;
	int i;
	k = Punti.size() - m;

	//Nodi fittizi a sinistra

	for (i = 0; i < m; i++)
		Nodi[i] = 0;    //Fisso i nodi fittizi a  sinistra coincidenti con l'estremo sinistro dell'intervallo di parametrizzazione

						//Nodi veri
	int cont = 2;
	for (i = m; i < m + k; i++) {
		Nodi[i] = t[cont];
		cont++;
	}
	//Nodi fittizi a destra

	for (i = m + k; i < 2 * m + k; i++)
		Nodi[i] = 1;  //Fisso i nodi fittizi a destra coincidenti con l'estremo destro dell'intervallo del parametro, cioè 1;
}

void Partizione_nodale_estesa_multipli(float *t, float *Nodi) {

}

int localizza_intervallo_internodale(float t, float* Nodi)
{
	int a = m - 1;
	int b = Punti.size();
	int centro;
	while (b - a > 1) {
		centro = (a + b) / 2;
		if (t < Nodi[centro]) {
			b = centro;
		}
		else
		{
			a = centro;
		}

	}
	return a;
}

void costruisci_Bspline(int l, float* Nodi, float punto, float *v)
{
	int k, i, j;
	float d1, d2, beta;
	for (i = 0; i < Punti.size(); i++)
		v[i] = 0;

	v[l] = 1;

	for (i = 0; i < m - 1; i++) {

		float tmp = 0.0;

		for (j = l - i; j <= l; j++) {
			d1 = punto - Nodi[j];
			d2 = Nodi[i + j + 1] - punto;
			beta = v[j] / (d1 + d2);
			v[j - 1] = d2 * beta + tmp;
			tmp = d1 * beta;
		}
		v[l] = tmp;
	}
}

void costruisciMatriceSistema(float *Nodi, float*t, float **A)
{
	int k, l, j;
	float *v = new float[Punti.size()];

	for (k = 0; k < Punti.size(); k++) {
		l = localizza_intervallo_internodale(t[k], Nodi);

		costruisci_Bspline(l, Nodi, t[k], v);

		for (j = 0; j < Punti.size(); j++)
			A[k][j] = v[j];
	}
}

void BackSostitution(float *b, float** R, float* x)
{
	double somma = 0;
	int i, j;
	int N = Punti.size() - 1;
	//Sostituzione all'indietro
	x[N] = b[N] / R[N][N];

	for (i = N - 1; i >= 0; i--)
	{
		somma = 0;
		for (j = i + 1; j <= N; j++)
		{
			somma += R[i][j] * x[j];
		}
		x[i] = (b[i] - somma) / R[i][i];
	}

}


void Risolvi(float **A, float *x, float *y, float* alfa, float* beta)
{
	//Input A: matrice delle funzioni base B-spline
	//  x,y:       termini noti
	// alfa beta:  vettori in cui saranno memorizzate le soluzioni dei due sistemi lineari.


	float** R = new float*[Punti.size()]; //le funzioni base sono quanti sono i punti

	for (unsigned int i = 0; i < Punti.size(); i++) {
		R[i] = new float[Punti.size()]();
	}

	float *bx = new float[Punti.size()];
	float *by = new float[Punti.size()];

	int l, k, i, j;
	double m, max, aus;

	for (i = 0; i <Punti.size(); i++)
	{
		for (j = 0; j < Punti.size(); j++)
		{
			R[i][j] = A[i][j];
		}
		//Copia dei vettori dei termini noti 
		bx[i] = x[i];
		by[i] = y[i];
	}

	//Algoritmo di Gauss
	for (k = 0; k<Punti.size() - 1; k++)	//passi dell'algoritmo
	{
		max = 0; //inizializzo il massimo a zero
		max = fabs(R[k][k]);
		l = k;
		for (i = k; i <Punti.size(); i++)
		{
			if (fabs(R[i][k])>max)
			{
				max = fabs(R[i][k]);
				l = i;
			}
		}

		if (l != k)
		{
			for (j = k; j <Punti.size(); j++)
			{
				aus = R[k][j];
				R[k][j] = R[l][j];
				R[l][j] = aus;
			}
			aus = bx[k];
			bx[k] = bx[l];
			bx[l] = aus;

			aus = by[k];
			by[k] = by[l];
			by[l] = aus;

		}

		for (i = k + 1; i < Punti.size(); i++)		//righe
		{
			m = R[i][k] / R[k][k];
			for (j = k + 1; j < Punti.size(); j++)		//colonne
			{
				R[i][j] = R[i][j] - m * R[k][j];
			}
			bx[i] = bx[i] - m * bx[k];
			by[i] = by[i] - m * by[k];
		}
	}

	//Azzera la parte inferiore alla diagonale
	for (i = 1; i <Punti.size(); i++)
		for (j = 0; j<i; j++)
			R[i][j] = 0;
	BackSostitution(bx, R, alfa);
	BackSostitution(by, R, beta);

}

void ValutaSpline(float* Nodi, float *alfa, float *beta, float *sx, float *sy)
{
	float dtg = 1.0 / (float)(pval - 1);
	float tg = 0;
	int k, i;
	float *v = new float[Punti.size()];

	for (k = 0; k < pval; k++, tg += dtg)
	{
		sx[k] = 0;
		sy[k] = 0;

		int l = localizza_intervallo_internodale(tg, Nodi);

		costruisci_Bspline(l, Nodi, tg, v);
		for (i = 0; i<Punti.size(); i++)
			printf("v[%d]=%f\n", i, v[i]);
		//printf("sx[%d]=%f\n", k, sx[k]);
		for (i = l - m + 1; i <= l; i++)
		{
			sx[k] = sx[k] + v[i] * alfa[i];
			sy[k] = sy[k] + v[i] * beta[i];

		}
		//	printf("sx[%d]=%f\n", k, sx[k]);
		//printf("sy[%d]=%f\n", k, sy[k]);
	}
}

void drawBezier()
{
    int i;

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glPointSize(5.0);
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POINTS);
    for (i = 0; i < Punti.size(); i++)
        glVertex2f(Punti.at(i).x, Punti.at(i).y);
    glEnd();

    glEnable(GL_LINE_STIPPLE);
    glLineStipple(4, 0x3F07);
    glBegin(GL_LINE_STRIP);
    for (i = 0; i < Punti.size(); i++)
        glVertex2f(Punti.at(i).x, Punti.at(i).y);
    glEnd();

    glDisable(GL_LINE_STIPPLE);

    if (Punti.size() > 1) {
        bezier.t = new float[Punti.size()];
        parametrizzazione_uniforme(bezier.t);

        bezier.x = new float[Punti.size()];
        bezier.y = new float[Punti.size()];
        bezier.px = new float[pval];
        bezier.py = new float[pval];

        for (i = 0; i < Punti.size(); i++) {
            bezier.x[i] = Punti.at(i).x;
            bezier.y[i] = Punti.at(i).y;
        }

        valuta_bezier(bezier.t, bezier.x, bezier.y, bezier.px, bezier.py);

        glColor4f(1.0, 0.0, 0.0, 1.0);
        glBegin(GL_LINE_STRIP);
        for (i = 0; i < pval; i++) {
            glVertex3f(bezier.px[i], bezier.py[i], 0);
        }
        glEnd();
    }
}

void drawBSpline()
{
    int i;

    float** A = new float*[Punti.size()];

    for (i = 0; i < Punti.size(); i++) {
        A[i] = new float[Punti.size()]();
    }

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glPointSize(5.0);
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POINTS);
    for (i = 0; i < Punti.size(); i++)
        glVertex2f(Punti.at(i).x, Punti.at(i).y);
    glEnd();

    glEnable(GL_LINE_STIPPLE);
    glLineStipple(4, 0x3F07);
    glBegin(GL_LINE_STRIP);
    for (i = 0; i < Punti.size(); i++)
        glVertex2f(Punti.at(i).x, Punti.at(i).y);
    glEnd();

    glDisable(GL_LINE_STIPPLE);

    if (Punti.size() >= m) {
        bSpline.t = new float[Punti.size()];
        parametrizzazione_uniforme(bSpline.t);

        int DimPartNodEstesa = Punti.size() + m;
        bSpline.Nodi = new float[DimPartNodEstesa];
        bSpline.x = new float[Punti.size()];
        bSpline.y = new float[Punti.size()];
        bSpline.alpha = new float[Punti.size()];
        bSpline.beta = new float[Punti.size()];
        bSpline.sx = new float[pval];
        bSpline.sy = new float[pval];

        Partizione_nodale_estesa(bSpline.t, bSpline.Nodi);

        costruisciMatriceSistema(bSpline.Nodi, bSpline.t, A);

        for (i = 0; i < Punti.size(); i++) {
            bSpline.x[i] = Punti.at(i).x;
            bSpline.y[i] = Punti.at(i).y;
        }
        Risolvi(A, bSpline.x, bSpline.y, bSpline.alpha, bSpline.beta);

        ValutaSpline(bSpline.Nodi, bSpline.alpha, bSpline.beta, bSpline.sx, bSpline.sy);

        glColor4f(1.0, 0.0, 0.0, 1.0);
        glBegin(GL_LINE_STRIP);
        for (i = 0; i < pval; i++) {
            glVertex3f(bSpline.sx[i], bSpline.sy[i], 0);
        }
        glEnd();
    }
}

void display(void)
{
    switch (M_C) {
    case 0:
        drawBezier();
        break;

    case 1:
        drawBSpline();
        break;

    default:
        break;
    }

	glFlush();
}

void mouseMotion(int x, int y)
{
	POINT2D newPoint;
	newPoint.x = x;
	newPoint.y = WINDOW_HEIGHT - y;

	if (selected_point != -1)
	{
		if (M_P == 1)
		{
			Punti.at(selected_point) = newPoint;
			//printf("Ho selezionato il Punto selezionato %d \n", PuntoSelezionato);
		}
	}
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {

	POINT2D zero, newPoint;
	float toll = 30, distanza, distanza1;

	zero.x = 0;
	zero.y = 0;

	newPoint.x = x;
	newPoint.y = WINDOW_HEIGHT - y;

	if (state != GLUT_DOWN) {
		return;
	}

	switch (button) {
		case GLUT_LEFT_BUTTON:
			if (M_P == 0) {
				Punti.push_back(newPoint);
			}
			else if (M_P == 1) {
				selected_point = 0;
				distanza = sqrt((Punti.at(0).x - newPoint.x)*(Punti.at(0).x - newPoint.x) + (Punti.at(0).y - newPoint.y)*(Punti.at(0).y - newPoint.y));

				for (int i = 1; i < Punti.size(); i++) {
					distanza1 = sqrt((Punti.at(i).x - newPoint.x)*(Punti.at(i).x - newPoint.x) + (Punti.at(i).y - newPoint.y)*(Punti.at(i).y - newPoint.y));
					if (distanza1<distanza) {
						selected_point = i;
						distanza = distanza1;
					}
				}

				if (distanza > toll) {
					selected_point = -1;
				}
			}
			else if (M_P == 2) {
				if (Punti.size() > 0) {
					Punti.pop_back();
				}
			}

			break;
		default:
			break;
	}

	glutPostRedisplay();
}

void menu_punti(int num) {
	M_P = num;

	glutPostRedisplay();
}

void menu_curve(int num) {
    M_C = num;

    glutPostRedisplay();
}

void menu(int num) {

}

void crea_menu() {
	int punti_id = glutCreateMenu(menu_punti);
	glutAddMenuEntry("Inserisci", 0);
	glutAddMenuEntry("Sposta", 1);
	glutAddMenuEntry("Elimina", 2);

	int curve_id = glutCreateMenu(menu_curve);
	glutAddMenuEntry("Bezièr", 0);
	glutAddMenuEntry("Spline a nodi semplici", 1);
	glutAddMenuEntry("Spline a nodi multipli", 2);
	glutAddMenuEntry("NURBS", 3);

	int menu_id = glutCreateMenu(menu);
	glutAddMenuEntry("Disegna Griglia", 0);
	glutAddSubMenu("Punti", punti_id);
	glutAddSubMenu("Curve", curve_id);

	glutAttachMenu(GLUT_RIGHT_BUTTON);

}

void resize(int w, int h)
{
	float ratio = w / h;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, WINDOW_WIDTH, 0.0, WINDOW_HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char** argv) {

	GLboolean GlewInitResult;

	glutInit(&argc, argv);

	glutInitContextVersion(4, 2);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(20, 20);

	glutCreateWindow("Editor Curve");

	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);

	crea_menu();

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

	return 0;
}