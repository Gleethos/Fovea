//
// Lighthouse3D.com GLSL Core Tutorial - Color Example
//
//
// This demo was built for learning purposes only.
// Some code could be severely optimised, but I tried to
// keep as simple and clear as possible.
//
// The code comes with no warranties, use it at your own risk.
// You may use it, or parts of it, wherever you want.
//
// If you do use it I would love to hear about it. Just post a comment
// at Lighthouse3D.com

// Have Fun :-)


#include <math.h>
#include <string>

#include "shaderDemo.h"

// include GLEW to access OpenGL 3.3 functions
#include <GLEW/glew.h>

// GLUT is the toolkit to interface with the OS
#include <FREEGLUT/freeglut.h>
//#include <GL/glut.h>

// Use Very Simple Libs
#include "VSMathlib.h"
#include "VSShaderlib.h"

VSMathLib *vsml;
VSShaderLib polarization_shader; // Vertecies to polar -=> fragments to polarTexture
								 // VERTEX SHADER STEP -->  FRAGMENT SHADER STEP
VSShaderLib to_cartesian_shader; // Inputs to outputs  -=> polarTexture pixels to cartesian

// Camera Position
float camX, camY, camZ;

// Mouse Tracking Variables
int startX, startY, tracking = 0;

// Camera Spherical Coordinates
float alpha = -4, beta = 5;
float r = 5.25f;

// Frame counting and FPS computation
long myTime,timebase = 0,frame = 0;
char s[32];

GLuint vao;

struct Params {
	Params(float fx_, float fy_, float base_, float r_) : fx(fx_), fy(fy_), base(base_), r(r_) {}

	float fx;
	float fy;
	float base;
	float r;
};

int width = 1024;
int height = 512;

int focalPointX = width / 2;
int focalPointY = height / 2;

bool finishedInitializing = false;

// radius of central area
int radius = 300;

GLuint polarBufferID = 0;
int polarTextureID = 0;

// ------------------------------------------------------------
//
// utility functions
//

void sendFoveaParamsToShader(VSShaderLib shader, int focalPointX, int focalPointY, int radius, int base = 2) {
	Params params = Params(focalPointX, focalPointY, base, radius);
	shader.setBlock("Params", &params);
	printf("shader params changed: (fx: %f, fy: %f, base: %f, r: %f)\n", params.fx, params.fy, params.base, params.r);
}

// ------------------------------------------------------------
//
// Framebuffer methods
//

int createFramebuffer() {
	GLuint FramebufferName = 0;
	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

	return FramebufferName;
}

void bindFrameBuffer(int frameBuffer, int width, int height) {
	if (frameBuffer != 0)
		glBindTexture(GL_TEXTURE_2D, 0); // to make sure the texture is not bound

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glViewport(0, 0, width, height);
}

int createTextureAttachment(int width, int height) {
	GLuint texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);

	return texture;
}

void initializeFrameBuffer() {
	//polarBufferID = createFramebuffer();
	//polarTextureID = createTextureAttachment(width, height);

	// set up floating point framebuffer to render scene to
	
	glGenFramebuffers(1, &polarBufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, polarBufferID);
	unsigned int colorBuffers[1];
	glGenTextures(1, colorBuffers);
	for (unsigned int i = 0; i < 1; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA16F, width, width, 0, GL_RGBA, GL_FLOAT, NULL
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to framebuffer
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0
		);
	}

	unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachments);

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		printf("framebuffer complete\n");

	polarTextureID = colorBuffers[0];

	polarization_shader.setUniform("polBufferTexture", &polarTextureID); // set id of texture to use it as sampler2D in renderer

	printf("our buffer id: %d\n", polarBufferID);
	printf("our texture id: %d\n", polarTextureID);

	//bindFrameBuffer(0, width, height); // binds back to screen
}

// ------------------------------------------------------------
//
// Reshape Callback Function
//

void changeSize(int w, int h) {

	width = w;
	height = h;

	focalPointX = width / 2;
	focalPointY = height / 2;

	// printf("resize: new width, height = %f, %f\n", float(width), float(height));

	float ratio;
	// Prevent a divide by zero, when window is too short
	if(h == 0)
		h = 1;
	// set the viewport to be the entire window
	glViewport(0, 0, w, h);
	// set the projection matrix
	ratio = (1.0f * w) / h;
	vsml->loadIdentity(VSMathLib::PROJECTION);
	vsml->perspective(53.13f, ratio, 0.1f, 1000.0f);
}


