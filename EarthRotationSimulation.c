// EarthRotationSimulation

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "GL/glut.h"

#define Pi 3.141592654

typedef struct POINT{
	GLfloat x;
	GLfloat y;
} Point;

void drawingCB(void);
void reshapeCB(int width, int height);
void timerCB(int value);
void keyboardCB(unsigned char key, int x, int y);
void specialCB(int key, int x, int y);

GLuint generateTextures(char *imagepath);
GLubyte *readBMP(char *imagepath, int *width, int *height);
void TerminationErrorFunc(char *ErrorString);

Point earthCoords, moonCoords;
int cnt, full, refresh;

GLuint earthTexID;
GLuint moonTexID;
GLuint sunTexID;

GLfloat light_diffuse[] = {1.5, 1.5, 1.5, 1.0};
GLfloat light_position[] = {0.0, 0.0, 0.0, 0.0};
GLfloat light_emission[] = {1.0, 1.0, 1.0, 0.0};

int main(int argc, char *argv[])
{
	//initializing points and circle array
	earthCoords.x = 10.0f;
	earthCoords.y = 0.0f;
	moonCoords.x = 3.0f;
	moonCoords.y = 0.0f;

	cnt = 0;
	full = 8760;
	refresh = 40;

	//initizlizing GLUT
	glutInit(&argc,argv);

	//initializing window
	glutInitWindowPosition(100,100);
	glutInitWindowSize(600,400);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("Solar System");

	glEnable(GL_DEPTH_TEST);

	//registering callbacks
	glutDisplayFunc(drawingCB);
	glutReshapeFunc(reshapeCB);

	//Adding timer handler
	glutTimerFunc(refresh, timerCB, 0);

	//Adding keyboard keys handler
	glutKeyboardFunc(keyboardCB);
	glutSpecialFunc(specialCB);

	//setting Global texture parameters
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); //determine how texture
	//mapping interacts with shading modes. Can be: GL_MODULATE or GL_BLEND or GL_REPLACE
	glEnable(GL_TEXTURE_2D);
	//loading textures
	earthTexID = generateTextures("earth.bmp");
	moonTexID = generateTextures("moon.bmp");
	sunTexID = generateTextures("sun.bmp");

	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	//starting main loop
	glutMainLoop(); 
}

void drawSun()
{
	GLUquadricObj* pObj;

	glPushMatrix();

	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, light_emission);

	//drawing sun
	glBindTexture(GL_TEXTURE_2D, sunTexID);
	pObj = gluNewQuadric(); //creates new quadric for pObj
	gluQuadricTexture(pObj, GL_TRUE);
	gluSphere(pObj, 2.0, 50, 50); //draws the sphere for the earth
	gluDeleteQuadric(pObj); //frees pObj

	glPopMatrix();
}

void drawEarth()
{
	GLUquadricObj* pObj;

	glRotatef(((GLfloat)cnt/full)*360.0, 0.0f, 0.0f, 1.0f);
	glTranslatef(earthCoords.x, earthCoords.y, 0.0f);

	glPushMatrix();

	glRotatef(23.5f, 0.0f, 1.0f, 0.0f);
	glRotatef(((cnt%365)/24.0)*360.0, 0.0f, 0.0f, 1.0f);

	//drawing earth
	glBindTexture(GL_TEXTURE_2D, earthTexID);
	pObj = gluNewQuadric(); //creates new quadric for pObj
	gluQuadricTexture(pObj, GL_TRUE);
	gluSphere(pObj, 1, 50, 50); //draws the sphere for the earth
	gluDeleteQuadric(pObj); //frees pObj

	glPopMatrix();
}

void drawMoon()
{
	GLUquadricObj* pObj;

	glRotatef(((cnt%730)/730.0)*360.0, 0.0f, 0.0f, 1.0f);
	glTranslatef(moonCoords.x, moonCoords.y, 0.0f);

	//drawing moon
	glBindTexture(GL_TEXTURE_2D, moonTexID);
	pObj = gluNewQuadric(); //creates new quadric for pObj
	gluQuadricTexture(pObj, GL_TRUE);
	gluSphere(pObj, 0.5, 50, 50); //draws the sphere for the earth
	gluDeleteQuadric(pObj); //frees pObj
}

