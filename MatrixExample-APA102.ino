
#include <FastLED.h>
#include <LEDMatrix.h>  
// ver https://github.com/AaronLiddiment/LEDMatrix/wiki

#include "coordinates.h"



// Change the next 6 defines to match your matrix type and size




// Defining LED strip

#define DATA_PIN D2                //Using WS2812B -- if you use APA102 or other 4-wire LEDs you need to also add a clock pin
#define CLOCK_PIN   D1
#define LED_TYPE    APA102
#define COLOR_ORDER BGR

#define CHIPSET        LED_TYPE

#define MATRIX_WIDTH   20  // Set this negative if physical led 0 is opposite to where you want logical 0
#define MATRIX_HEIGHT  24  // Set this negative if physical led 0 is opposite to where you want logical 0
#define MATRIX_TYPE    VERTICAL_ZIGZAG_MATRIX  // See top of LEDMatrix.h for matrix wiring types
#define MATRIX_SIZE		MATRIX_WIDTH*MATRIX_HEIGHT
cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds;
CRGB leds2[MATRIX_SIZE];

uint8_t hue, nivelEfecto;
int16_t counter;
uint8 efecto=6, numEffects=7;
uint8_t h;


// Param for different pixel layouts para usar la misma funcion XY
// const bool    kMatrixSerpentineLayout = true; eliminar 
// all 2D effects will be calculated in this matrix size
// do not touch
const uint8_t WIDTH = MATRIX_WIDTH;// 
const uint8_t HEIGHT = MATRIX_HEIGHT;

void drawLineByAngle(uint8_t x, uint8_t y, uint16_t angle, uint8_t length, CHSV  color)
{
	float p_angle = angle, p_length = length, p_start = 0, x1, y1;
	//#define PI 3.14159265

	/*leds.DrawLine(
		x,
		y,
		x + length*cos8(angle ),
		y + length*sin8_C(angle ), color);// version de 8 bits
	*/
	x1 = x + ((p_start + p_length)*cos(p_angle*PI/180)) ;
	y1 = y + ((p_start + p_length)*sin(p_angle*PI / 180)) ;

	leds.DrawLine(

		uint8_t(x + (p_start*( cos(p_angle*PI / 180)))),
		uint8_t(y + (p_start*( sin(p_angle*PI / 180)))),
		uint16_t(x1),
		uint16_t(y1), color
	);
}

void drawLineByAngle(uint8_t xc, uint8_t yc, uint8_t angle, uint8_t start, uint8_t length, CHSV color)
{
	//float p_angle = angle, p_length = length, p_start = start,
	
	Coordinates point = Coordinates();

	
	 point.fromPolar(start+length, angle, xc, yc); // x + ((p_start + p_length)*(1+cos(p_angle))/2);
	 //y1 = y + ((p_start + p_length)*(1+sin(p_angle))/2);
	 uint8_t x1 = point.getX(); // coordenadas punto final
	 uint8_t y1 = point.getY();
	 point.fromPolar(start, angle, xc, yc);
	 DrawWuLine(
		 *leds[0],
		 point.getX(),
		 point.getY(),
		x1,
		y1, color,255,8);
	//Serial.print("x1: "); Serial.print(x1); Serial.print("y1: "); Serial.println(y1);// debug
	
}

void drawEstrella(uint8_t x, uint8_t y, uint8_t arms, uint8_t start, uint8_t length, CHSV color) //center , #arms,start length, length, color
{
	uint8_t delta = 64/arms; // angulo incremental
	for (uint16_t angle = 0; angle <= 254; angle=angle+delta) // vuelta completa
	{
		drawLineByAngle(x, y, angle, start, length,color);
		//Serial.print("\tangle= ");Serial.println( angle);// debug
		drawLineByAngle(x, y, angle+128, start, length, color);
		;
	}
	
	blur2d(leds[0], MATRIX_WIDTH, MATRIX_HEIGHT, 16);
	RenderCustomMatrix();
}