// ------------------------------------------------------------
// Nepp & Hinterleitner - Windows - Foveated Rendering (log polar transformation shaders...)
// Render stuff
//

void basicRendering() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// load identity matrices
	vsml->loadIdentity(VSMathLib::VIEW);
	vsml->loadIdentity(VSMathLib::MODEL);
	// set the camera using a function similar to gluLookAt
	vsml->lookAt(camX, camY, camZ, 0, 0, 0, 0, 1, 0);
	// use our shader
	glUseProgram(polarization_shader.getProgramIndex());
	// send matrices to uniform buffer
	vsml->matricesToGL();
	// render VAO
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, faceCount * 3, GL_UNSIGNED_INT, 0);
	//swap buffers
	glutSwapBuffers();
}

void renderScene(void) {
	
	//	---------------------------------
	//	binding to our buffer section
	
	polarization_shader.setUniform("is_rendering_to_polar_texture", 1);
	bindFrameBuffer(polarBufferID, width, height);

	basicRendering();

	//	---------------------------------
	//	binding to screen buffer (output)

	polarization_shader.setUniform("is_rendering_to_polar_texture", 0);
	bindFrameBuffer(0, width, height);

	basicRendering();
}


// ------------------------------------------------------------
//
// Events from the Keyboard
//

void processKeys(unsigned char key, int xx, int yy)
{
	switch(key) {

		case 27: // esc key
			glutLeaveMainLoop();
			break;

		case 43: // + key
			radius += 2;
			sendFoveaParamsToShader(polarization_shader, focalPointX, focalPointY, radius);
			break;

		case 45: // - key
			radius -= 2;
			sendFoveaParamsToShader(polarization_shader, focalPointX, focalPointY, radius);
			break;

		case 'c': 
			printf("Camera Spherical Coordinates (%f, %f, %f)\n", alpha, beta, r);
			break;

	}

//  uncomment this if not using an idle func
//	glutPostRedisplay();
}

static void specialKeyPressed(int key, int x, int y)
{
	switch (key) {
		case GLUT_KEY_UP:
			focalPointY += 5;
			sendFoveaParamsToShader(polarization_shader, focalPointX, focalPointY, radius);
			break;

		case GLUT_KEY_DOWN:
			focalPointY -= 5;
			sendFoveaParamsToShader(polarization_shader, focalPointX, focalPointY, radius);
			break;

		case GLUT_KEY_LEFT:
			focalPointX -= 5;
			sendFoveaParamsToShader(polarization_shader, focalPointX, focalPointY, radius);
			break;

		case GLUT_KEY_RIGHT:
			focalPointX += 5;
			sendFoveaParamsToShader(polarization_shader, focalPointX, focalPointY, radius);
			break;
	}
}


// ------------------------------------------------------------
//
// Mouse Events
//

void processMouseButtons(int button, int state, int xx, int yy)
{
	// start tracking the mouse
	if (state == GLUT_DOWN)  {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
	}

	//stop tracking the mouse
	else if (state == GLUT_UP) {
		if (tracking == 1) {
			alpha -= (xx - startX);
			beta += (yy - startY);
		}
		else if (tracking == 2) {
			r += (yy - startY) * 0.01f;
			if (r < 0.1f)
				r = 0.1f;
		}
		tracking = 0;
	}
}

// Track mouse motion while buttons are pressed

