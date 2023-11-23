#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"

#define MS_IN_THE_ANIMATION_CYCLE	1000000
#define NUMSEGS 200

//	This is a model of the solar system rendered using OpenGL
//
//	Planetary rotation and planetary orbit periods are done in relatively correct speeds.
//  Other than the size of the Sun, the planetary radius and orbital distance from the Sun is also
//  relatively accurate, albeit scaled to fit in a single viewport.
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Young-Joon Park

// title of these windows:

const char* WINDOWTITLE = "OpenGL / GLUT SolarSystem -- Young-Joon Park";
const char* GLUITITLE = "User Interface Window";

// what the glui package defines as true and false:

const int GLUITRUE = true;
const int GLUIFALSE = false;

// the escape key:

const int ESCAPE = 0x1b;

// initial window size:

const int INIT_WINDOW_SIZE = 600;

// size of the 3d box to be drawn:

const float BOXSIZE = 2.f;

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = 1.f;
const float SCLFACT = 0.005f;

// minimum allowable scale factor:

const float MINSCALE = 0.006f;

// scroll wheel button values:

const int SCROLL_WHEEL_UP = 3;
const int SCROLL_WHEEL_DOWN = 4;

// equivalent mouse movement when we click the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = 0.4f;

// active mouse buttons (or them together):

const int LEFT = 4;
const int MIDDLE = 2;
const int RIGHT = 1;

// which projection:

enum Projections
{
	ORTHO,
	PERSP
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[] = { 0., 0., 0., 1. };

// line width for the axes:

const GLfloat AXES_WIDTH = 3.;

// the color numbers:
// this order must match the radio button order, which must match the order of the color names,
// 	which must match the order of the color RGB values

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK
};

char* ColorNames[] =
{
	(char*)"Red",
	(char*)"Yellow",
	(char*)"Green",
	(char*)"Cyan",
	(char*)"Blue",
	(char*)"Magenta",
	(char*)"White",
	(char*)"Black"
};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[][3] =
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
	{ 1., 1., 1. },		// white
	{ 0., 0., 0. },		// black
};

// fog parameters:

const GLfloat FOGCOLOR[4] = { .0f, .0f, .0f, 1.f };
const GLenum  FOGMODE = GL_LINEAR;
const GLfloat FOGDENSITY = 0.30f;
const GLfloat FOGSTART = 1.5f;
const GLfloat FOGEND = 4.f;


// what options should we compile-in?
//#define DEMO_Z_FIGHTING
//#define DEMO_DEPTH_BUFFER


// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to force the creation of z-fighting
GLuint	OrbitList;				// object display list
GLuint	EarthList;
GLuint	SunList;
GLuint	MercuryList;
GLuint	VenusList;
GLuint	MarsList;
GLuint	JupiterList;
GLuint	SaturnList;
GLuint	UranusList;
GLuint	NeptuneList;
GLuint  StarsList;
int		MainWindow;				// window id for main graphics window
float	Scale;					// scaling factor
int		ShadowsOn;				// != 0 means to turn shadows on
int		WhichColor;				// index into Colors[ ]
int		WhichProjection;		// ORTHO or PERSP
int		Xmouse, Ymouse;			// mouse values
float White[] = { 1.,1.,1.,1. };
float	Xrot, Yrot;				// rotation angles in degrees
float   Time;				// for anims
unsigned char* Texture0;	// the texels
unsigned char* Texture1;	// the texels
unsigned char* Texture2;	// the texels
unsigned char* Texture3;	// the texels
unsigned char* Texture4;	// the texels
unsigned char* Texture5;	// the texels
unsigned char* Texture6;	// the texels
unsigned char* Texture7;	// the texels
unsigned char* Texture8;	// the texels
unsigned char* Texture9;	// the texels
unsigned int   EarthTex;	// the texture object
unsigned int   SunTex;	// the texture object
unsigned int   MercuryTex;	// the texture object
unsigned int   VenusTex;	// the texture object
unsigned int   MarsTex;	// the texture object
unsigned int   JupiterTex;	// the texture object
unsigned int   SaturnTex;	// the texture object
unsigned int   UranusTex;	// the texture object
unsigned int   NeptuneTex;	// the texture object
unsigned int   StarsTex;	// the texture object
bool		   Frozen;
bool		   ShowBackground;
bool		   ShowOrbitLines;

// function prototypes:
void	Animate();
void	Display();
void	DoAxesMenu(int);
void	DoColorMenu(int);
void	DoDepthBufferMenu(int);
void	DoDepthFightingMenu(int);
void	DoDepthMenu(int);
void	DoDebugMenu(int);
void	DoMainMenu(int);
void	DoProjectMenu(int);
void	DoRasterString(float, float, float, char*);
void	DoStrokeString(float, float, float, float, char*);
float	ElapsedSeconds();
void	InitGraphics();
void	InitLists();
void	InitMenus();
void	Keyboard(unsigned char, int, int);
void	MouseButton(int, int, int, int);
void	MouseMotion(int, int);
void	Reset();
void	Resize(int, int);
void	Visibility(int);
float* Array3(float, float, float);
float* MulArray3(float, float[3]);
void SetPointLight(int, float, float, float, float, float, float);
void SetSpotLight(int, float, float, float, float, float, float, float, float, float);
void SetMaterial(float, float, float, float);

void			Axes(float);

unsigned char* BmpToTexture(char*, int*, int*);
int				ReadInt(FILE*);
short			ReadShort(FILE*);

void			HsvRgb(float[3], float[3]);

void			Cross(float[3], float[3], float[3]);
float			Dot(float[3], float[3]);
float			Unit(float[3], float[3]);

// OSUSphere
#ifndef POINT_H
#define POINT_H
struct point
{
	float x, y, z;		// coordinates
	float nx, ny, nz;	// surface normal
	float s, t;		// texture coords
};

inline
void
DrawPoint(struct point* p)
{
	glNormal3fv(&p->nx);
	glTexCoord2fv(&p->s);
	glVertex3fv(&p->x);
}
#endif

int		SphNumLngs, SphNumLats;
struct point* SphPts;

