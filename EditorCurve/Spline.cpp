
#include "librerie.h";

typedef struct {
    float x;
    float y;
} Point2D;

extern vector<Point2D> Punti;

extern int pval;
extern int m;
extern int estnodi;

int localizza_intervallo_internodale(float t, float* Nodi)
{
    int a = m-1;
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

/*
int localizza_intervallo_internodale(float t, float* Nodi) {

}
*/

void Partizione_nodale_estesa(float *t, float *Nodi)
{
    int k;  //k numero dei nodi veri;
    int i, n = Punti.size() - m;

    //Nodi fittizi a sinistra

    if (estnodi == 1) {

        for (i = 0; i < m; i++)
            Nodi[i] = 0;    //Fisso i nodi fittizi a  sinistra coincidenti con l'estremo sinistro dell'intervallo di parametrizzazione

                            //Nodi veri
        int cont = 2;
        for (i = m; i < (n + m); i++) {
            Nodi[i] = t[cont];
            cont++;
        }
        //Nodi fittizi a destra

        for (i = (n + m); i < (n + m + m); i++)
            Nodi[i] = 1;  //Fisso i nodi fittizi a destra coincidenti con l'estremo destro dell'intervallo del parametro, cioè 1;

    } else {
        float dtg = 1.0 / (float)(Punti.size() + m - 1);
        float tg = 0;

        for (i = 0; i < (Punti.size() + m); i++, tg += dtg) {
            Nodi[i] = tg;
        }
    }
}

/*
float DeBoor(int l, float punto, float* Nodi, float* c) {

    int r, j;
    float d[30];

    for (j = 0; j <= m; j++) {
        d[j] = c[j + l - m];
    }

    for (r = 1; r <= m; r++) {
        for (j = m; j > (r - 1); j--) {
            float alpha = (punto - Nodi[j + l - m]) / (Nodi[j + 1 + l - r] - Nodi[j + l - m]);
            d[j] = ((1.0 - alpha) * d[j - 1]) + (alpha * d[j]);
        }
    }

    return d[m];
}
*/

void costruisci_Bspline(int l, float* Nodi, float punto, float *v)
{

    //Input: l=estremo sinistro dell'intervallo a cui punto appartiente
    //       Nodi   Vettore dei nodi
    //		 punto:  punto in cui valutare le b-spline diverse da zero
    //                 con indice l-m+1,...l 
    //       v:   vettore in cui sono memorizzati i valori delle b-spline diverse da zero.

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

void ValutaSpline(float* Nodi, float *sx, float *sy)
{
    float dtg = 1.0 / (float)(pval - 1);
    float tg = 0;
    int k, i;
    float *v = new float[Punti.size()];

    float* x = new float[Punti.size()];
    float* y = new float[Punti.size()];

    for (i = 0; i < Punti.size(); i++) {
        x[i] = Punti.at(i).x;
        y[i] = Punti.at(i).y;
    }

    for (k = 0; k < pval; k++, tg += dtg)
    {
        sx[k] = 0;
        sy[k] = 0;

        int l = localizza_intervallo_internodale(tg, Nodi);

        costruisci_Bspline(l, Nodi, tg, v);
        //for (i = 0; i < Punti.size(); i++)
        //    printf("v[%d]=%f\n", i, v[i]);
        //printf("sx[%d]=%f\n", k, sx[k]);
        for (i = l - m + 1; i <= l; i++)
        {
            sx[k] = sx[k] + v[i] * x[i];
            sy[k] = sy[k] + v[i] * y[i];

        }
        //	printf("sx[%d]=%f\n", k, sx[k]);
        //printf("sy[%d]=%f\n", k, sy[k]);
    }

}

/*
void ValutaSpline(float* Nodi, float* sx, float* sy) {

    float dtg = 1.0 / (float)(pval - 1);
    float tg = 0;
    int i, k;

    float* nx = new float[Punti.size()];
    float* ny = new float[Punti.size()];

    for (i = 0; i < Punti.size(); i++) {
        nx[i] = Punti.at(i).x;
        ny[i] = Punti.at(i).y;
    }

    for (k = 0; k < pval; k++, tg += dtg)
    {
        int l = localizza_intervallo_internodale(tg, Nodi);

        printf("sx[%d]\n", k);
        sx[k] = DeBoor(l, tg, Nodi, nx);
        printf("sy[%d]\n", k);
        sy[k] = DeBoor(l, tg, Nodi, ny);
        printf("\n");
    }

}
*/