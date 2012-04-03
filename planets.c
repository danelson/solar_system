/*
 * Dan Nelson
 * Project 1: OpenGL
 * CS451 Advanced Graphics
 * February 16, 2012
 * Draws the solar system where planets are to scale (rotations, size, orbits)
 
 * Controls:
 	up - speed up animation
 	down - speed down animation
 	right - rotate positive y
 	left - rotate positive x
 	i - zoom in
 	o - zoom out
 	z - rotate negative x (objects behind the sun rise)
 	x - rotate positive x (objects infront of the sun fall)
 	r - pause/unpause
 	s - step mode
 	
 	To run:
 	$ make planets
 	$ ./planets
 
 */

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <math.h>
#include <Gl/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <tiffio.h>
#include "planets.h"
#include "tiff-util.h"


#define TextureWidth 512
#define TextureHeight 512

GLubyte textureImage[TextureHeight][TextureWidth][4];
GLuint textureName[10];
GLUquadricObj *sphere;

const float DEG2RAD = 3.14159/180;

static GLenum rotate = GL_TRUE;
static GLenum step = GL_FALSE;

static float hour1,hour2,hour3,hour4,hour5,hour6,hour7,hour8 = 0.0;
static float day1,day2,day3,day4,day5,day6,day7,day8 = 0.0;
static float hourMoon,hourGanymede,hourCallisto,hourIo,hourEuropa;
static float dayMoon,dayGanymede,dayCallisto,dayIo,dayEuropa;
static float animateInc = 24.0;
static float scale = 1.0/500000;
static float planetSize = 0.5;
static float planetDistance = 16.0;
static float zoom = -30;
static float viewX = 15.0;
static float viewY = 0.0;


// read the image file texture.tif (note the absence of error handling)
void createTextureImage(char name[]) {
	uint32 row, col;
	uint32 *rawImage;
	long i, j;
	
	rawImage = tiff_read(name, &row, &col);
	
	for(i=0;i<TextureWidth;i++) {
		for(j=0;j<TextureHeight;j++) {
			textureImage[i][j][0] = rawImage[i*col + j] & 0x000000FF;
			textureImage[i][j][1] = (rawImage[i*col + j] & 0x0000FF00) >> 8;
			textureImage[i][j][2] = (rawImage[i*col + j] & 0x00FF0000) >> 16;
			textureImage[i][j][3] = 255;
		}
	}
	free(rawImage);
}


// handles keypresses
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 'R':
		case 'r':
			keyR();
			break;
		case 's':
		case 'S':
			keyS();
			break;
		case 'i':
		case 'I':
			keyI();
			break;
		case 'o':
		case 'O':
			keyO();
			break;
		case 'z':
		case 'Z':
			keyZ();
			break;
		case 'x':
		case 'X':
			keyX();
			break;		
		case 'q':
		case 'Q':
		case 27: // escape
			exit(0);
	}
}


// handles special key presses
void specialKeyFunc( int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_UP:		
			keyUp();
			break;
		case GLUT_KEY_DOWN:
			keyDown();
			break;
		case GLUT_KEY_LEFT:
			keyLeft();
			break;
		case GLUT_KEY_RIGHT:
			keyRight();
			break;
	}
}


// handles o key
void keyO(void) {
	zoom *= 1.1;
}


// handles i key
void keyI(void) {
	zoom /= 1.1;
	if (zoom > -9.5) {
		zoom = -9.5;
	}
}


// handles z key
void keyZ(void) {
	viewX += 2.5;
}


// handles x key
void keyX(void) {
	viewX -= 2.5;
}


// handles left key
void keyLeft(void) {
	viewY -= 2.0;
}


// handles right key
void keyRight(void) {
	viewY += 2.0;
}


// handles r key
void keyR(void) {
	if ( step ) {
		step = GL_FALSE;
		rotate = GL_TRUE;
	}
	else {
		rotate = !rotate;
	}
}


// handles s key
void keyS(void) {
	step = GL_TRUE;
	rotate = GL_TRUE;
}


