#include "Coordinates.h"
#include <FastLED.h>


Coordinates::Coordinates(uint8_t x, uint8_t y) {
	fromCartesian(x, y);
}

void Coordinates::fromCartesian(uint8_t x, uint8_t y) {
	this->x = x;
	this->y = y;
	if (x == 0 && y == 0) {
		this->r = 0;
		this->phi = 0;
		return;
	}
	this->r = sqrt(x*x + y*y);
	if (x == 0 && 0 < y) {
		this->phi = PI / 2;
	}
	else if (x == 0 && y < 0) {
		this->phi = PI * 3 / 2;
	}
	else if (x < 0) { //x != 0
		this->phi = atan(y / x) + PI;
	}
	else if (y < 0) {
		this->phi = atan(y / x) + 2 * PI;
	}
	else {
		this->phi = atan(y / x);
	}

}

void Coordinates::fromPolar(uint8_t r, uint8_t phi, uint8_t xc, uint8_t yc) {
	// 

	this->r = r;//r;
	this->phi = phi*M_PI/128;  //  divido por 256 y multiplico por 2Pi
	auxx = (float)this->r*cos(this->phi);
	auxy = (float)this->r*sin(this->phi);// multiplico por 2 pero debe ser el factor escala visual del tablero= sepaaracion x / separacionY
	this->y = (yc + auxy); 
	this->x = (xc + auxx);
	/*
	if ((this->phi >= 0 && this->phi <= 128)) //1st and 2nd quadrant
	{
		signy = 1; // y positive
	}
	else
	{
		signy = 2;// y negative
	}

	if ((this->phi >= 0 && this->phi <= 64) || (this->phi >= 192 && this->phi <= 255)) // 1st and 4th quadrant
	{
		signx = 1; // positive x
	}
	else
	{
		signx = 2; // negative X
	}

	switch (signx)
	{
	case 1:
		this->x = xc + auxx ;// this->x = xc + auxx ;
		break;
	case 2:
		this->x = xc - auxx ;//this->x = xc - auxx ;
		break;
	} ;
	switch (signy)
	{
	case 1:
		this->y = yc + auxy;//this->y = yc + auxy;
		break;
	case 2:
		this->y = yc - auxy;//this->y = yc - auxy;
		break;

	};
	*/
}