/*
Arguments:
+  pDC is where line is drawn. Can be memory device context.
+  (X0,Y0) is start point of line.
+  (X1, Y1) is end point of line.
+  BaseColor is intensity of line. Pass 0 for black line.
+  NumLevels is number of gray scale levels. Pass 256.
+  IntensityBits denotes bits used to represent color component. Pass 8.

Note: NumLevels and IntensityBits have
been preserved from Michael Abrash's implementation.
They come very handy in customizing drawing
algorithm on different graphics hardware.
You may hardcode them.

Wu antialiased line drawer.
* (X0,Y0),(X1,Y1) = line to draw
* BaseColor = color # of first color in block used for antialiasing, the
*          100% intensity version of the drawing color
* NumLevels = size of color block, with BaseColor+NumLevels-1 being the
*          0% intensity version of the drawing color
* IntensityBits = log base 2 of NumLevels; the # of bits used to describe
*          the intensity of the drawing color. 2**IntensityBits==NumLevels
*http://www.drdobbs.com/database/graphics-programming/184408790?pgno=3#0169_000a
*Basecolor is now used as h for hsv 
*intensisty is set as S , and V = 255
*Adapted to work with 255 intensity levels, a grid up to 255 x 255 pixels
*for larger matrix the uint8 math have to be replaced with 16 bit integers
*/
void DrawWuLine(CRGB leds2 , uint8_t X0, uint8_t Y0, uint8_t X1, uint8_t Y1,
	CHSV BaseColor, uint8_t NumLevels, uint8_t  IntensityBits)
{
	uint8_t IntensityShift, ErrorAdj, ErrorAcc;
	uint8_t ErrorAccTemp, Weighting, WeightingComplementMask;
	uint8_t DeltaX, DeltaY, Temp, XDir1,XDir2;

	/* Make sure the line runs top to bottom */
	if (Y0 > Y1) {
		Temp = Y0; Y0 = Y1; Y1 = Temp;
		Temp = X0; X0 = X1; X1 = Temp;
	}
	/* Draw the initial pixel, which is always exactly intersected by
	the line and so needs no weighting */
	 DrawPixel(leds2, X0, Y0, BaseColor);
	 if (X1 >= X0) {
		 DeltaX = X1 - X0;
		 XDir1 = 1;XDir2 = 0;//pendiente positiva
			//Serial.print("inicial x1 "); Serial.println(X1);// debug
			//Serial.print("; inicial y1 "); Serial.println(Y1);// debug
		 
	 }
	
	else {
		XDir1 = 0; XDir2 = 1;	//pendiente negativa XDir = -1  because i dont use negative numbers;
		DeltaX = X0-X1; /* make DeltaX positive */
	}
	/* Special-case horizontal, vertical, and diagonal lines, which
	require no weighting because they go right through the center of
	every pixel */

	DeltaY = Y1 - Y0;
	

	if (DeltaY==0) {
		/* Horizontal line */
		

		while (DeltaX-- != 0) {
			X0 += XDir1-XDir2;
			DrawPixel(leds2, X0, Y0, BaseColor);
		}
		return;
	}
	if (DeltaX == 0) {
		/* Vertical line */
		do {
			Y0++;
			DrawPixel(leds2, X0, Y0, BaseColor);
		} while (--DeltaY != 0);
		return;
	}
	if (DeltaX == DeltaY) {
		/* Diagonal line */
		do {
			X0 += XDir1-XDir2;
			Y0++;
			DrawPixel(leds2, X0, Y0, BaseColor);
		} while (--DeltaY != 0);
		return;
	}

	/* Line is not horizontal, diagonal, or vertical */
	

	ErrorAcc = 0;  /* initialize the line error accumulator to 0 */
				   /* # of bits by which to shift ErrorAcc to get intensity level */
	IntensityShift = 16 - IntensityBits;
	/* Mask used to flip all bits in an intensity weighting, producing the
	result (1 - intensity weighting) */
	WeightingComplementMask = NumLevels - 1;
	/* Is this an X-major or Y-major line? */
	if (DeltaY > DeltaX) {
		/* Y-major line; calculate 8-bit fixed-point fractional part of a
		pixel that X advances each time Y advances 1 pixel, truncating the
		result so that we won't overrun the endpoint along the X axis */
		ErrorAdj = ((int )DeltaX << 8) / (uint8_t)DeltaY;
		/* Draw all pixels other than the first and last */
		while (--DeltaY) {
			ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
			ErrorAcc = ErrorAcc + ErrorAdj;/* calculate error for next pixel */
			
			if (ErrorAcc <= ErrorAccTemp) {
				/* The error accumulator turned over ( saturated to max 255), so advance the X coord */
				X0 += XDir1-XDir2;
			}
			Y0++; /* Y-major, so always advance Y */
				  /* The IntensityBits most significant bits of ErrorAcc give us the
				  intensity weighting for this pixel, and the complement of the
				  weighting for the paired pixel */
			Weighting = ErrorAcc ;
			DrawPixel(leds2, X0, Y0, CHSV(BaseColor.hue, 255,ErrorAcc));

			//Serial.print("pix1 x0 "); Serial.print(X0); Serial.print("; pix1 y0  "); Serial.println(Y0);		// debug


			DrawPixel(leds2, X0 + XDir1-XDir2, Y0,CHSV(BaseColor.hue ,255,255-ErrorAcc));

			//Serial.print("xdir1  "); Serial.print((XDir1)); Serial.print("; xdir2  "); Serial.println((XDir2));// debug
		}
		/* Draw the final pixel, which is
		always exactly intersected by the line
		and so needs no weighting */
		DrawPixel(leds2, X1, Y1, CHSV(BaseColor.hue,255,255));
		return;
	}
	/* It's an X-major line; calculate 16-bit fixed-point fractional part of a
	pixel that Y advances each time X advances 1 pixel, truncating the
	result to avoid overrunning the endpoint along the X axis */
	ErrorAdj = ((int)DeltaY << 8) / (uint8_t)DeltaX;

	//Serial.print("error  "); Serial.println(ErrorAdj);// debug

	/* Draw all pixels other than the first and last */
	while (--DeltaX) {
		ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
		//ErrorAcc=qadd8(ErrorAcc,ErrorAdj);      /* calculate error for next pixel */
		ErrorAcc = ErrorAcc + ErrorAdj;
		//if (ErrorAcc ==255) {
			if(ErrorAcc<= ErrorAccTemp){
			/* The error accumulator turned over, so advance the Y coord */
			Y0++;
		}
		X0 += XDir1-XDir2; /* X-major, so always advance X */
					/* The IntensityBits most significant bits of ErrorAcc give us the
					intensity weighting for this pixel, and the complement of the
					weighting for the paired pixel */
		Weighting = ErrorAcc >> IntensityShift;
		DrawPixel(leds2, X0, Y0, CHSV(BaseColor.hue,255,255-ErrorAcc) );
		DrawPixel(leds2, X0, Y0 + 1,CHSV(BaseColor.hue,255,ErrorAcc));

		//Serial.print("x0  "); Serial.print(X0); Serial.print(" y0  "); Serial.print(Y0); Serial.print(" err "); Serial.println(ErrorAcc);// debug
	}
	/* Draw the final pixel, which is always exactly intersected by the line
	and so needs no weighting */
	DrawPixel(leds2, X1, Y1, CHSV(BaseColor.hue,255,255));
}

