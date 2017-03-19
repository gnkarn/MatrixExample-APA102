// coordinates.h

#ifndef _COORDINATES_h
#define _COORDINATES_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class Coordinates {
public:
	// start value, start speed, update confidence, evolution confidence
	Coordinates(uint8_t x = 0, uint8_t y = 0); // constructor
	void fromCartesian(uint8_t x, uint8_t y);
	void fromPolar(uint8_t r, uint8_t phi, uint8_t xc, uint8_t yc);
	uint8_t getR() { return r; };
	uint8_t getAngle() { return phi; };
	uint8_t getX() { return x; };
	uint8_t getY() { return y; };
	uint8_t getxc() { return xc; };
	uint8_t getyc() { return yc; };
private:
	uint8_t r;
	float phi;
	float x, y;
	uint8_t signx;
	uint8_t signy;
	uint8_t xc, yc;
	float auxx, auxy;
};



#endif

