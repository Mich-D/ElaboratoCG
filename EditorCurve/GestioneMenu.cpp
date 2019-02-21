
#include "librerie.h";
extern int flagG;
extern string mod;
extern int selected;
extern int M_I;
extern int metodo;
extern int mod_par_der;
extern int visualizzaTg;
extern int submenu_param;
extern int submenu_hermite;
extern int submenu_bezier;
extern int submenu_spline;
extern int submenu_nurbs;
extern int menu_id;
extern int scelta_param;
extern int estnodi;
extern string tipo_param;

void menu(int num) {
	if (num == 0)
		if (flagG == 1)
			flagG = 0;
		else
			flagG = 1;


	glutPostRedisplay();
}

//Menu per la scelta della modalità del tasto sinistro
void menu_M_I(int num) {
	M_I = num;
	if (M_I == 0)
		mod = "Modalita' Inserisci ";
	if (M_I == 1)
	{
		mod = "Modalita' Sposta ";
		//
		selected = -1;
	}
	if (M_I == 2)
		mod = "Modalita' Elimina ";
	glutPostRedisplay();
}

void menu_hermite(int num)
{
	metodo = 1;

	if (num == 1) {
		if (mod_par_der == 1)
			mod_par_der = 0;
		else
		{
			mod_par_der = 1;
			mod = "Modalita' Modifica tangenti ";
			M_I = -1;
		}
	}

	if (num == 2) {
		if (visualizzaTg == 1)
			visualizzaTg = 0;
		else
			visualizzaTg = 1;
	}
	glutPostRedisplay();

}

void menu_spline(int num)
{
	if (num==0)
	metodo = 2;

	if (num == 1)
		estnodi = 0;

	if (num == 2)
		estnodi = 1;


	glutPostRedisplay();


}

void menu_nurbs(int num) {
    if (num == 0)
        metodo = 3;


    glutPostRedisplay();
}

void menu_Param(int num) {
	scelta_param = num;
	if (scelta_param == 0)
		tipo_param = "Parametrizzazione Uniforme ";
	if (scelta_param == 1)
	{
		tipo_param = "Parametrizzazione della lunghezza delle corde ";
	}
	glutPostRedisplay();
}


void menu_bezier(int num)
{
	metodo = 0;
	glutPostRedisplay();
}
void createMenu(void) {
	// Creazione sottomenu da attaccare all'item del menu relativo alla scelta
	// del tipo di modalità del tasto sinisto


	int submenu_Opzioni_I = glutCreateMenu(menu_M_I);
	glutAddMenuEntry("Inserisci", 0);
	glutAddMenuEntry("Sposta", 1);
	glutAddMenuEntry("Elimina", 2);



	submenu_param = glutCreateMenu(menu_Param);
	glutAddMenuEntry("Uniforme", 0);
	glutAddMenuEntry("Lunghezza delle corde", 1);

	submenu_hermite = glutCreateMenu(menu_hermite);
	glutAddMenuEntry("Calcola Curva Interpolante", 0);
	glutAddMenuEntry("Modalita' modifica tangenti", 1);
	glutAddMenuEntry("Visualizza tangenti ", 2);

	submenu_bezier = glutCreateMenu(menu_bezier);
	glutAddMenuEntry("Calcola Curva Bezier", 0);

	submenu_spline = glutCreateMenu(menu_spline);
	glutAddMenuEntry("Calcola Curva Spline", 0);
	glutAddMenuEntry("Nodi agli estremi non coincidenti", 1);
	glutAddMenuEntry("Nodi agli estremi  coincidenti", 2);

    submenu_nurbs = glutCreateMenu(menu_nurbs);
    glutAddMenuEntry("Calcola Curva NURBS", 0);

	menu_id = glutCreateMenu(menu);
	glutAddMenuEntry("Disegna Griglia", 0);

	glutAddSubMenu("Parametrizzazione", submenu_param);
	glutAddSubMenu("Modalita inserimento", submenu_Opzioni_I);

	glutAddSubMenu("Curve Interpolanti di Hermite di classe C1", submenu_hermite);
	glutAddSubMenu("Curve di Bezier approssimanti di forma", submenu_bezier);
	glutAddSubMenu("Curve Spline cubiche approssimanti di Forma", submenu_spline);
    glutAddSubMenu("Curve NURBS cubiche approssimanti di Forma", submenu_nurbs);


	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
