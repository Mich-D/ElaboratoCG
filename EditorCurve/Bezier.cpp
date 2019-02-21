
#include "librerie.h"
typedef struct {
	float x;
	float y;
} Point2D;


extern vector<Point2D> Punti;
extern vector<Point2D> DerivateMod;
extern vector<float> pesi;

extern int pval;


void CostruisciBezier(float *cx, float *cy)
{
	float tstep = 1.0 / (float)(pval - 1);  //passo tra i valori del parametro in cui valutare la curva
	float tg; //valore del parametro in cui valutare la curva
	int i, j, k;
	k = 0;

	Point2D *c = new Point2D[Punti.size()];

	for (tg = 0; tg <= 1; tg += tstep)
	{

		for (i = 0; i < Punti.size(); i++)
		{
			c[i].x = Punti[i].x;
			c[i].y = Punti[i].y;
		}

		for (j = 1; j<Punti.size(); j++)
		{
			for (i = 0; i<Punti.size() - j; i++)
			{
				c[i].x = (1 - tg)*c[i].x + tg*c[i + 1].x;
				c[i].y = (1 - tg)*c[i].y + tg*c[i + 1].y;
			}

		}

		cx[k] = c[0].x;
		cy[k] = c[0].y;
		k++;
	}




}

void ValutaBernst(float ti, float*b)
{
	int i;
	for (i = 0; i <= Punti.size(); i++)
		b[i] = 0;

	b[Punti.size()] = 1.0; //Condizione iniziale

	for (int i = 1; i<Punti.size(); i++)
	{
		float d1b = 0;
		for (int j = 0; j<Punti.size(); j++)
		{
			b[j] = (1 - ti)*b[j + 1] + d1b;
			d1b = ti*b[j + 1];
		}
		b[Punti.size()] = d1b;
	}

}
void DisegnaBasebezier(float **B)
{
    float tg = 0;
    float *b = new float[Punti.size()+1];
    float *somme = new float[pval];
    float tstep = 1.0 / (float)(pval - 1);
    int k, i;

    for (i = 0; i <= Punti.size(); i++)
        for (k = 0; k < pval; k++)
            B[k][i] = 0;


    for (k = 0; k < pval; k++, tg += tstep)
    {
        somme[k] = 0;
        ValutaBernst(tg, b);

        for (i = 0; i <= Punti.size(); i++)
            B[k][i] = b[i];
        for (int i = 0; i < Punti.size(); i++)
            somme[k] += (pesi[i] * B[k][i + 1]);



    }
    for (int j = 0; j < Punti.size(); j++) {
        for (int k = 0; k < pval; k++)
            B[k][j] = pesi[j] * B[k][j + 1] / somme[k];
    }
}