inline
struct point*
	SphPtsPointer(int lat, int lng)
{
	if (lat < 0)			lat += (SphNumLats - 1);
	if (lng < 0)			lng += (SphNumLngs - 0);
	if (lat > SphNumLats - 1)	lat -= (SphNumLats - 1);
	if (lng > SphNumLngs - 1)	lng -= (SphNumLngs - 0);
	return &SphPts[SphNumLngs * lat + lng];
}

void
OsuSphere(float radius, int slices, int stacks)
{
	// set the globals:

	SphNumLngs = slices;
	SphNumLats = stacks;
	if (SphNumLngs < 3)
		SphNumLngs = 3;
	if (SphNumLats < 3)
		SphNumLats = 3;

	// allocate the point data structure:

	SphPts = new struct point[SphNumLngs * SphNumLats];

	// fill the SphPts structure:

	for (int ilat = 0; ilat < SphNumLats; ilat++)
	{
		float lat = -M_PI / 2. + M_PI * (float)ilat / (float)(SphNumLats - 1);	// ilat=0/lat=0. is the south pole
											// ilat=SphNumLats-1, lat=+M_PI/2. is the north pole
		float xz = cosf(lat);
		float  y = sinf(lat);
		for (int ilng = 0; ilng < SphNumLngs; ilng++)				// ilng=0, lng=-M_PI and
											// ilng=SphNumLngs-1, lng=+M_PI are the same meridian
		{
			float lng = -M_PI + 2. * M_PI * (float)ilng / (float)(SphNumLngs - 1);
			float x = xz * cosf(lng);
			float z = -xz * sinf(lng);
			struct point* p = SphPtsPointer(ilat, ilng);
			p->x = radius * x;
			p->y = radius * y;
			p->z = radius * z;
			p->nx = x;
			p->ny = y;
			p->nz = z;
			p->s = (lng + M_PI) / (2. * M_PI);
			p->t = (lat + M_PI / 2.) / M_PI;
		}
	}

	struct point top, bot;		// top, bottom points

	top.x = 0.;		top.y = radius;	top.z = 0.;
	top.nx = 0.;		top.ny = 1.;		top.nz = 0.;
	top.s = 0.;		top.t = 1.;

	bot.x = 0.;		bot.y = -radius;	bot.z = 0.;
	bot.nx = 0.;		bot.ny = -1.;		bot.nz = 0.;
	bot.s = 0.;		bot.t = 0.;

	// connect the north pole to the latitude SphNumLats-2:

	glBegin(GL_TRIANGLE_STRIP);
	for (int ilng = 0; ilng < SphNumLngs; ilng++)
	{
		float lng = -M_PI + 2. * M_PI * (float)ilng / (float)(SphNumLngs - 1);
		top.s = (lng + M_PI) / (2. * M_PI);
		DrawPoint(&top);
		struct point* p = SphPtsPointer(SphNumLats - 2, ilng);	// ilat=SphNumLats-1 is the north pole
		DrawPoint(p);
	}
	glEnd();

	// connect the south pole to the latitude 1:

	glBegin(GL_TRIANGLE_STRIP);
	for (int ilng = SphNumLngs - 1; ilng >= 0; ilng--)
	{
		float lng = -M_PI + 2. * M_PI * (float)ilng / (float)(SphNumLngs - 1);
		bot.s = (lng + M_PI) / (2. * M_PI);
		DrawPoint(&bot);
		struct point* p = SphPtsPointer(1, ilng);					// ilat=0 is the south pole
		DrawPoint(p);
	}
	glEnd();

	// connect the horizontal strips:

	for (int ilat = 2; ilat < SphNumLats - 1; ilat++)
	{
		struct point* p;
		glBegin(GL_TRIANGLE_STRIP);
		for (int ilng = 0; ilng < SphNumLngs; ilng++)
		{
			p = SphPtsPointer(ilat, ilng);
			DrawPoint(p);
			p = SphPtsPointer(ilat - 1, ilng);
			DrawPoint(p);
		}
		glEnd();
	}

	// clean-up:

	delete[] SphPts;
	SphPts = NULL;
}

// Light-setting helpers
float*
Array3(float a, float b, float c)
{
	static float array[4];
	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return array;
}
// utility to create an array from a multiplier and an array:
float*
MulArray3(float factor, float array0[3])
{
	static float array[4];
	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor * array0[2];
	array[3] = 1.;
	return array;
}

void
SetPointLight(int ilight, float x, float y, float z, float r, float g, float b)
{
	glLightfv(ilight, GL_POSITION, Array3(x, y, z));
	glLightfv(ilight, GL_AMBIENT, Array3(0., 0., 0.));
	glLightfv(ilight, GL_DIFFUSE, Array3(r, g, b));
	glLightfv(ilight, GL_SPECULAR, Array3(r, g, b));
	glLightf(ilight, GL_CONSTANT_ATTENUATION, 1.);
	glLightf(ilight, GL_LINEAR_ATTENUATION, 0.);
	glLightf(ilight, GL_QUADRATIC_ATTENUATION, 0.);
	glEnable(ilight);
}
void
SetSpotLight(int ilight, float x, float y, float z, float xdir, float ydir, float zdir, float r, float g, float b)
{
	glLightfv(ilight, GL_POSITION, Array3(x, y, z));
	glLightfv(ilight, GL_SPOT_DIRECTION, Array3(xdir, ydir, zdir));
	glLightf(ilight, GL_SPOT_EXPONENT, 1.);
	glLightf(ilight, GL_SPOT_CUTOFF, 45.);
	glLightfv(ilight, GL_AMBIENT, Array3(0., 0., 0.));
	glLightfv(ilight, GL_DIFFUSE, Array3(r, g, b));
	glLightfv(ilight, GL_SPECULAR, Array3(r, g, b));
	glLightf(ilight, GL_CONSTANT_ATTENUATION, 1.);
	glLightf(ilight, GL_LINEAR_ATTENUATION, 0.);
	glLightf(ilight, GL_QUADRATIC_ATTENUATION, 0.);
	glEnable(ilight);
}
void
SetMaterial(float r, float g, float b, float shininess)
{
	glMaterialfv(GL_BACK, GL_EMISSION, Array3(0., 0., 0.));
	glMaterialfv(GL_BACK, GL_AMBIENT, MulArray3(.4f, White));
	glMaterialfv(GL_BACK, GL_DIFFUSE, MulArray3(1., White));
	glMaterialfv(GL_BACK, GL_SPECULAR, Array3(0., 0., 0.));
	glMaterialf(GL_BACK, GL_SHININESS, 2.f);
	glMaterialfv(GL_FRONT, GL_EMISSION, Array3(0., 0., 0.));
	glMaterialfv(GL_FRONT, GL_AMBIENT, Array3(r, g, b));
	glMaterialfv(GL_FRONT, GL_DIFFUSE, Array3(r, g, b));
	glMaterialfv(GL_FRONT, GL_SPECULAR, MulArray3(.8f, White));
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}
// main program:
int
main(int argc, char* argv[])
{
	// turn on the glut package:

	glutInit(&argc, argv);

	// setup all the graphics:

	InitGraphics();

	// create the display structures that will not change:

	InitLists();

	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset();

	// setup all the user interface:

	InitMenus();

	// draw the scene once and wait for some interaction:

	glutSetWindow(MainWindow);
	glutMainLoop();

	return 0;
}

