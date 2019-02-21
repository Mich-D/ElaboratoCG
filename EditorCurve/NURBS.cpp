#include "librerie.h";
#include "Spline.h"

typedef struct {
    float x;
    float y;
} Point2D;

extern vector<Point2D> Punti;
extern vector<float> pesi;

extern int pval;
extern int m;

void costruisci_BsplineRazionale(int l, float* Nodi, float ti, float *v, int dim) {
    int k, i, j;
    float d1, d2, beta;
    for (i = 0; i < dim; i++)
        v[i] = 0;

    v[l] = 1;

    for (i = 0; i < m - 1; i++) {

        float tmp = 0.0;

        for (j = l - i; j <= l; j++) {
            d1 = ti - Nodi[j];
            d2 = Nodi[i + j + 1] - ti;
            beta = v[j] / (d1 + d2);
            v[j - 1] = d2 * beta + tmp;
            tmp = d1 * beta;
        }
        v[l] = tmp;
    }
}

void DisegnaBaseSplineRazionale(float *Nodi, int dim, float **B) {

    float ti = 0;
    float dt = 1.0 / (float)(pval - 1);
    int j, i;
    //in ogni colonna la j-esima funzione base e in ogni riga la valutazione nei punti
    float *v = new float[dim];
    float *somme = new float[pval];
    float *pesiloc = new float[dim];
    //float *temp = new float[dim];

    //Copio i pesi in pesiloc perchè mi serve gestirli diversamentenel caso di nodi multipli
    for (int i = 0; i < Punti.size(); i++)
    {
        pesiloc[i] = pesi.at(i);
        //temp[i] = pesi.at(i);
    }

    // ----------------------------------------------
          //Per la gestione dei nodi multipli
    //--------------------------------------------//

    // E' necessario aggiungere il peso ai punti che vengono replicati (quando viene aumentata
    //la molteplicità del nodo corrispondente)
    /*
    if (flagModMolte == 1)
    {
        if (PuntoSelezionato > 3 && PuntoSelezionato < (int)Punti.size() - 3) {

            for (int i = PuntoSelezionato; i <= PuntoSelezionato + molte - 1; i++)
                temp[i] = pesiloc[PuntoSelezionato];

            int cont1 = 1;
            for (int i = PuntoSelezionato + molte; i < dim; i++)
            {
                pesiloc[i] = temp[PuntoSelezionato + cont1];
                cont1++;
            }

        }
    }
    */
    //-------------------------------------------------------------------------------------

        //valutazione di ciascuna funzione base per ogni valore del parametro t mediante le formule di Cooks
    for (int k = 0; k < pval; k++, ti += dt) {

        somme[k] = 0;
        int l = localizza_intervallo_internodale(ti, Nodi);

        costruisci_BsplineRazionale(l, Nodi, ti, v, dim);


        for (j = 0; j < dim; j++)
            B[k][j] = v[j];

        for (int i = 0; i < dim; i++) {
            somme[k] += (pesiloc[i] * B[k][i]);


        }
    }
    for (j = 0; j < dim; j++) {
        for (int k = 0; k < pval; k++)
            B[k][j] = pesiloc[j] * B[k][j] / somme[k];
    }
}