void DrawPixel(CRGB leds2, uint8_t X0, uint8_t Y0, CHSV pixel)
{
	leds(X0, Y0) = pixel;
	
	
}

void DrawVentilador(uint8_t centrox, uint8_t centroy, uint8_t radio,int angulo, CHSV color) {
	
		color.h = random8(200);
		drawLineByAngle(centrox, centroy, angulo, 2,radio, color);
		 //blur2d((leds[0]), MATRIX_WIDTH, MATRIX_HEIGHT, 32);

		

		 
		
	
}

/*
-------------------------------------------------------------------
Basic Helper functions:

XY                  translate 2 dimensional coordinates into an index
Line                draw a line
Pixel               draw a pixel
ClearAll            empty the screenbuffer
MoveOscillators     increment osci[] and calculate p[]=sin8(osci)
InitMSGEQ7          activate the MSGEQ7
ReadAudio           get data from MSGEQ7 into left[7] and right[7]

-------------------------------------------------------------------
*/

// translates from x, y into an index into the LED array and
// finds the right index for a S shaped matrix
// para la matriz de height width
//NOTE: if i use cled class, this conversion is not needed

uint16_t XY(uint8_t x, uint8_t y) {
	if (y > HEIGHT) {
		y = HEIGHT;
	}
	if (y < 0) {
		y = 0;
	}
	if (x > WIDTH) {
		x = WIDTH;
	}
	if (x < 0) {
		x = 0;
	}
	// for a serpentine layout reverse every 2nd column:
	{
		uint16_t i;

		if (MATRIX_TYPE != VERTICAL_ZIGZAG_MATRIX) {
			i = (x * HEIGHT) + y;
		}

		if (MATRIX_TYPE == VERTICAL_ZIGZAG_MATRIX) {
			if (x & 0x01) {
				// Odd rows run backwards
				uint8_t reverseY = (HEIGHT - 1) - y;
				i = (x * HEIGHT) + reverseY;
			}
			else {
				// Even rows run forwards
				i = (x * HEIGHT) + y;
			}
		}

		return i;
	}
}

