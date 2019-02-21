
#include "librerie.h";
typedef struct {
	float x;
	float y;
} Point2D;

extern vector<Point2D> Punti;
extern vector<Point2D> DerivateMod;
extern vector<float> pesi;

extern int pval;
extern int visualizzaTg;
extern float Tens, Cont, Bias;


//Per Curve di hermite
#define PHI0(t)  (2.0*t*t*t-3.0*t*t+1)
#define PHI1(t)  (t*t*t-2.0*t*t+t)
#define PSI0(t)  (-2.0*t*t*t+3.0*t*t)
#define PSI1(t)  (t*t*t-t*t)

float dx(int i, float *t, float Tens, float Bias, float Cont)
{
	if (i == 0)
		return  0.5*(1 - Tens)*(1 - Bias)*(1 - Cont)*(Punti.at(i + 1).x - Punti.at(i).x) / (t[i + 1] - t[i]);
	if (i == Punti.size() - 1)
		return  0.5*(1 - Tens)*(1 - Bias)*(1 - Cont)*(Punti.at(i).x - Punti.at(i - 1).x) / (t[i] - t[i - 1]);

	if (i % 2 == 0)
		return  0.5*(1 - Tens)*(1 + Bias)*(1 + Cont)*(Punti.at(i).x - Punti.at(i - 1).x) / (t[i] - t[i - 1]) + 0.5*(1 - Tens)*(1 - Bias)*(1 - Cont)*(Punti.at(i + 1).x - Punti.at(i).x) / (t[i + 1] - t[i]);
	else
		return  0.5*(1 - Tens)*(1 + Bias)*(1 - Cont)*(Punti.at(i).x - Punti.at(i - 1).x) / (t[i] - t[i - 1]) + 0.5*(1 - Tens)*(1 - Bias)*(1 + Cont)*(Punti.at(i + 1).x - Punti.at(i).x) / (t[i + 1] - t[i]);
}
float dy(int i, float *t, float Tens, float Bias, float Cont)
{
	if (i == 0)
		return 0.5*(1 - Tens)*(1 - Bias)*(1 - Cont)*(Punti.at(i + 1).y - Punti.at(i).y) / (t[i + 1] - t[i]);
	if (i == Punti.size() - 1)
		return  0.5*(1 - Tens)*(1 - Bias)*(1 - Cont)*(Punti.at(i).y - Punti.at(i - 1).y) / (t[i] - t[i - 1]);

	if (i % 2 == 0)
		return  0.5*(1 - Tens)*(1 + Bias)*(1 + Cont)*(Punti.at(i).y - Punti.at(i - 1).y) / (t[i] - t[i - 1]) + 0.5*(1 - Tens)*(1 - Bias)*(1 - Cont)*(Punti.at(i + 1).y - Punti.at(i).y) / (t[i + 1] - t[i]);
	else
		return  0.5*(1 - Tens)*(1 + Bias)*(1 - Cont)*(Punti.at(i).y - Punti.at(i - 1).y) / (t[i] - t[i - 1]) + 0.5*(1 - Tens)*(1 - Bias)*(1 + Cont)*(Punti.at(i + 1).y - Punti.at(i).y) / (t[i + 1] - t[i]);
}

float DX(int i, float *t)
{
	if (DerivateMod.at(i).x == 0)
	{

		return dx(i, t, 0.0, 0.0, 0.0);
	}
	if (DerivateMod.at(i).x != 0)
	{
		return DerivateMod.at(i).x;
	}
}

float DY(int i, float *t)
{
	if (DerivateMod.at(i).y == 0)
	{

		return dy(i, t, 0.0, 0.0, 0.0);
	}
	if (DerivateMod.at(i).y != 0)
		return DerivateMod.at(i).y;

}

void InterpolazioneHermite(float* t, float *px, float *py)
{
	float passotg = 1.0 / (float)(pval - 1);
	float tg = 0, tgmapp, ampiezza;
	int i = 0;
	int is = 0; //indice dell'estremo sinistro dell'intervallo [t(i),t(i+1)] a cui il punto tg
				//appartiene


	for (tg = 0; tg <= 1; tg += passotg)
	{
		if (tg > t[is + 1]) is++;

		ampiezza = (t[is + 1] - t[is]);
		tgmapp = (tg - t[is]) / ampiezza;

		px[i] = Punti.at(is).x*PHI0(tgmapp) + DX(is, t)*PHI1(tgmapp)*ampiezza + Punti.at(is + 1).x*PSI0(tgmapp) + DX(is + 1, t)*PSI1(tgmapp)*ampiezza;
		py[i] = Punti.at(is).y*PHI0(tgmapp) + DY(is, t)*PHI1(tgmapp)*ampiezza + Punti.at(is + 1).y*PSI0(tgmapp) + DY(is + 1, t)*PSI1(tgmapp)*ampiezza;
		i++;

	}

	tg = 1;

	ampiezza = (t[is + 1] - t[is]);
	tgmapp = (tg - t[is]) / ampiezza;

	px[i] = Punti.at(is).x*PHI0(tgmapp) + DX(is, t)*PHI1(tgmapp)*ampiezza + Punti.at(is + 1).x*PSI0(tgmapp) + DX(is + 1, t)*PSI1(tgmapp)*ampiezza;
	py[i] = Punti.at(is).y*PHI0(tgmapp) + DY(is, t)*PHI1(tgmapp)*ampiezza + Punti.at(is + 1).y*PSI0(tgmapp) + DY(is + 1, t)*PSI1(tgmapp)*ampiezza;

    if (visualizzaTg == 1)
    {
        Point2D p1, p2;
        glColor3f(0.0, 1.0, 0.0);

        glBegin(GL_LINES);
        for (i = 0; i < Punti.size(); i++)
        {
            /*
            float tdx = dx(i, t, 0.0, 0.0, 0.0);
            float tdy = dy(i, t, 0.0, 0.0, 0.0);

            p1.x = Punti[i].x + tdx + (1 / tdx);
            p1.y = Punti[i].y + tdy + (1 / tdy);

            p2.x = Punti[i].x - tdx + (1 / tdx);
            p2.y = Punti[i].y - tdy + (1 / tdy);
            */
            
            p1.x = Punti[i].x + dx(i, t, 0.0, 0.0, 0.0) / 15;
            p1.y = Punti[i].y + dy(i, t, 0.0, 0.0, 0.0) / 15;

            p2.x = Punti[i].x - dx(i, t, 0.0, 0.0, 0.0) / 15;
            p2.y = Punti[i].y - dy(i, t, 0.0, 0.0, 0.0) / 15;
            
            glVertex3f(p1.x, p1.y, 0.0);
            glVertex3f(p2.x, p2.y, 0.0);

        }
        glEnd();



    }



}