void
Animate()
{
	int ms = glutGet(GLUT_ELAPSED_TIME);	// milliseconds
	ms %= MS_IN_THE_ANIMATION_CYCLE;
	Time = (float)ms / (float)MS_IN_THE_ANIMATION_CYCLE;        // [ 0., 1. )
	// force a call to Display( ) next time it is convenient:

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// draw the complete scene:

void
Display()
{	
	
	// set which window we want to do the graphics into:

	glutSetWindow(MainWindow);


	// erase the background:

	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
#ifdef DEMO_DEPTH_BUFFER
	if (DepthBufferOn == 0)
		glDisable(GL_DEPTH_TEST);
#endif


	// specify shading to be flat:

	glShadeModel(GL_FLAT);


	// set the viewport to a square centered in the window:

	GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = (vx - v) / 2;
	GLint yb = (vy - v) / 2;
	glViewport(xl, yb, v, v);


	// set the viewing volume:
	// the Z clipping  values are actually given as DISTANCES IN FRONT OF THE EYE

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (WhichProjection == ORTHO)
		glOrtho(-2.f, 2.f, -2.f, 2.f, 0.1f, 1000.f);
	else
		gluPerspective(70.f, 1.f, 0.1f, 1000.f);


	// place the objects into the scene:

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	// set the eye position, look-at position, and up-vector:

	gluLookAt(0.f, 0.f, 3.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);


	// rotate the scene:

	glRotatef((GLfloat)Yrot, 0.f, 1.f, 0.f);
	glRotatef((GLfloat)Xrot, 1.f, 0.f, 0.f);


	// uniformly scale the scene:

	if (Scale < MINSCALE)
		Scale = MINSCALE;
	glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);


	// set the fog parameters:

	if (DepthCueOn != 0)
	{
		glFogi(GL_FOG_MODE, FOGMODE);
		glFogfv(GL_FOG_COLOR, FOGCOLOR);
		glFogf(GL_FOG_DENSITY, FOGDENSITY);
		glFogf(GL_FOG_START, FOGSTART);
		glFogf(GL_FOG_END, FOGEND);
		glEnable(GL_FOG);
	}
	else
	{
		glDisable(GL_FOG);
	}


	// possibly draw the axes:

	if (AxesOn != 0)
	{
		glColor3fv(&Colors[WhichColor][0]);
		glCallList(AxesList);
	}


	// for glScalef( ), be sure the normals get unitized:

	glEnable(GL_NORMALIZE);
	// Lighting
	SetPointLight(GL_LIGHT0, 0., 0., 0., 1., 1., 1.);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, MulArray3(.3f, White));
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	
	SetMaterial(1., 1., 1., 0.);
	// Globe
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	if (ShowOrbitLines) {
		glCallList(OrbitList);
	}
	glPopMatrix();
	// Earth
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, EarthTex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glRotatef(Time * 360. * 164.904, 0., 0., 1.);
	glTranslatef(0., 10., 0.);
	glRotatef(90., 0., 1., 0.);
	glRotatef(90., 1., 0., 0.);
	glRotatef(90., 0., 0., 1.);
	glRotatef(Time * 360. * 225, 0., 1., 0.);
	glCallList(EarthList);
	glPopMatrix();
	// Sun
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, SunTex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glCallList(SunList);
	glPopMatrix();
	// Mercury
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, MercuryTex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glRotatef(Time * 360. * 683.977, 0., 0., 1.);
	glTranslatef(0., 3.9, 0.);
	glRotatef(90., 0., 1., 0.);
	glRotatef(90., 1., 0., 0.);
	glRotatef(90., 0., 0., 1.);
	glRotatef(Time * 360. * 3.81, 0., 1., 0.);
	glCallList(MercuryList);
	glPopMatrix();
	// Venus
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, VenusTex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glRotatef(Time * -360. * 267.511, 0., 0., 1.);
	glTranslatef(0., 7.2, 0.);
	glRotatef(90., 0., 1., 0.);
	glRotatef(90., 1., 0., 0.);
	glRotatef(90., 0., 0., 1.);
	glRotatef(Time * 360. * 1, 0., 1., 0.);
	glCallList(VenusList);
	glPopMatrix();
	// Mars
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, MarsTex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glRotatef(Time * 360. * 87.613, 0., 0., 1.);
	glTranslatef(0., 15.2, 0.);
	glRotatef(90., 0., 1., 0.);
	glRotatef(90., 1., 0., 0.);
	glRotatef(90., 0., 0., 1.);
	glRotatef(Time * 360. * 225, 0., 1., 0.);
	glCallList(MarsList);
	glPopMatrix();
	// Jupiter
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, JupiterTex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glRotatef(Time * 360. * 13.894, 0., 0., 1.);
	glTranslatef(0., 52., 0.);
	glRotatef(90., 0., 1., 0.);
	glRotatef(90., 1., 0., 0.);
	glRotatef(90., 0., 0., 1.);
	glRotatef(Time * 360. * 540, 0., 1., 0.);
	glCallList(JupiterList);
	glPopMatrix();
	// Saturn
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, SaturnTex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glRotatef(Time * 360. * 5.594, 0., 0., 1.);
	glTranslatef(0., 95.4, 0.);
	glRotatef(90., 0., 1., 0.);
	glRotatef(90., 1., 0., 0.);
	glRotatef(90., 0., 0., 1.);
	glRotatef(Time * 360. * 540, 0., 1., 0.);
	glCallList(SaturnList);
	glPopMatrix();
	// Uranus
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, UranusTex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glRotatef(Time * 360. * 1.955, 0., 0., 1.);
	glTranslatef(0., 192., 0.);
	glRotatef(90., 0., 1., 0.);
	glRotatef(90., 1., 0., 0.);
	glRotatef(90., 0., 0., 1.);
	glRotatef(Time * 360. * 317, 0., 1., 0.);
	glCallList(UranusList);
	glPopMatrix();
	// Neptune
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, NeptuneTex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glRotatef(Time * 360. * 1, 0., 0., 1.);
	glTranslatef(0., 300., 0.);
	glRotatef(90., 0., 1., 0.);
	glRotatef(90., 1., 0., 0.);
	glRotatef(90., 0., 0., 1.);
	glRotatef(Time * 360. * 338, 0., 1., 0.);
	glCallList(NeptuneList);
	glPopMatrix();

	// Stars
	glPushMatrix();
	if (ShowBackground) {
		glBindTexture(GL_TEXTURE_2D, StarsTex);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glCallList(StarsList);
	}
	glPopMatrix();

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);