// handles up  key
void keyUp(void) {
	animateInc *= 2.0;
}


// handles down key
void keyDown(void) {
	animateInc /= 2.0;
}


// handles drawing ellipse
void drawEllipse(float x, float y) {
	glBegin(GL_LINE_LOOP);
	int i;
	float rad;
	for(i=0; i<360; i++) {
		rad = i*DEG2RAD;
		glVertex2f(cos(rad)*x,sin(rad)*y);
	}
	glEnd();
}


// handles display
void display(void) {

	// Set the model view
	glMatrixMode(GL_MODELVIEW);

	// Clear the redering window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (rotate) {
		// hour and day rotations for each planet
		
		// Mercury
		hour1 += animateInc;
		day1 += animateInc/24.0;
		hour1 = hour1 - ((int)(hour1/(24*58)))*(24*58);
		day1 = day1 - ((int)(day1/88))*88;
		
		// Venus
		hour2 += animateInc;
		day2 += animateInc/24.0;
		hour2 = hour2 - ((int)(hour2/(24*243)))*(24*243);
		day2 = day2 - ((int)(day2/224))*224;
		
		// Earth
		hour3 += animateInc;
		day3 += animateInc/24.0;
		hour3 = hour3 - ((int)(hour3/24))*24;
		day3 = day3 - ((int)(day3/365))*365;
		// Moon
		hourMoon += animateInc;
		dayMoon += animateInc/24.0;
		hourMoon = hourMoon - ((int)(hourMoon/(24*27.32)))*(24*27.32);
		dayMoon = dayMoon - ((int)(dayMoon/27.32))*27.32;
		
		// Mars
		hour4 += animateInc;
		day4 += animateInc/24.0;
		hour4 = hour4 - ((int)(hour4/24.6))*24.6;
		day4 = day4 - ((int)(day4/687))*687;
		
		// Jupiter
		hour5 += animateInc;
		day5 += animateInc/24.0;
		hour5 = hour5 - ((int)(hour5/9.8))*9.8;
		day5 = day5 - ((int)(day5/4329))*4329;
		// Jupiter's Moons
		hourGanymede += animateInc;
		dayGanymede += animateInc/24.0;
		hourGanymede += hourGanymede - ((int)(hourGanymede/(24.0*7.125)))*(24.0*7.125);
		dayGanymede += dayGanymede - ((int)(dayGanymede/7.125))*7.125;
		//
		hourCallisto += animateInc;
		dayCallisto += animateInc/24.0;
		hourCallisto += hourCallisto - ((int)(hourCallisto/(24.0*16.7)))*(24.0*16.7);
		dayCallisto += dayCallisto - ((int)(dayCallisto/16.7))*16.7;
		//
		hourIo += animateInc;
		dayIo += animateInc/24.0;
		hourIo += hourIo - ((int)(hourIo/(24.0*1.77)))*(24.*1.77);
		dayIo += dayIo - ((int)(dayIo/1.77))*1.77;
		//
		hourEuropa += animateInc;
		dayEuropa += animateInc/24.0;
		hourEuropa += hourEuropa - ((int)(hourEuropa/(24.0*3.5)))*(24.0*3.5);
		dayEuropa += dayEuropa - ((int)(dayEuropa/3.5))*3.5;
		
		// Saturn
		hour6 += animateInc;
		day6 += animateInc/24.0;
		hour6 = hour6 - ((int)(hour6/10.2))*10.2;
		day6 = day6 - ((int)(day6/10753))*10753;
		
		// Uranus
		hour7 += animateInc;
		day7 += animateInc/24.0;
		hour7 = hour7 - ((int)(hour7/17.9))*17.9;
		day7 = day7 - ((int)(day7/30685))*30685;
		
		// Neptune
		hour8 += animateInc;
		day8 += animateInc/24.0;
		hour8 = hour8 - ((int)(hour8/19.1))*19.1;
		day8 = day8 - ((int)(day8/60225))*60225;
	}
	
	// Lighting
	GLfloat position[] = {0.0, 0.0, 0.0, 1.0};
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	// set the texturing function
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glLoadIdentity();
	
	// Back off eight units to be able to view from the origin.
	glTranslatef(0.0, 0.0, zoom);
	
	// rotate about x-axis
	glRotatef(viewX, 1.0, 0.0, 0.0);
	glRotatef(viewY, 0.0, 1.0, 0.0);

	// Sun
	glColor3f(1.0, 1.0, 0.0);
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glRotatef(-90, 1.0, 0.0, 0.0);
	glCallList(textureName[0]);
	glPopMatrix();
	glEnable(GL_LIGHTING);
	glPushMatrix(); // save sun
	
	// Mercury
	glRotatef(360.0*day1/88.0, 0.0, 1.0, 0.0);
	glTranslatef(planetDistance*0.38, 0.0, 0.0);
	// Axis
	glRotatef(360.0*hour1/(24.0*58.0), 0.0, 1.0, 0.0);
	glColor3f(0.66, 0.33, 0.13);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glCallList(textureName[1]);
	glPopMatrix(); // restore sun
	glPushMatrix(); // save sun
	
	// Venus
	glRotatef(360.0*day2/224.0, 0.0, 1.0, 0.0);
	glTranslatef(planetDistance*0.72, 0.0, 0.0);
	// Axis
	glRotatef(360.0*hour2/(24.0*243.0), 0.0, 1.0, 0.0);
	glColor3f(0.0, 0.5, 0.0);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glCallList(textureName[2]);
	glPopMatrix(); // restore sun
	glPushMatrix(); // save sun
	
	// Earth
	glRotatef(360.0*day3/365.0, 0.0, 1.0, 0.0);
	glTranslatef(planetDistance, 0.0, 0.0);
	glPushMatrix(); // save earth
	// Axis
	glRotatef(360.0*hour3/24.0, 0.0, 1.0, 0.0);
	//glColor3f(0.2, 0.2, 1.0);
	glColor3f(1.0, 1.0, 1.0);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glCallList(textureName[3]);
	glPopMatrix(); // restore earth
	
	// Moon
	glRotatef(360.0*dayMoon/(27.32), 0.0, 1.0, 0.0);
	glTranslatef(1.5, 0.0, 0.0);
	// Axis
	glRotatef(360.0*hourMoon/(24.0*27.32), 0.0, 1.0, 0.0);
	glColor3f(1.0,1.0,1.0);
	glCallList(textureName[4]);
	glPopMatrix(); // restore sun
	glPushMatrix(); // save sun
	
	// Mars
	glRotatef(360.0*day4/687.0, 0.0, 1.0, 0.0);
	glTranslatef(planetDistance*1.52, 0.0, 0.0);
	// Axis
	glRotatef(360.0*hour4/24.6, 0.0, 1.0, 0.0);
	glColor3f(1.0, 0.0, 0.0 );
	glRotatef(-90, 1.0, 0.0, 0.0);
	glCallList(textureName[5]);
	glPopMatrix(); // restore sun
	glPushMatrix(); // save sun
	
	// Jupiter
	glRotatef(360.0*day5/4329.0, 0.0, 1.0, 0.0);
	glTranslatef(planetDistance*5.2, 0.0, 0.0 );
	glPushMatrix(); // save jupiter
	glPushMatrix(); // save jupiter
	glPushMatrix(); // save jupiter
	glPushMatrix(); // save jupiter
	// Axis
	glRotatef(360.0*hour5/9.8, 0.0, 1.0, 0.0);
	glColor3f(0.80, 0.5, 0.20);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glCallList(textureName[6]);
	glPopMatrix(); // restore jupiter
	// Jupiter's Moons
	// Ganymede
	glRotatef(360.0*dayGanymede/(7.125), 0.0, 1.0, 0.0);
	glTranslatef(13.0, 0.0, 0.0);
	// Axis
	glRotatef(360.0*hourGanymede/(24.0*7.125), 0.0, 1.0, 0.0);
	glColor3f(0.5,0.5,0.5);
	glutWireSphere(planetSize*0.418, 5, 5);
	glPopMatrix(); // restore jupiter
	// Callisto
	glRotatef(360.0*dayGanymede/(16.7), 0.0, 1.0, 0.0);
	glTranslatef(17.0, 0.0, 0.0);
	// Axis
	glRotatef(360.0*hourGanymede/(24.0*16.7), 0.0, 1.0, 0.0);
	glColor3f(0.54,0.27,0.07);
	glutWireSphere(planetSize*0.383, 5, 5);
	glPopMatrix(); // restore jupiter
	// Io
	glRotatef(360.0*dayGanymede/(1.77), 0.0, 1.0, 0.0);
	glTranslatef(8.0, 0.0, 0.0);
	// Axis
	glRotatef(360.0*hourGanymede/(24.*1.77), 0.0, 1.0, 0.0);
	glColor3f(1.0,0.82,0.0);
	glutWireSphere(planetSize*0.290, 5, 5);
	glPopMatrix(); // restore jupiter
	// Europa	
	glRotatef(360.0*dayGanymede/(3.5), 0.0, 1.0, 0.0);
	glTranslatef(10.0, 0.0, 0.0);
	// Axis
	glRotatef(360.0*hourGanymede/(24.0*3.5), 0.0, 1.0, 0.0);
	glColor3f(0.8,0.52,0.25);
	glutWireSphere(planetSize*0.248, 5, 5);
	glPopMatrix(); // restore sun
	glPushMatrix(); // save the sun
	
	 // Saturn
	glRotatef(360.0*day6/10753.0, 0.0, 1.0, 0.0);
	glTranslatef(planetDistance*9.54, 0.0, 0.0);
	glPushMatrix(); // save saturn
	// Axis
	glRotatef(360.0*hour6/10.2, 0.0, 1.0, 0.0);
	glColor3f(0.86, 0.86, 0.44);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glCallList(textureName[7]);
	glPopMatrix(); // restore saturn
	// Rings
	glColor3f(1.0,1.0,1.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	drawEllipse(planetSize*10.5, planetSize*14.0);
	drawEllipse(planetSize*10.5, planetSize*14.5);
	drawEllipse(planetSize*10.5, planetSize*13.5);
	glPopMatrix(); // restore sun
	glPushMatrix(); // save the sun
	
	 // Uranus
	glRotatef(360.0*day7/30685.0, 0.0, 1.0, 0.0);
	glTranslatef(planetDistance*19.22, 0.0, 0.0);
	// Axis
	glRotatef(360.0*hour7/17.9, 0.0, 1.0, 0.0);
	glColor3f(0.53, 0.81, 0.95);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glCallList(textureName[8]);
	glPopMatrix(); // restore sun
	glPushMatrix(); // save the sun
	
	// Neptune
	glRotatef(360.0*day8/60225.0, 0.0, 1.0, 0.0);
	glTranslatef(planetDistance*30.06, 0.0, 0.0);
	// Axis
	glRotatef(360.0*hour8/19.1, 0.0, 1.0, 0.0);
	glColor3f(.48, 0.41, 0.93);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glCallList(textureName[9]);
	glPopMatrix(); // restore sun
	
	// draw everything and switch buffers
	glFlush();
	glutSwapBuffers();
	
	if (step) {
		rotate = GL_FALSE;
	}
	
	glutPostRedisplay();
}