void processMouseMotion(int xx, int yy)
{

	int deltaX, deltaY;
	float alphaAux, betaAux;
	float rAux;

	deltaX =  - xx + startX;
	deltaY =    yy - startY;

	// left mouse button: move camera
	if (tracking == 1) {


		alphaAux = alpha + deltaX;
		betaAux = beta + deltaY;

		if (betaAux > 85.0f)
			betaAux = 85.0f;
		else if (betaAux < -85.0f)
			betaAux = -85.0f;
		rAux = r;
	}
	// right mouse button: zoom
	else if (tracking == 2) {

		alphaAux = alpha;
		betaAux = beta;
		rAux = r + (deltaY * 0.01f);
		if (rAux < 0.1f)
			rAux = 0.1f;
	}

	camX = rAux * sin(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
	camZ = rAux * cos(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
	camY = rAux *   						       sin(betaAux * 3.14f / 180.0f);

//  uncomment this if not using an idle func
//	glutPostRedisplay();
}


void mouseWheel(int wheel, int direction, int x, int y) {

	r += direction * 0.1f;
	if (r < 0.1f)
		r = 0.1f;

	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r *   						     sin(beta * 3.14f / 180.0f);

//  uncomment this if not using an idle func
//	glutPostRedisplay();
}


// --------------------------------------------------------
//
// Shader Stuff
//

GLuint setupShaders() {

	// Shader for models
	polarization_shader.init();
	polarization_shader.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/polarization_shader.vert");
	
	// set semantics for the shader variables
	polarization_shader.setProgramOutput(0, "outputFragment");
	polarization_shader.setVertexAttribName(VSShaderLib::VERTEX_COORD_ATTRIB, "position");
	
	polarization_shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/polarization_shader.frag");

	// our shader
	// ------------------------------------------------------------------------------

	// vertex polarization shader
	//shader.setVertexAttribName(VSShaderLib::VERTEX_COORD_ATTRIB, "v");
	//shader.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/vertex_polarization.txt");


	Params params = Params(focalPointX, focalPointY, 2, radius);
	//Params params = Params(1.0f, 1.0f, 0.0f, 1.0f); // only to check if uniform blocks are working in our programm

	printf("shader params: (fx: %f, fy: %f, base: %f, r: %f)\n", params.fx, params.fy, params.base, params.r);

	polarization_shader.prepareProgram();

	polarization_shader.setBlock("Params", &params);

	printf("InfoLog for Shaders\n%s\n\n", polarization_shader.getAllInfoLogs().c_str());
	
	return(polarization_shader.isProgramValid());
}


// ------------------------------------------------------------
//
// Model loading and OpenGL setup
//

void initOpenGL()
{
	// set the camera position based on its spherical coordinates
	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r *   						     sin(beta * 3.14f / 180.0f);

	// some GL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// create the VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// create buffers for our vertex data
	GLuint buffers[4];
	glGenBuffers(4, buffers);

	//vertex coordinates buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(VSShaderLib::VERTEX_COORD_ATTRIB);
	glVertexAttribPointer(VSShaderLib::VERTEX_COORD_ATTRIB, 4, GL_FLOAT, 0, 0, 0);

	//texture coordinates buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(VSShaderLib::TEXTURE_COORD_ATTRIB);
	glVertexAttribPointer(VSShaderLib::TEXTURE_COORD_ATTRIB, 2, GL_FLOAT, 0, 0, 0);

	//normals buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(VSShaderLib::NORMAL_ATTRIB);
	glVertexAttribPointer(VSShaderLib::NORMAL_ATTRIB, 3, GL_FLOAT, 0, 0, 0);

	//index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(faceIndex), faceIndex, GL_STATIC_DRAW);

	// initialize polar buffer
	initializeFrameBuffer();

	// unbind the VAO
	glBindVertexArray(0);
}

// ------------------------------------------------------------
//
// Initialize VS Math Library
//

void initVSL() {
	vsml = VSMathLib::getInstance();
	// tell VSL the uniform block name
	vsml->setUniformBlockName("Matrices");
	// set semantics for the matrix variable
	vsml->setUniformName(VSMathLib::PROJ_VIEW_MODEL, "pvm");
}

// ------------------------------------------------------------
//
// Timer function for fixed framerate
//

void timer(int value) {
	glutPostRedisplay();
	glutTimerFunc(1000 / 60.0, &timer, 1);
}

// ------------------------------------------------------------
//
// Main function
//


int main(int argc, char **argv) {

//  GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA|GLUT_MULTISAMPLE);

	glutInitContextVersion (3, 3);
	glutInitContextProfile (GLUT_CORE_PROFILE );
	glutInitContextFlags(GLUT_DEBUG);

	glutInitWindowPosition(150, 150);
	glutInitWindowSize(width, height);
	glutCreateWindow("Nepp & Hinterleitner - Windows - Foveated Rendering");

//  Callback Registration
	glutDisplayFunc(renderScene);
	glutReshapeFunc(&changeSize);

	glutTimerFunc(1000 / 60.0, &timer, 1); //	use timer function instead of renderScene as idle function

//	Mouse and Keyboard Callbacks
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(specialKeyPressed);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);

	glutMouseWheelFunc(mouseWheel);

//	return from main loop
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

//	Init GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	printf ("Vendor: %s\n", glGetString (GL_VENDOR));
	printf ("Renderer: %s\n", glGetString (GL_RENDERER));
	printf ("Version: %s\n", glGetString (GL_VERSION));
	printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));

	if (!setupShaders())
		return(1);

	initOpenGL();

	initVSL();

	//glutFullScreen();

	finishedInitializing = true;

	//  GLUT main loop
	glutMainLoop();

	return(0);

}