class Circulo {
	// Class Member Variables
	// These are initialized at startup
private:
	unsigned long mpreviousMillis, mupdatet; 	// will store last time LED was updated
	uint8_t mcrad, mcrpm, mcxc, mcyc;
	bool mcdir;
	uint8_t mcx, mcy;// pixel coordinates
	uint16_t  mcrev;
	CHSV mccolor;
	uint8_t mcphi; // angulo del pixel

public:
	// cxc,cyc,crpm,crev,crad,ccolor,cdir : center coords, rpm, num of revs, radious , color ,dir
	// Constructor - creates a Circulo  
	// and initializes the member variables and state
	
	Circulo(uint8_t cxc, uint8_t cyc, uint16_t crpm, uint8_t crev, uint8_t crad, CHSV ccolor, bool cdir)
	{
		mcxc = cxc;
		mcyc = cyc;
		mcrpm = crpm;
		mcrev = crev;
		mcrad = crad;
		mcphi = 0;
		mccolor = ccolor;
		mcdir = cdir;// 0 horario 1 anti
		mpreviousMillis = 0;  	// will store last time LED was updated
		mupdatet = 256 / (crpm); // time in msecs  that justifies and update of 1 degree , based on rpm and matrix resolution , other alternative =7500 / (crpm*crad)
	}

	uint8_t getrad() { return mcrad; };
	uint8_t getrpm() { return mcrpm; };
	uint8_t getxc() { return mcxc; };
	uint8_t getycrev() { return mcrev; };
	void setxc(uint8_t cxc) {  mcxc = cxc; };
	void setyc(uint8_t cyc) {  mcyc = cyc; };
	void  setColor(CHSV ccolor) { mccolor = ccolor; };
	


	//void Start() {};
	//void Stop() {};
	void ChgDir() {
		mcdir =!mcdir;
	}

	void Update()
	{
		// check to see if it's time to change the state of the LED
		unsigned long currentMillis = millis();

		if (currentMillis - mpreviousMillis >= mupdatet)
		{
			Coordinates cpoint = Coordinates();

			cpoint.fromPolar(mcrad, mcphi, mcxc, mcyc); // x + ((p_start + p_length)*(1+cos(p_angle))/2);
															//y1 = y + ((p_start + p_length)*(1+sin(p_angle))/2);
			uint8_t x1 = cpoint.getX(); // coordenadas punto 
			uint8_t y1 = cpoint.getY();
			leds(x1,y1) = mccolor;
			//Serial.println(  x1, y1);

		mpreviousMillis = currentMillis;  // Remember the time
		mcphi =mcphi +1*(mcdir)-1*(!mcdir); // incrementa en la direccion indicada
		FastLED.show();

		}
	}
	
};


void diagonalFill() {
	// ** Fill LED's with diagonal stripes
	for (uint8 x = 0; x < (leds.Width() + leds.Height()); ++x)
	{
		leds.DrawLine(x - leds.Height(), leds.Height() - 1, x, 0, CHSV(h, 255, 255));
		h += 16;
	}
}

void HorizontalStripes(){
	// ** Fill LED's with horizontal stripes
	for (uint8 y = 0; y<leds.Height(); ++y)
	{
		leds.DrawLine(0, y, leds.Width() - 1, y, CHSV(h, 255, 255));
		

		h += 16;
	}
	blur2d(leds[0], MATRIX_WIDTH, MATRIX_HEIGHT, 16);// blur the individual circles
}