void initLights(void) {
	GLfloat ambient[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat diffuse[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat mat_diffuse[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat mat_shininess[] = {100.0};
	
	// material values
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	
	// generic lighting values
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
	
	// specific light source
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	
	// enable lighting, light0 and depth testing
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
}
 

void initTexture(void) {
	// set the clear color
	glClearColor(0.0, 0.0, 0.0, 0.0);
	
	// set the shade model to smooth
	glShadeModel(GL_SMOOTH);
	
	// turn on the z-buffer
	glEnable(GL_DEPTH_TEST);
	
	// enable texturing
	glEnable(GL_TEXTURE_2D);
	
	// read in the texture data
	char zero[] = "sunTexture.tif";
	char one[] = "mercuryTexture.tif";
	char two[] = "venusTexture.tif";
	char three[] = "earthTexture.tif";
	char four[] = "moonTexture.tif";
	char five[] = "marsTexture.tif";
	char six[] = "jupiterTexture.tif";
	char seven[] = "saturnTexture.tif";
	char eight[] = "uranusTexture.tif";
	char nine[] = "neptuneTexture.tif";
	
	// Call this because we haven't packed things
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	// Generate the appropriate number of textures
	glGenTextures(10, textureName);
	
	// Setup the textures
	setupTexture(0,zero);
	setupTexture(1,one);
	setupTexture(2,two);
	setupTexture(3,three);
	setupTexture(4,four);
	setupTexture(5,five);
	setupTexture(6,six);
	setupTexture(7,seven);
	setupTexture(8,eight);
	setupTexture(9,nine);
	
	// Set up the sphere
	sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricTexture(sphere, GL_TRUE);
	gluQuadricNormals(sphere, GLU_SMOOTH);
	gluQuadricOrientation(sphere, GLU_OUTSIDE);

	//Making a display list (a module)
	glNewList(textureName[0], GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, textureName[0]);
	gluSphere(sphere, 3.0, 20, 20);
	glEndList();
	glNewList(textureName[1], GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, textureName[1]);
	gluSphere(sphere, planetSize*0.383, 20, 20);
	glEndList();
	glNewList(textureName[2], GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, textureName[2]);
	gluSphere(sphere, planetSize*0.949, 20, 20);
	glEndList();
	glNewList(textureName[3], GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, textureName[3]);
	gluSphere(sphere, planetSize*1.0, 20, 20);
	glEndList();
	glNewList(textureName[4], GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, textureName[4]);
	gluSphere(sphere, planetSize*0.272, 20, 20);
	glEndList();
	glNewList(textureName[5], GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, textureName[5]);
	gluSphere(sphere, planetSize*0.533, 20, 20);
	glEndList();
	glNewList(textureName[6], GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, textureName[6]);
	gluSphere(sphere, planetSize*11.21, 20, 20);
	glEndList();
	glNewList(textureName[7], GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, textureName[7]);
	gluSphere(sphere, planetSize*9.55, 20, 20);
	glEndList();
	glEndList();
	glNewList(textureName[8], GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, textureName[8]);
	gluSphere(sphere, planetSize*4.07, 20, 20);
	glEndList();
	glEndList();
	glNewList(textureName[9], GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, textureName[9]);
	gluSphere(sphere, planetSize*3.94, 20, 20);
	glEndList();
	gluDeleteQuadric(sphere);
}


void setupTexture(int index, char name[]) {
	glBindTexture(GL_TEXTURE_2D, textureName[index]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	createTextureImage(name);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TextureWidth, TextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureImage);
}


// initializes opengl
void init(void) {
	// set the shade model to smooth
    glShadeModel(GL_SMOOTH);
    
    // clear the clear color
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0);
    
    // turn on the z-buffer
    glEnable(GL_DEPTH_TEST);
    
    initLights();
	initTexture();
}


// handles resizing the window
void reshape(int w, int h) {
	float aspect;
	
	if (h==0) {
		h = 1;
	}
	if (w==0) {
		w = 1;
	}
	
	glViewport(0, 0, w, h);
	aspect = (float)w/(float)h;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective((GLdouble)60.0, (GLdouble)aspect, (GLdouble)1.0, (GLdouble)2000.0);
	
	glMatrixMode(GL_MODELVIEW);
}


// main function
int main( int argc, char **argv ) {
	// initialize
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Planets");
	init();
	
	// set up callback functions
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyFunc);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	
	// main loop
	glutMainLoop();
	
	return(0);
}