// rendering callback
void drawingCB()
{
	GLenum er;
	GLfloat ModelviewMatrix[16];
	GLfloat ProjectionMatrix[16];

	//clearing the background
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//initializing modelview transformation matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(-10.0f, -10.0f, 20.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 1.0f);


	glPushMatrix();
	drawSun();
	drawEarth();
	drawMoon();
	glPopMatrix();



	//checking transformation matrixes for debuging
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelviewMatrix); 
	glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrix); 

	//forces execution of OpenGL functions in finite time
	//glFlush();
	glutSwapBuffers();

	//check for errors
	er = glGetError();  //get errors. 0 for no error, find the error codes in: https://www.opengl.org/wiki/OpenGL_Error
	if(er) printf("error: %d\n",er);
}

void reshapeCB(int width, int height)
{
	float AR;
	GLfloat ModelviewMatrix[16];
	GLfloat ProjectionMatrix[16];

	//update viewport
	glViewport(0,0,width,height);

	//clear the transformation matrices (load identity)
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//compute the aspect ratio
	AR = (float)width/height;

	//Perspective view
	gluPerspective(40.0, AR, 1.0, 50.0);

	//checking transformation matrixes for debuging
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelviewMatrix); 
	glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrix); 
}

void timerCB(int value)
{
	// Update parameters
	cnt = (cnt+1)%8760;

	glutPostRedisplay();

	glutTimerFunc(refresh, timerCB, 0);
}

void keyboardCB(unsigned char key, int x, int y)
{
	//printf("%c", key);
}

void specialCB(int key, int x, int y)
{
	//printf("%d", key);
	switch(key)
	{
	case 102:
		if(refresh >= 15)
			refresh-=5;
		break;
	case 100:
		if(refresh <= 120)
			refresh+=5;
		break;
	}
	glutPostRedisplay();
}

GLuint generateTextures(char *imagepath)
{
	GLubyte *ImageData;
	int width, height;
	GLuint TextureID;
	//reading the image_
	ImageData = readBMP(imagepath, &width, &height);
	//Giving a texture ID_
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	//Assign Image as a texture
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, ImageData);
	//freeing image memory
	free(ImageData);
	//setting Specific texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //GL_REPEAT or GL_CLAMP
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// GL_LINEAR or GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return TextureID;
}
// Function to load bmp file
// buffer for the image is allocated in this function, you should free this buffer
GLubyte *readBMP(char *imagepath, int *width, int *height)
{
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos; // Position in the file where theactual data begins
	unsigned int imageSize; // = width*height*3
	unsigned char * data;
	unsigned char tmp;
	int i;
	// Open the file
	FILE * file = fopen(imagepath, "rb");

	if (!file){
		TerminationErrorFunc("Image could not be opened\n");
	}
	if (fread(header, 1, 54, file) != 54){ // If not 54 bytes read :problem
		TerminationErrorFunc("Not a correct BMP file\n");
	}
	if (header[0] != 'B' || header[1] != 'M'){
		TerminationErrorFunc("Not a correct BMP file\n");
	}
	// Read ints from the byte array
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	*width = *(int*)&(header[0x12]);
	*height = *(int*)&(header[0x16]);
	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)
		imageSize = *width**height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)
		dataPos = 54; // The BMP header is done that way
	// Create a buffer
	data = malloc(imageSize * sizeof(GLubyte));
	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);
	//swap the r and b values to get RGB (bitmap is BGR)
	for (i = 0; i<*width**height; i++)
	{
		tmp = data[i * 3];
		data[i * 3] = data[i * 3 + 2];
		data[i * 3 + 2] = tmp;
	}
	//Everything is in memory now, the file can be closed
	fclose(file);
	return data;
}

void TerminationErrorFunc(char *ErrorString)
{
	char string [256];
	printf(ErrorString);
	fgets(string,256,stdin);     // warning: unsafe (see fgets instead)

	exit(0);
}