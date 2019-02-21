#pragma once

int localizza_intervallo_internodale(float t, float* Nodi);
void Partizione_nodale_estesa(float *t, float *Nodi);
void costruisci_BsplineRazionale(int l, float* Nodi, float ti, float* v, int dim);
void DisegnaBaseSplineRazionale(float *Nodi, int dim, float **B);