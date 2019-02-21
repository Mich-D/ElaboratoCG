//Parametrizzazione unifome

#include "librerie.h"
typedef struct {
	float x;
	float y;
} Point2D;

extern vector<Point2D> Punti;
extern int m;

void parametrizzazione_uniforme(float *t)
{
	int i;
	float step = 1.0 / (float)(Punti.size() - 1);

	for (i = 0; i < Punti.size(); i++)
		t[i] = i*step;

}

void parametrizzazione_uniforme_bspline(float *t)
{
    int i, n = Punti.size() - m;
    float step = 1.0 / (float)(n - 1);

    float count = step;
    for (i = 0; i < n; i++) {
        t[i] = step;
        step += step;
    }

}

//Parametrizzazione secondo la lunghezza della corda

void parametrizzazione_corde(float *t)
{
	int i;
	t[0] = 0;

	for (i = 1; i < Punti.size(); i++)
		t[i] = t[i - 1] + sqrt((Punti.at(i).x - Punti.at(i - 1).x)*(Punti.at(i).x - Punti.at(i - 1).x) + (Punti.at(i).y - Punti.at(i - 1).y)*(Punti.at(i).y - Punti.at(i - 1).y));

	for (i = 1; i < Punti.size(); i++)
		t[i] = t[i] / t[Punti.size() - 1];


}