void anillos() {
	// ** circulos concentricos
	for (uint8 y = 0; y<leds.Height(); ++y)
	{
		
		leds.DrawCircle(10, 12, y, CHSV(h, 255, 250));

		h += 16;
	}
	blur2d(leds[0], MATRIX_WIDTH, MATRIX_HEIGHT, 16);// blur the individual circles
}
// Circulo( cxc,  cyc,  crpm,  crev,  crad,  ccolor,  cdir)

Circulo cir1(10, 12, 255, 2, 6, CHSV(0, 255, 255), 0);

void circleBeat() {
	// Use two out-of-sync sine waves
	//incorporar centro y radio para ubicarlo en cualquier posicion, pasarlo a clase
	//uint8_t  r = beatsin8(5, 0, 19, 0, 0);
	uint8_t  i = beatsin8(32, 0, 19, 0, 0);
	uint8_t  j = beatsin8(32, 0, 23, 0, 192);// la misma frecuencia hace un circulo con y defasado 64 gira horario y defasado192 en anti
	leds(i, j) = CHSV((i+j)*4, 255, 255);
	
	blur2d(leds[0], MATRIX_WIDTH, MATRIX_HEIGHT, 16);
}


class CircleBeat { //cxc,cyc,bpm,crad,ccolor, status, cdir 
	// Class Member Variables
	// These are initialized at startup
private:
	unsigned long mpreviousMillis, mupdatet; 	// will store last time LED was updated
	uint8_t mcrad, mcbpm, mcxc, mcyc; // radio, BPM, center coords
	bool mcdir;
	uint8_t mcx, mcy;// pixel coordinates
	bool  mcstatus;// 0 detenido , 1 activo
	CHSV mccolor;
	

public:
	// cxc,cyc,crpm,crev,crad,ccolor,cdir : center coords, rpm, num of revs, radious , color ,dir
	// Constructor - creates a Circulo  
	// and initializes the member variables and state

	CircleBeat(uint8_t cxc, uint8_t cyc, uint16_t cbpm, uint8_t crad, CHSV ccolor, bool cbstatus, bool cdir)
	{
		mcxc = cxc;
		mcyc = cyc;
		mcbpm = cbpm;
		mcrad = crad;
		mccolor = ccolor;
		mcdir = cdir;// 0 horario 1 anti
		mpreviousMillis = 0;  	// will store last time LED was updated
		mcstatus = cbstatus = 0;
	
	}

	uint8_t getrad() { return mcrad; };
	uint8_t getbpm() { return mcbpm; };
	uint8_t getxc() { return mcxc; };
	bool  getstatus() { return mcstatus; };
	bool  getdir() { return mcdir; };
	void setxc(uint8_t cxc) { mcxc = cxc; };
	void setyc(uint8_t cyc) { mcyc = cyc; };
	void  setColor(CHSV ccolor) { mccolor = ccolor; };
	void setdir(bool cdir) { mcdir = cdir; };


	void Start() { mcstatus = 1; };
	void ChangeStatus() { mcstatus = !mcstatus; };
	void Stop() { mcstatus = 0; };
	void ChgDir() {mcdir = !mcdir;}

	void Update()
	{
		// check to see if it's time to change the state of the LED
		unsigned long currentMillis = millis();

		if (currentMillis - mpreviousMillis >= mupdatet)
		{
			// Use two out-of-sync sine waves
			//incorporar centro y radio para ubicarlo en cualquier posicion, pasarlo a clase
			//uint8_t  r = beatsin8(5, 0, 19, 0, 0);

			uint8_t  i = beatsin8(mcbpm, mcxc-mcrad, mcxc + mcrad, 0,0);
			uint8_t  j = beatsin8(mcbpm, mcyc - mcrad,mcyc + mcrad,0, 64*mcdir+192*!mcdir);// la misma frecuencia hace un circulo con y defasado 64 gira horario y defasado192 en anti
			leds(i, j) = mccolor;

			blur2d(leds[0], MATRIX_WIDTH, MATRIX_HEIGHT, 16);

		}
	}

};
CircleBeat circleB1(10, 12, 20, 8, CHSV(200, 255, 255), 1, 0);
CircleBeat circleB2(10, 12, 32, 4, CHSV(120, 255, 255), 1, 1);

