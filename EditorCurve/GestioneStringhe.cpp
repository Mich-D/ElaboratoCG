
#include "librerie.h";

//funzione per convertire un  int in stringa
string intToString(int value) {
	stringstream ss;
	ss << value;
	return ss.str();
}

//funzione per convertire un float in stringa
string floatToString(float value) {
	stringstream ss;
	ss << value;
	return ss.str();
}

void bitmap_output(float x, float y, float z, string s, void *font) {
	glPushMatrix();
	glRasterPos3f(x, y, 0);
	for (unsigned int i = 0; i < s.length(); i++) {
		glutBitmapCharacter(font, s[i]);
	}
	glPopMatrix();
}