#ifdef DEMO_Z_FIGHTING
	if (DepthFightingOn != 0)
	{
		glPushMatrix();
		glRotatef(90.f, 0.f, 1.f, 0.f);
		glCallList(BoxList);
		glPopMatrix();
	}
#endif


	// draw some gratuitous text that just rotates on top of the scene:

	glDisable(GL_DEPTH_TEST);
	glColor3f(0.f, 1.f, 1.f);
	//DoRasterString( 0.f, 1.f, 0.f, (char *)"Text That Moves" );


	// draw some gratuitous text that is fixed on the screen:

	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.f, 100.f, 0.f, 100.f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(1.f, 1.f, 1.f);
	//DoRasterString( 5.f, 5.f, 0.f, (char *)"Text That Doesn't" );

	// swap the double-buffered framebuffers:

	glutSwapBuffers();

	// be sure the graphics buffer has been sent:

	glFlush();
}


void
DoAxesMenu(int id)
{
	AxesOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoColorMenu(int id)
{
	WhichColor = id - RED;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDebugMenu(int id)
{
	DebugOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDepthBufferMenu(int id)
{
	DepthBufferOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDepthFightingMenu(int id)
{
	DepthFightingOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDepthMenu(int id)
{
	DepthCueOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// main menu callback:

void
DoMainMenu(int id)
{
	switch (id)
	{
	case RESET:
		Reset();
		break;

	case QUIT:
		// gracefully close out the graphics:
		// gracefully close the graphics window:
		// gracefully exit the program:
		glutSetWindow(MainWindow);
		glFinish();
		glutDestroyWindow(MainWindow);
		exit(0);
		break;

	default:
		fprintf(stderr, "Don't know what to do with Main Menu ID %d\n", id);
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoProjectMenu(int id)
{
	WhichProjection = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// use glut to display a string of characters using a raster font:

void
DoRasterString(float x, float y, float z, char* s)
{
	glRasterPos3f((GLfloat)x, (GLfloat)y, (GLfloat)z);

	char c;			// one character to print
	for (; (c = *s) != '\0'; s++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString(float x, float y, float z, float ht, char* s)
{
	glPushMatrix();
	glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
	float sf = ht / (119.05f + 33.33f);
	glScalef((GLfloat)sf, (GLfloat)sf, (GLfloat)sf);
	char c;			// one character to print
	for (; (c = *s) != '\0'; s++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
	}
	glPopMatrix();
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds()
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet(GLUT_ELAPSED_TIME);

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus()
{
	glutSetWindow(MainWindow);

	int numColors = sizeof(Colors) / (3 * sizeof(int));
	int colormenu = glutCreateMenu(DoColorMenu);
	for (int i = 0; i < numColors; i++)
	{
		glutAddMenuEntry(ColorNames[i], i);
	}

	int axesmenu = glutCreateMenu(DoAxesMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthcuemenu = glutCreateMenu(DoDepthMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthbuffermenu = glutCreateMenu(DoDepthBufferMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthfightingmenu = glutCreateMenu(DoDepthFightingMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int debugmenu = glutCreateMenu(DoDebugMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int projmenu = glutCreateMenu(DoProjectMenu);
	glutAddMenuEntry("Orthographic", ORTHO);
	glutAddMenuEntry("Perspective", PERSP);

	int mainmenu = glutCreateMenu(DoMainMenu);
	glutAddSubMenu("Axes", axesmenu);
	glutAddSubMenu("Axis Colors", colormenu);

#ifdef DEMO_DEPTH_BUFFER
	glutAddSubMenu("Depth Buffer", depthbuffermenu);
#endif

#ifdef DEMO_Z_FIGHTING
	glutAddSubMenu("Depth Fighting", depthfightingmenu);
#endif

	glutAddSubMenu("Depth Cue", depthcuemenu);
	glutAddSubMenu("Projection", projmenu);
	glutAddMenuEntry("Reset", RESET);
	glutAddSubMenu("Debug", debugmenu);
	glutAddMenuEntry("Quit", QUIT);

	// attach the pop-up menu to the right mouse button:

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}



// initialize the glut and OpenGL libraries:
//	also setup callback functions

void
InitGraphics()
{
	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	// set the initial window configuration:

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(INIT_WINDOW_SIZE, INIT_WINDOW_SIZE);

	// open the window and set its title:

	MainWindow = glutCreateWindow(WINDOWTITLE);
	glutSetWindowTitle(WINDOWTITLE);

	// set the framebuffer clear values:

	glClearColor(BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3]);

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow(MainWindow);
	glutDisplayFunc(Display);
	glutReshapeFunc(Resize);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(MouseMotion);
	//glutPassiveMotionFunc( NULL );
	glutVisibilityFunc(Visibility);
	glutEntryFunc(NULL);
	glutSpecialFunc(NULL);
	glutSpaceballMotionFunc(NULL);
	glutSpaceballRotateFunc(NULL);
	glutSpaceballButtonFunc(NULL);
	glutButtonBoxFunc(NULL);
	glutDialsFunc(NULL);
	glutTabletMotionFunc(NULL);
	glutTabletButtonFunc(NULL);
	glutMenuStateFunc(NULL);
	glutTimerFunc(-1, NULL, 0);

	// setup glut to call Animate( ) every time it has
	// 	nothing it needs to respond to (which is most of the time)

	glutIdleFunc(Animate);

	// Textures for Sun and planets
	int width0, height0, width1, height1, width2, height2, width3, height3, width4, height4, width5, height5, width6, height6, width7, height7, width8, height8, width9, height9;
	// EARTH
	Texture0 = BmpToTexture((char*)"./bmp/earthtex.bmp", &width0, &height0);
	if (Texture0 == NULL)
		fprintf(stderr, "Cannot open texture '%s'\n", "./bmp/earthtex.bmp");
	else
		fprintf(stderr, "Width = %d ; Height = %d\n", width0, height0);

	// SUN
	Texture1 = BmpToTexture((char*)"./bmp/suntex.bmp", &width1, &height1);
	if (Texture1 == NULL)
		fprintf(stderr, "Cannot open texture '%s'\n", "./bmp/suntex.bmp");
	else
		fprintf(stderr, "Width = %d ; Height = %d\n", width1, height1);
	// MERCURY
	Texture2 = BmpToTexture((char*)"./bmp/mercurytex.bmp", &width2, &height2);
	if (Texture2 == NULL)
		fprintf(stderr, "Cannot open texture '%s'\n", "./bmp/mercurytex.bmp");
	else
		fprintf(stderr, "Width = %d ; Height = %d\n", width2, height2);
	// VENUS
	Texture3 = BmpToTexture((char*)"./bmp/venustex.bmp", &width3, &height3);
	if (Texture3 == NULL)
		fprintf(stderr, "Cannot open texture '%s'\n", "./bmp/venustex.bmp");
	else
		fprintf(stderr, "Width = %d ; Height = %d\n", width3, height3);
	// MARS
	Texture4 = BmpToTexture((char*)"./bmp/marstex.bmp", &width4, &height4);
	if (Texture4 == NULL)
		fprintf(stderr, "Cannot open texture '%s'\n", "./bmp/marstex.bmp");
	else
		fprintf(stderr, "Width = %d ; Height = %d\n", width4, height4);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// JUPITER
	Texture5 = BmpToTexture((char*)"./bmp/jupitertex.bmp", &width5, &height5);
	if (Texture5 == NULL)
		fprintf(stderr, "Cannot open texture '%s'\n", "./bmp/jupitertex.bmp");
	else
		fprintf(stderr, "Width = %d ; Height = %d\n", width5, height5);
	// SATURN
	Texture6 = BmpToTexture((char*)"./bmp/saturntex.bmp", &width6, &height6);
	if (Texture6 == NULL)
		fprintf(stderr, "Cannot open texture '%s'\n", "./bmp/saturntex.bmp");
	else
		fprintf(stderr, "Width = %d ; Height = %d\n", width6, height6);
	// URANUS
	Texture7 = BmpToTexture((char*)"./bmp/uranustex.bmp", &width7, &height7);
	if (Texture7 == NULL)
		fprintf(stderr, "Cannot open texture '%s'\n", "./bmp/uranustex.bmp");
	else
		fprintf(stderr, "Width = %d ; Height = %d\n", width7, height7);
	// NEPTUNE
	Texture8 = BmpToTexture((char*)"./bmp/neptunetex.bmp", &width8, &height8);
	if (Texture8 == NULL)
		fprintf(stderr, "Cannot open texture '%s'\n", "./bmp/neptunetex.bmp");
	else
		fprintf(stderr, "Width = %d ; Height = %d\n", width8, height8);
	// STARS
	Texture9 = BmpToTexture((char*)"./bmp/starstex.bmp", &width9, &height9);
	if (Texture1 == NULL)
		fprintf(stderr, "Cannot open texture '%s'\n", "./bmp/starstex.bmp");
	else
		fprintf(stderr, "Width = %d ; Height = %d\n", width9, height9);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// assign binding handles
	glGenTextures(1, &EarthTex);
	glGenTextures(1, &SunTex);
	glGenTextures(1, &MercuryTex);
	glGenTextures(1, &VenusTex);
	glGenTextures(1, &MarsTex);
	glGenTextures(1, &JupiterTex);
	glGenTextures(1, &SaturnTex);
	glGenTextures(1, &UranusTex);
	glGenTextures(1, &NeptuneTex);
	glGenTextures(1, &StarsTex);
	// Earth
	glBindTexture(GL_TEXTURE_2D, EarthTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width0, height0, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture0);
	// Sun
	glBindTexture(GL_TEXTURE_2D, SunTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture1);
	// Mercury
	glBindTexture(GL_TEXTURE_2D, MercuryTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture2);
	// Venus
	glBindTexture(GL_TEXTURE_2D, VenusTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width3, height3, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture3);
	// Mars
	glBindTexture(GL_TEXTURE_2D, MarsTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width4, height4, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture4);
	// Jupiter
	glBindTexture(GL_TEXTURE_2D, JupiterTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width5, height5, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture5);
	// Saturn
	glBindTexture(GL_TEXTURE_2D, SaturnTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width6, height6, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture6);
	// Uranus
	glBindTexture(GL_TEXTURE_2D, UranusTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width7, height7, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture7);
	// Neptune
	glBindTexture(GL_TEXTURE_2D, NeptuneTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width8, height8, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture8);
	// Stars
	glBindTexture(GL_TEXTURE_2D, StarsTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width9, height9, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture9);
	// init the glew package (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		fprintf(stderr, "glewInit Error\n");
	}
	else
		fprintf(stderr, "GLEW initialized OK\n");
	fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists()
{
	float dx = BOXSIZE / 2.f;
	float dy = BOXSIZE / 2.f;
	float dz = BOXSIZE / 2.f;
	glutSetWindow(MainWindow);

	// create the object:
	OrbitList = glGenLists(1);
	glNewList(OrbitList, GL_COMPILE);
		glColor3f(0., 0., 0.);
		float dang = 2. * M_PI / (float)(199);
		float ang = 0.;
		// Earth
		glBegin(GL_LINE_LOOP);
		for (int i = 0; i < NUMSEGS; i++)
		{
			glVertex3f(10 * cos(ang), 10 * sin(ang), 0.);
			ang += dang;
		}
		glEnd();
		// Mercury
		glBegin(GL_LINE_LOOP);
		for (int i = 0; i < NUMSEGS; i++)
		{
			glVertex3f(3.9 * cos(ang), 3.9 * sin(ang), 0.);
			ang += dang;
		}
		glEnd();
		// Venus
		glBegin(GL_LINE_LOOP);
		for (int i = 0; i < NUMSEGS; i++)
		{
			glVertex3f(7.2 * cos(ang), 7.2 * sin(ang), 0.);
			ang += dang;
		}
		glEnd();
		// Mars
		glBegin(GL_LINE_LOOP);
		for (int i = 0; i < NUMSEGS; i++)
		{
			glVertex3f(15.2 * cos(ang), 15.2 * sin(ang), 0.);
			ang += dang;
		}
		glEnd();
		// Jupiter
		glBegin(GL_LINE_LOOP);
		for (int i = 0; i < NUMSEGS; i++)
		{
			glVertex3f(52. * cos(ang), 52. * sin(ang), 0.);
			ang += dang;
		}
		glEnd();
		// Saturn
		glBegin(GL_LINE_LOOP);
		for (int i = 0; i < NUMSEGS; i++)
		{
			glVertex3f(95.4 * cos(ang), 95.4 * sin(ang), 0.);
			ang += dang;
		}
		glEnd();
		// Uranus
		glBegin(GL_LINE_LOOP);
		for (int i = 0; i < NUMSEGS; i++)
		{
			glVertex3f(192. * cos(ang), 192. * sin(ang), 0.);
			ang += dang;
		}
		glEnd();
		// Neptune
		glBegin(GL_LINE_LOOP);
		for (int i = 0; i < NUMSEGS; i++)
		{
			glVertex3f(300. * cos(ang), 300. * sin(ang), 0.);
			ang += dang;
		}
		glEnd();
	glEndList();
	// earth list
	EarthList = glGenLists(1);
	glNewList(EarthList, GL_COMPILE);
		glColor3f(0.04, 0.04, 0.04);
		OsuSphere(0.635, 512, 512);
		glTranslatef(0., -10., 0.);
	glEndList();
	// sun list
	SunList = glGenLists(1);
	glNewList(SunList, GL_COMPILE);
		glColor3f(0.04, 0.04, 0.04);
		OsuSphere(3., 512, 512);
	glEndList();
	// mercury list
	MercuryList = glGenLists(1);
	glNewList(MercuryList, GL_COMPILE);
		glColor3f(0.04, 0.04, 0.04);
		OsuSphere(0.24, 512, 512);
		glTranslatef(0., -3.9, 0.);
	glEndList();
	// venus list
	VenusList = glGenLists(1);
	glNewList(VenusList, GL_COMPILE);
		glColor3f(0.04, 0.04, 0.04);
		OsuSphere(0.4, 512, 512);
		glTranslatef(0., -7.2, 0.);
	glEndList();
	// mars list
	MarsList = glGenLists(1);
	glNewList(MarsList, GL_COMPILE);
		glColor3f(0.04, 0.04, 0.04);
		OsuSphere(0.34, 512, 512);
		glTranslatef(0., -15.2, 0.);
	glEndList();
	// jupiter list
	JupiterList = glGenLists(1);
	glNewList(JupiterList, GL_COMPILE);
		glColor3f(0.04, 0.04, 0.04);
		OsuSphere(7.15, 512, 512);
		glTranslatef(0., -52., 0.);
	glEndList();
	// saturn list
	SaturnList = glGenLists(1);
	glNewList(SaturnList, GL_COMPILE);
		glColor3f(0.04, 0.04, 0.04);
		OsuSphere(5.8, 512, 512);
		glTranslatef(0., -95.4, 0.);
	glEndList();
	// uranus list
	UranusList = glGenLists(1);
	glNewList(UranusList, GL_COMPILE);
		glColor3f(0.04, 0.04, 0.04);
		OsuSphere(2.5, 512, 512);
		glTranslatef(0., -192., 0.);
	glEndList();	
	// neptune list
	NeptuneList = glGenLists(1);
	glNewList(NeptuneList, GL_COMPILE);
		glColor3f(0.04, 0.04, 0.04);
		OsuSphere(1.2, 512, 512);
		glTranslatef(0., -300., 0.);
	glEndList();
	// stars list
	StarsList = glGenLists(1);
		glNewList(StarsList, GL_COMPILE);
		glColor3f(0.04, 0.04, 0.04);
		OsuSphere(500., 512, 512);
	glEndList();

	// create the axes:

	AxesList = glGenLists(1);
	glNewList(AxesList, GL_COMPILE);
	glLineWidth(AXES_WIDTH);
	Axes(1.5);
	glLineWidth(1.);
	glEndList();
}


// the keyboard callback:

void
Keyboard(unsigned char c, int x, int y)
{
	if (DebugOn != 0)
		fprintf(stderr, "Keyboard: '%c' (0x%0x)\n", c, c);

	switch (c)
	{
	case 'o':
	case 'O':
		WhichProjection = ORTHO;
		break;

	case 'p':
	case 'P':
		WhichProjection = PERSP;
		break;

	case 'q':
	case 'Q':
	case ESCAPE:
		DoMainMenu(QUIT);	// will not return here
		break;				// happy compiler

	case 'f':
	case 'F':
		Frozen = !Frozen;
		if (Frozen)
			glutIdleFunc(NULL);
		else
			glutIdleFunc(Animate);
		break;

	case '1':
		ShowBackground = !ShowBackground;
		break;

	case '2':
		ShowOrbitLines = !ShowOrbitLines;
		break;

	default:
		fprintf(stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c);
	}

	// force a call to Display( ):

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// called when the mouse button transitions down or up:

void
MouseButton(int button, int state, int x, int y)
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if (DebugOn != 0)
		fprintf(stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y);


	// get the proper button bit mask:

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		b = LEFT;		break;

	case GLUT_MIDDLE_BUTTON:
		b = MIDDLE;		break;

	case GLUT_RIGHT_BUTTON:
		b = RIGHT;		break;

	case SCROLL_WHEEL_UP:
		Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
		// keep object from turning inside-out or disappearing:
		if (Scale < MINSCALE)
			Scale = MINSCALE;
		break;

	case SCROLL_WHEEL_DOWN:
		Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
		// keep object from turning inside-out or disappearing:
		if (Scale < MINSCALE)
			Scale = MINSCALE;
		break;

	default:
		b = 0;
		fprintf(stderr, "Unknown mouse button: %d\n", button);
	}

	// button down sets the bit, up clears the bit:

	if (state == GLUT_DOWN)
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();

}


// called when the mouse moves while a button is down:

void
MouseMotion(int x, int y)
{
	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if ((ActiveButton & LEFT) != 0)
	{
		Xrot += (ANGFACT * dy);
		Yrot += (ANGFACT * dx);
	}

	if ((ActiveButton & MIDDLE) != 0)
	{
		Scale += SCLFACT * (float)(dx - dy);

		// keep object from turning inside-out or disappearing:

		if (Scale < MINSCALE)
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset()
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale = 1.0;
	ShadowsOn = 0;
	WhichColor = WHITE;
	WhichProjection = PERSP;
	Xrot = Yrot = 0.;
	Frozen = false;
	ShowBackground = true;
	ShowOrbitLines = true;
}


// called when user resizes the window:

void
Resize(int width, int height)
{
	// window size is checked each time in Display( ):

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}
// handle a change to the window's visibility:

void
Visibility(int state)
{
	if (DebugOn != 0)
		fprintf(stderr, "Visibility: %d\n", state);

	if (state == GLUT_VISIBLE)
	{
		glutSetWindow(MainWindow);
		glutPostRedisplay();
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it here
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[] = { 0.f, 1.f, 0.f, 1.f };

static float xy[] = { -.5f, .5f, .5f, -.5f };

static int xorder[] = { 1, 2, -3, 4 };

static float yx[] = { 0.f, 0.f, -.5f, .5f };

static float yy[] = { 0.f, .6f, 1.f, 1.f };

static int yorder[] = { 1, 2, 3, -2, 4 };

static float zx[] = { 1.f, 0.f, 1.f, 0.f, .25f, .75f };

static float zy[] = { .5f, .5f, -.5f, -.5f, 0.f, 0.f };

static int zorder[] = { 1, 2, 3, 4, -5, 6 };

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes(float length)
{
	glBegin(GL_LINE_STRIP);
	glVertex3f(length, 0., 0.);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., length, 0.);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., 0., length);
	glEnd();

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 4; i++)
	{
		int j = xorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(base + fact * xx[j], fact * xy[j], 0.0);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 5; i++)
	{
		int j = yorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(fact * yx[j], base + fact * yy[j], 0.0);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 6; i++)
	{
		int j = zorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(0.0, fact * zy[j], base + fact * zx[j]);
	}
	glEnd();

}

// read a BMP file into a Texture:

#define VERBOSE				false
#define BMP_MAGIC_NUMBER	0x4d42
#ifndef BI_RGB
#define BI_RGB				0
#define BI_RLE8				1
#define BI_RLE4				2
#endif


// bmp file header:
struct bmfh
{
	short bfType;		// BMP_MAGIC_NUMBER = "BM"
	int bfSize;		// size of this file in bytes
	short bfReserved1;
	short bfReserved2;
	int bfOffBytes;		// # bytes to get to the start of the per-pixel data
} FileHeader;

// bmp info header:
struct bmih
{
	int biSize;		// info header size, should be 40
	int biWidth;		// image width
	int biHeight;		// image height
	short biPlanes;		// #color planes, should be 1
	short biBitCount;	// #bits/pixel, should be 1, 4, 8, 16, 24, 32
	int biCompression;	// BI_RGB, BI_RLE4, BI_RLE8
	int biSizeImage;
	int biXPixelsPerMeter;
	int biYPixelsPerMeter;
	int biClrUsed;		// # colors in the palette
	int biClrImportant;
} InfoHeader;



// read a BMP file into a Texture:

unsigned char*
BmpToTexture(char* filename, int* width, int* height)
{
	FILE* fp;
#ifdef _WIN32
	errno_t err = fopen_s(&fp, filename, "rb");
	if (err != 0)
	{
		fprintf(stderr, "Cannot open Bmp file '%s'\n", filename);
		return NULL;
	}
#else
	fp = fopen(filename, "rb");
	if (fp == NULL)
	{
		fprintf(stderr, "Cannot open Bmp file '%s'\n", filename);
		return NULL;
	}
#endif

	FileHeader.bfType = ReadShort(fp);


	// if bfType is not BMP_MAGIC_NUMBER, the file is not a bmp:

	if (VERBOSE) fprintf(stderr, "FileHeader.bfType = 0x%0x = \"%c%c\"\n",
		FileHeader.bfType, FileHeader.bfType & 0xff, (FileHeader.bfType >> 8) & 0xff);
	if (FileHeader.bfType != BMP_MAGIC_NUMBER)
	{
		fprintf(stderr, "Wrong type of file: 0x%0x\n", FileHeader.bfType);
		fclose(fp);
		return NULL;
	}


	FileHeader.bfSize = ReadInt(fp);
	if (VERBOSE)	fprintf(stderr, "FileHeader.bfSize = %d\n", FileHeader.bfSize);

	FileHeader.bfReserved1 = ReadShort(fp);
	FileHeader.bfReserved2 = ReadShort(fp);

	FileHeader.bfOffBytes = ReadInt(fp);


	InfoHeader.biSize = ReadInt(fp);
	InfoHeader.biWidth = ReadInt(fp);
	InfoHeader.biHeight = ReadInt(fp);

	const int nums = InfoHeader.biWidth;
	const int numt = InfoHeader.biHeight;

	InfoHeader.biPlanes = ReadShort(fp);

	InfoHeader.biBitCount = ReadShort(fp);
	if (VERBOSE)	fprintf(stderr, "InfoHeader.biBitCount = %d\n", InfoHeader.biBitCount);

	InfoHeader.biCompression = ReadInt(fp);
	if (VERBOSE)	fprintf(stderr, "InfoHeader.biCompression = %d\n", InfoHeader.biCompression);

	InfoHeader.biSizeImage = ReadInt(fp);
	if (VERBOSE)	fprintf(stderr, "InfoHeader.biSizeImage = %d\n", InfoHeader.biSizeImage);

	InfoHeader.biXPixelsPerMeter = ReadInt(fp);
	InfoHeader.biYPixelsPerMeter = ReadInt(fp);

	InfoHeader.biClrUsed = ReadInt(fp);
	if (VERBOSE)	fprintf(stderr, "InfoHeader.biClrUsed = %d\n", InfoHeader.biClrUsed);

	InfoHeader.biClrImportant = ReadInt(fp);

	// fprintf( stderr, "Image size found: %d x %d\n", ImageWidth, ImageHeight );

	// pixels will be stored bottom-to-top, left-to-right:
	unsigned char* texture = new unsigned char[3 * nums * numt];
	if (texture == NULL)
	{
		fprintf(stderr, "Cannot allocate the texture array!\n");
		return NULL;
	}

	// extra padding bytes:

	int requiredRowSizeInBytes = 4 * ((InfoHeader.biBitCount * InfoHeader.biWidth + 31) / 32);
	if (VERBOSE)	fprintf(stderr, "requiredRowSizeInBytes = %d\n", requiredRowSizeInBytes);

	int myRowSizeInBytes = (InfoHeader.biBitCount * InfoHeader.biWidth + 7) / 8;
	if (VERBOSE)	fprintf(stderr, "myRowSizeInBytes = %d\n", myRowSizeInBytes);

	int numExtra = requiredRowSizeInBytes - myRowSizeInBytes;
	if (VERBOSE)	fprintf(stderr, "New NumExtra padding = %d\n", numExtra);


	// this function does not support compression:

	if (InfoHeader.biCompression != 0)
	{
		fprintf(stderr, "Wrong type of image compression: %d\n", InfoHeader.biCompression);
		fclose(fp);
		return NULL;
	}

	// we can handle 24 bits of direct color:
	if (InfoHeader.biBitCount == 24)
	{
		rewind(fp);
		fseek(fp, FileHeader.bfOffBytes, SEEK_SET);
		int t;
		unsigned char* tp;
		for (t = 0, tp = texture; t < numt; t++)
		{
			for (int s = 0; s < nums; s++, tp += 3)
			{
				*(tp + 2) = fgetc(fp);		// b
				*(tp + 1) = fgetc(fp);		// g
				*(tp + 0) = fgetc(fp);		// r
			}

			for (int e = 0; e < numExtra; e++)
			{
				fgetc(fp);
			}
		}
	}

	// we can also handle 8 bits of indirect color:
	if (InfoHeader.biBitCount == 8 && InfoHeader.biClrUsed == 256)
	{
		struct rgba32
		{
			unsigned char r, g, b, a;
		};
		struct rgba32* colorTable = new struct rgba32[InfoHeader.biClrUsed];

		rewind(fp);
		fseek(fp, sizeof(struct bmfh) + InfoHeader.biSize - 2, SEEK_SET);
		for (int c = 0; c < InfoHeader.biClrUsed; c++)
		{
			colorTable[c].r = fgetc(fp);
			colorTable[c].g = fgetc(fp);
			colorTable[c].b = fgetc(fp);
			colorTable[c].a = fgetc(fp);
			if (VERBOSE)	fprintf(stderr, "%4d:\t0x%02x\t0x%02x\t0x%02x\t0x%02x\n",
				c, colorTable[c].r, colorTable[c].g, colorTable[c].b, colorTable[c].a);
		}

		rewind(fp);
		fseek(fp, FileHeader.bfOffBytes, SEEK_SET);
		int t;
		unsigned char* tp;
		for (t = 0, tp = texture; t < numt; t++)
		{
			for (int s = 0; s < nums; s++, tp += 3)
			{
				int index = fgetc(fp);
				*(tp + 0) = colorTable[index].r;	// r
				*(tp + 1) = colorTable[index].g;	// g
				*(tp + 2) = colorTable[index].b;	// b
			}

			for (int e = 0; e < numExtra; e++)
			{
				fgetc(fp);
			}
		}

		delete[] colorTable;
	}

	fclose(fp);

	*width = nums;
	*height = numt;
	return texture;
}

int
ReadInt(FILE* fp)
{
	const unsigned char b0 = fgetc(fp);
	const unsigned char b1 = fgetc(fp);
	const unsigned char b2 = fgetc(fp);
	const unsigned char b3 = fgetc(fp);
	return (b3 << 24) | (b2 << 16) | (b1 << 8) | b0;
}

short
ReadShort(FILE* fp)
{
	const unsigned char b0 = fgetc(fp);
	const unsigned char b1 = fgetc(fp);
	return (b1 << 8) | b0;
}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb(float hsv[3], float rgb[3])
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while (h >= 6.)	h -= 6.;
	while (h < 0.) 	h += 6.;

	float s = hsv[1];
	if (s < 0.)
		s = 0.;
	if (s > 1.)
		s = 1.;

	float v = hsv[2];
	if (v < 0.)
		v = 0.;
	if (v > 1.)
		v = 1.;

	// if sat==0, then is a gray:

	if (s == 0.0)
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:

	float i = (float)floor(h);
	float f = h - i;
	float p = v * (1.f - s);
	float q = v * (1.f - s * f);
	float t = v * (1.f - (s * (1.f - f)));

	float r = 0., g = 0., b = 0.;			// red, green, blue
	switch ((int)i)
	{
	case 0:
		r = v;	g = t;	b = p;
		break;

	case 1:
		r = q;	g = v;	b = p;
		break;

	case 2:
		r = p;	g = v;	b = t;
		break;

	case 3:
		r = p;	g = q;	b = v;
		break;

	case 4:
		r = t;	g = p;	b = v;
		break;

	case 5:
		r = v;	g = p;	b = q;
		break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

void
Cross(float v1[3], float v2[3], float vout[3])
{
	float tmp[3];
	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}

float
Dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

float
Unit(float vin[3], float vout[3])
{
	float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}
	return dist;
}