void setup()
{
	Serial.begin(115200);
  LEDS.addLeds<LED_TYPE, DATA_PIN, CLOCK_PIN, COLOR_ORDER>(leds[0], leds.Size()).setCorrection(TypicalSMD5050);
  FastLED.clear(true);
  Serial.print("Program MAtrix Example started  ");// debug

  FastLED.setBrightness(127);
  FastLED.clear(true);
  delay(500);
  FastLED.showColor(CRGB::Red);
  delay(1000);
  FastLED.showColor(CRGB::Lime);
  delay(1000);
  FastLED.showColor(CRGB::Blue);
  delay(1000);
  FastLED.showColor(CRGB::White);
  delay(1000);
  drawEstrella(10, 12, 2, 0, 10, CHSV(HUE_RED,255,255));
  blur2d((leds[0]), MATRIX_WIDTH, MATRIX_HEIGHT, 32);

  delay(2000);
  FastLED.clear(true);
  for (uint16_t i = 0; i <= 1024; i++)
  {
	  //DrawVentilador(10, 12, 5, i, CHSV(0, 255, 255));
	  drawLineByAngle(10, 12, i, 0, 7,CHSV (i,255,255));
	  drawLineByAngle(10, 12, i+128 , 0, 7, CHSV(i, 255, 255));
	  FastLED.show();
	  FastLED.delay(2);
	  fadeToBlackBy(leds[0], 480, 32);
  }
  for (uint16_t i = 0; i <= 5000; i++)
  {
	  circleBeat();
	  FastLED.show();
	  FastLED.delay(1);
	  fadeToBlackBy(leds[0], 480, 32);
  }


  delay(1000);

  FastLED.clear(true);

  hue = 0;
  counter = 0;
 
  
 
  Serial.print("Program MAtrix Example fin setup   ");// debug
}

void loop()
{
	//Coordinates point = Coordinates();
	EVERY_N_MILLISECONDS(5000) {                           // FastLED based non-blocking delay to update/display the sequence.
		efecto= ( efecto+1) %numEffects;

	}
  int16_t sx, sy, x, y;
  
  //long nowms = millis();
 

  h = hue;
  nivelEfecto = (efecto%numEffects);
  
  
	  if (efecto <= 3)
	  {
		  diagonalFill();
		  hue += 4;
	  }
	  if (efecto<7) {
		  HorizontalStripes();
		  hue += 4;
	  }
	  else
	  {

	  }
  
  
switch (efecto)
  {
  case 1:
		 // diagonalFill();
		  break;
  case 2:
	  cir1.Update();
	  fadeToBlackBy(leds[0], 480, 32);
	  blur2d((leds[0]), MATRIX_WIDTH, MATRIX_HEIGHT, 32);
	  break;

  case 3:
	  anillos();
  hue += 4;
	  break;
  case 4:
	  leds.HorizontalMirror();
	 
	  break;
  case 5:
	  leds.VerticalMirror();
	  
	  break;
  case 6:
	  leds.QuadrantMirror();
	  
	  break;
  case 7:
	  //circleBeat();
	  circleB1.Update();
	  circleB2.Update();
	  break;
  default:
	  break;
  }
  
  /*
 
	  

  }
  else if (counter < 375)
    leds.HorizontalMirror();
  else if (counter < 625)
    leds.VerticalMirror();
  else if (counter < 875)
    leds.QuadrantMirror();
  else if (counter < 1125)
    leds.QuadrantRotateMirror();
  else if (counter < 1250)
    ;
  else if (counter < 1500)
    leds.TriangleTopMirror();
  else if (counter < 1750)
    leds.TriangleBottomMirror();
  else if (counter < 2000)
    leds.QuadrantTopTriangleMirror();
  else if (counter < 2250)
    leds.QuadrantBottomTriangleMirror();
	  //point.fromPolar(x, y);

  counter++;
  if (counter >= 3000)
    counter = 0;

  */
  //FastLED.show();
  RenderCustomMatrix();
  fadeToBlackBy(leds[0], 480, 16);
}

void RenderCustomMatrix() {
	//memmove8(&leds, &leds2, MATRIX_SIZE * sizeof(CRGB));// dest , source 
	

	FastLED.show();
}

