#ifdef __APPLE__
#include <GLUT/glut.h>
#endif
#ifdef __unix__
#include <GL/glut.h>
#endif
#include <cstdlib>
#include <iostream>
#include <vector>
#define GLM_FORCE_RADIANS
#include "glm/gtx/transform.hpp"
#include "GLSL.h"
#include "Camera.h"
#include "Image.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "stdio.h"
#include "Ramp.h"
#include "GuardRail.h"

#define FLOOR_SIZE 150.0f
#define FLOOR_HEIGHT -2.01f
#define TRACK_RADIUS 100.0f
#define TRACK_WIDTH 20.0f



using namespace std;
glm::vec2 mouseCoords;
bool keyDown[256] = {false};
Camera camera;

// Vertex buffer data. This data is populated on the CPU and sent to the GPU.
vector<float> positionBuf;
vector<float> trackBuf;
vector<unsigned int> trackIndexBuf;

vector<float> norBuf;
vector<unsigned int> indexBuf;
vector<float> texCoordsBuf;
vector<float> trackTexCoordsBuf;

// Vertex buffer object IDs. These are used by OpenGL to reference the buffers.
GLuint positionBufID;
GLuint indexBufID;
GLuint norBufID;
GLuint texCoordsBufID;
GLuint trackBufID;
GLuint trackIndexBufID;
GLuint trackTexCoordsBufID;

// Model matrix for the plane
glm::mat4 M;

// GLSL program
GLuint pid;

GLuint texture0ID;
GLuint texture1ID;
GLuint texture2ID;
GLuint texture3ID;
GLuint texture4ID;


// GLSL handles to various parameters in the shaders
GLint h_vertexPosition;
GLint h_P;
GLint h_MV;

GLint h_vertTexCoords;

GLint h_vertNor;
GLint h_texture0;
GLint h_lightPosCam;
GLint h_s;
GLint h_ka;
GLint h_ks;
int time0 = 0;
float heightFunc(float x, float y){

	// trackBuf.push_back(cos(degInRad)*radius);
	// trackBuf.push_back(3 * cos(i / 10.0f) - 2.0f);
	// trackBuf.push_back(-1 * sin(degInRad)*radius);

	// trackBuf.push_back(cos(degInRad)*(radius + width));
	// trackBuf.push_back(3 * cos(i / 10.0f) - 2.0f);
	// trackBuf.push_back(-1 * sin(degInRad)*(radius + width));

	return -2.0f;
}

void loadScene()
{
	// time
	camera.loadCar();
	Ramp::load();
	GuardRail::load();

	// Populate the world here
	positionBuf.push_back(-1 * FLOOR_SIZE);
	positionBuf.push_back(FLOOR_HEIGHT);
	positionBuf.push_back(FLOOR_SIZE);
	norBuf.push_back(0.0f);
	norBuf.push_back(-1.0f);
	norBuf.push_back(0.0f);
	texCoordsBuf.push_back(0.0f);
	texCoordsBuf.push_back(0.0f);

	positionBuf.push_back(FLOOR_SIZE);
	positionBuf.push_back(FLOOR_HEIGHT);
	positionBuf.push_back(FLOOR_SIZE);
	norBuf.push_back(0.0f);
	norBuf.push_back(-1.0f);
	norBuf.push_back(0.0f);
	texCoordsBuf.push_back(1.0f);
	texCoordsBuf.push_back(0.0f);

	positionBuf.push_back(-1 * FLOOR_SIZE);
	positionBuf.push_back(FLOOR_HEIGHT);
	positionBuf.push_back(-1 * FLOOR_SIZE);
	norBuf.push_back(0.0f);
	norBuf.push_back(-1.0f);
	norBuf.push_back(0.0f);
	texCoordsBuf.push_back(0.0f);
	texCoordsBuf.push_back(1.0f);

	positionBuf.push_back(FLOOR_SIZE);
	positionBuf.push_back(FLOOR_HEIGHT);
	positionBuf.push_back(-1 * FLOOR_SIZE);
	norBuf.push_back(0.0f);
	norBuf.push_back(-1.0f);
	norBuf.push_back(0.0f);
	texCoordsBuf.push_back(1.0f);
	texCoordsBuf.push_back(1.0f);

	indexBuf.push_back(0);
	indexBuf.push_back(1);
	indexBuf.push_back(2);
	indexBuf.push_back(3);
	indexBuf.push_back(2);
	indexBuf.push_back(1);

	//track
	float DEG2RAD = (atan(1.0f) * 4)/180;
	float radius = TRACK_RADIUS;
	float width = TRACK_WIDTH;
	for(int i = 0; i < 360; ++i){
		float degInRad = i*DEG2RAD;

		trackBuf.push_back(cos(degInRad)*radius);
		trackBuf.push_back(heightFunc(cos(degInRad)*radius, sin(degInRad)*radius));
		trackBuf.push_back(-1 * sin(degInRad)*radius);

		trackBuf.push_back(cos(degInRad)*(radius + width));
		trackBuf.push_back(heightFunc(cos(degInRad)*radius, sin(degInRad)*radius));
		trackBuf.push_back(-1 * sin(degInRad)*(radius + width));

		trackIndexBuf.push_back(2 * i);
		trackIndexBuf.push_back(2 * i + 1);
		trackIndexBuf.push_back((2 * i + 2) % 720);

		trackTexCoordsBuf.push_back(0.0f);
		trackTexCoordsBuf.push_back(0.0f);

		trackTexCoordsBuf.push_back(1.0f);
		trackTexCoordsBuf.push_back(0.0f);

		trackIndexBuf.push_back((2 * i + 2) % 720);
		trackIndexBuf.push_back(2 * i + 1);
		trackIndexBuf.push_back((2 * i + 3) % 720);
	}

	M = glm::translate(glm::vec3(0.0f, 0.0f, -5.0f));
}

void initGL()
{
	//////////////////////////////////////////////////////
	// Initialize GL for the whole scene
	//////////////////////////////////////////////////////
	
	// Set background color
	glClearColor(0.69f, 0.88f, 1.0f, 1.0f);
	// Enable z-buffer test
	glEnable(GL_DEPTH_TEST);

	//shapes
	Ramp::init();
	Ramp::populate(10, TRACK_RADIUS, TRACK_WIDTH);
	GuardRail::init();
	GuardRail::populate(TRACK_RADIUS, TRACK_WIDTH);

	camera.initCar();
	
	//////////////////////////////////////////////////////
	// Intialize the shaders
	//////////////////////////////////////////////////////
	
	// Create shader handles
	string vShaderName = "object_vert.glsl";
	string fShaderName = "object_frag.glsl";
	GLint rc;
	GLuint VS = glCreateShader(GL_VERTEX_SHADER);
	GLuint FS = glCreateShader(GL_FRAGMENT_SHADER);
	
	// Read shader sources
	const char *vshader = GLSL::textFileRead(vShaderName.c_str());
	const char *fshader = GLSL::textFileRead(fShaderName.c_str());
	glShaderSource(VS, 1, &vshader, NULL);
	glShaderSource(FS, 1, &fshader, NULL);
	
	// Compile vertex shader
	glCompileShader(VS);
	GLSL::printError();
	glGetShaderiv(VS, GL_COMPILE_STATUS, &rc);
	GLSL::printShaderInfoLog(VS);
	if(!rc) {
		printf("Error compiling vertex shader %s\n", vShaderName.c_str());
	}
	
	// Compile fragment shader
	glCompileShader(FS);
	GLSL::printError();
	glGetShaderiv(FS, GL_COMPILE_STATUS, &rc);
	GLSL::printShaderInfoLog(FS);
	if(!rc) {
		printf("Error compiling fragment shader %s\n", fShaderName.c_str());
	}
	
	// Create the program and link
	pid = glCreateProgram();
	glAttachShader(pid, VS);
	glAttachShader(pid, FS);
	glLinkProgram(pid);
	GLSL::printError();
	glGetProgramiv(pid, GL_LINK_STATUS, &rc);
	GLSL::printProgramInfoLog(pid);
	if(!rc) {
		printf("Error linking shaders %s and %s\n", vShaderName.c_str(), fShaderName.c_str());
	}
	h_vertNor = GLSL::getAttribLocation(pid, "vertNormal");
	h_vertexPosition = GLSL::getAttribLocation(pid, "vertexPosition");
	h_P = GLSL::getUniformLocation(pid, "P");
	h_MV = GLSL::getUniformLocation(pid, "MV");
	h_texture0 = GLSL::getUniformLocation(pid, "texture0");
	h_vertTexCoords = GLSL::getAttribLocation(pid, "vertTexCoords");

	h_lightPosCam = GLSL::getUniformLocation(pid, "lightPosCam");
	h_s = GLSL::getUniformLocation(pid, "s");
	h_ka = GLSL::getUniformLocation(pid, "ka");
	h_ks = GLSL::getUniformLocation(pid, "ks");

	//////////////////////////////////////////////////////
	// Initialize the vertex buffers
	//////////////////////////////////////////////////////
	
	// Send the position array to the GPU
	glGenBuffers(1, &positionBufID);
	glBindBuffer(GL_ARRAY_BUFFER, positionBufID);
	glBufferData(GL_ARRAY_BUFFER, positionBuf.size()*sizeof(float), &positionBuf[0], GL_STATIC_DRAW);

	glGenBuffers(1, &trackBufID);
	glBindBuffer(GL_ARRAY_BUFFER, trackBufID);
	glBufferData(GL_ARRAY_BUFFER, trackBuf.size()*sizeof(float), &trackBuf[0], GL_STATIC_DRAW);

	// Send the normal array (if it exists) to the GPU
	glGenBuffers(1, &norBufID);
	glBindBuffer(GL_ARRAY_BUFFER, norBufID);
	glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);

	glGenBuffers(1, &texCoordsBufID);
	glBindBuffer(GL_ARRAY_BUFFER, texCoordsBufID);
	glBufferData(GL_ARRAY_BUFFER, texCoordsBuf.size()*sizeof(float), &texCoordsBuf[0], GL_STATIC_DRAW);
	
	glGenBuffers(1, &trackTexCoordsBufID);
	glBindBuffer(GL_ARRAY_BUFFER, trackTexCoordsBufID);
	glBufferData(GL_ARRAY_BUFFER, trackTexCoordsBuf.size()*sizeof(float), &trackTexCoordsBuf[0], GL_STATIC_DRAW);
	// Send the index array to the GPU
	glGenBuffers(1, &indexBufID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuf.size()*sizeof(unsigned int), &indexBuf[0], GL_STATIC_DRAW);
	
	glGenBuffers(1, &trackIndexBufID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, trackIndexBufID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, trackIndexBuf.size()*sizeof(unsigned int), &trackIndexBuf[0], GL_STATIC_DRAW);

	// Unbind the arrays
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	Image *image0 = (Image *)malloc(sizeof(Image));
	if(image0 == NULL) {
		printf("Error allocating space for image");
	}
	if(!ImageLoad("floor.bmp", image0)) {
		printf("Error loading texture image\n");
	}

	// Set active texture to texture unit 0
	glActiveTexture(GL_TEXTURE0);
	// Generate a texture buffer object
	glGenTextures(1, &texture0ID);
	// Bind the current texture to be the newly generated texture object
	glBindTexture(GL_TEXTURE_2D, texture0ID);
	// Load the actual texture data
	// Base level is 0, number of channels is 3, and border is 0.
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image0->sizeX, image0->sizeY,
				 0, GL_RGB, GL_UNSIGNED_BYTE, image0->data);
	// Generate image pyramid
	glGenerateMipmap(GL_TEXTURE_2D);
	// Set texture wrap modes for the S and T directions
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set filtering mode for magnification and minimification
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// Unbind
	glBindTexture(GL_TEXTURE_2D, 0);
	// Free image, since the data is now on the GPU
	free(image0);

	// Repeat
	Image *image2 = (Image *)malloc(sizeof(Image));
	if(image2 == NULL) {
		printf("Error allocating space for image");
	}
	if(!ImageLoad("road_all.bmp", image2)) {
		printf("Error loading texture image\n");
	}
	glActiveTexture(GL_TEXTURE2);
	glGenTextures(1, &texture2ID);
	glBindTexture(GL_TEXTURE_2D, texture2ID);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image2->sizeX, image2->sizeY,
				 0, GL_RGB, GL_UNSIGNED_BYTE, image2->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	free(image2);

	Image *image1 = (Image *)malloc(sizeof(Image));
	if(image1 == NULL) {
		printf("Error allocating space for image");
	}
	if(!ImageLoad("YELLOW.bmp", image1)) {
		printf("Error loading texture image\n");
	}
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &texture1ID);
	glBindTexture(GL_TEXTURE_2D, texture1ID);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY,
				 0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	free(image1);
		

// Repeat
	Image *image3 = (Image *)malloc(sizeof(Image));
	if(image3 == NULL) {
		printf("Error allocating space for image");
	}
	if(!ImageLoad("red_pole.bmp", image3)) {
		printf("Error loading texture image\n");
	}
	glActiveTexture(GL_TEXTURE3);
	glGenTextures(1, &texture3ID);
	glBindTexture(GL_TEXTURE_2D, texture3ID);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image3->sizeX, image3->sizeY,
				 0, GL_RGB, GL_UNSIGNED_BYTE, image3->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	free(image3);

	// Repeat
	Image *image4 = (Image *)malloc(sizeof(Image));
	if(image4 == NULL) {
		printf("Error allocating space for image");
	}
	if(!ImageLoad("double_arrow.bmp", image4)) {
		printf("Error loading texture image\n");
	}
	glActiveTexture(GL_TEXTURE4);
	glGenTextures(1, &texture4ID);
	glBindTexture(GL_TEXTURE_2D, texture4ID);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image4->sizeX, image4->sizeY,
				 0, GL_RGB, GL_UNSIGNED_BYTE, image4->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	free(image4);
	// Check GLSL
	GLSL::checkVersion();
}

void reshapeGL(int w, int h)
{
	// Set view size
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	// Set camera aspect ratio
	camera.setWindowSize(w, h);
	camera.setRadius(TRACK_RADIUS, TRACK_WIDTH);
}

void drawGL()
{

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Enable backface culling
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	// Create matrix stacks
	MatrixStack P, MV;
	// Apply camera transforms
	P.pushMatrix();
	camera.applyProjectionMatrix(&P);
	MV.pushMatrix();
	camera.applyViewMatrix(&MV);
	
	// Bind the program
	glUseProgram(pid);

	// Bind textures

	// // Send projection matrix to the GPU
	 glUniformMatrix4fv( h_P, 1, GL_FALSE, glm::value_ptr( P.topMatrix()));
	 glUniform3fv(h_lightPosCam, 1, glm::value_ptr( glm::vec3(0.0f, 2.0f,0.0f) ));

	glUniform1f(h_s, 100.0);
	glUniform3fv(h_ka, 1, glm::value_ptr( glm::vec3(0.2f, 0.2f,0.2f) ));
	glUniform3fv(h_ks, 1, glm::value_ptr( glm::vec3(0.2f, 0.2f,0.2f) ));

	//draw track

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, texture2ID);
	glUniform1i(h_texture0, 2);

	// Enable and bind position array
	GLSL::enableVertexAttribArray(h_vertexPosition);
	glBindBuffer(GL_ARRAY_BUFFER, trackBufID);
	glVertexAttribPointer(h_vertexPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// Enable and bind normal array
	GLSL::enableVertexAttribArray(h_vertNor);
	glBindBuffer(GL_ARRAY_BUFFER, norBufID);
	glVertexAttribPointer(h_vertNor, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Enable and bind texcoord array
	GLSL::enableVertexAttribArray(h_vertTexCoords);
	glBindBuffer(GL_ARRAY_BUFFER, trackTexCoordsBufID);
	glVertexAttribPointer(h_vertTexCoords, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Bind index array
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, trackIndexBufID);

	// Apply model matrix and send to GPU
	MV.pushMatrix();
	MV.multMatrix(M);
	glUniformMatrix4fv(h_MV, 1, GL_FALSE, glm::value_ptr(MV.topMatrix()));
	MV.popMatrix();
	
	// Draw
	glDrawElements(GL_TRIANGLES, (int)trackIndexBuf.size(), GL_UNSIGNED_INT, 0);
		
	//use ground texture

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, texture0ID);
	glUniform1i(h_texture0, 0);


	
	// Enable and bind position array
	GLSL::enableVertexAttribArray(h_vertexPosition);
	glBindBuffer(GL_ARRAY_BUFFER, positionBufID);
	glVertexAttribPointer(h_vertexPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// Enable and bind normal array
	GLSL::enableVertexAttribArray(h_vertNor);
	glBindBuffer(GL_ARRAY_BUFFER, norBufID);
	glVertexAttribPointer(h_vertNor, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Enable and bind texcoord array
	GLSL::enableVertexAttribArray(h_vertTexCoords);
	glBindBuffer(GL_ARRAY_BUFFER, texCoordsBufID);
	glVertexAttribPointer(h_vertTexCoords, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Bind index array
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufID);

	// Apply model matrix and send to GPU
	MV.pushMatrix();
	MV.multMatrix(M);
	glUniformMatrix4fv(h_MV, 1, GL_FALSE, glm::value_ptr(MV.topMatrix()));
	MV.popMatrix();

	// Draw
	glDrawElements(GL_TRIANGLES, (int)indexBuf.size(), GL_UNSIGNED_INT, 0);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, texture1ID);
	glUniform1i(h_texture0, 1);

	MV.pushMatrix();
	camera.drawCar(MV, h_vertexPosition, h_vertNor, h_vertTexCoords, h_MV);
	MV.popMatrix();

	//use ramp texture
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, texture4ID);
	glUniform1i(h_texture0, 4);

	for (int i = 0; i < Ramp::ramps.size(); ++i){
		Ramp::ramps.at(i).draw(MV, h_vertexPosition, h_vertNor, h_vertTexCoords, h_MV);
	}

	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, texture3ID);
	glUniform1i(h_texture0, 3);

	
	for (int i = 0; i < GuardRail::rails.size(); ++i){
		GuardRail::rails.at(i).draw(MV, h_vertexPosition, h_vertNor, h_vertTexCoords, h_MV);
	}
	// Disable and unbind
	GLSL::disableVertexAttribArray(h_vertNor);
	GLSL::disableVertexAttribArray(h_vertexPosition);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	//unbind textures
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Unbind the program
	glUseProgram(0);

	// Pop stacks
	MV.popMatrix();
	P.popMatrix();

	// Double buffer
	glutSwapBuffers();
}

void mouse(int button, int state, int x, int y)
{
  if (state==GLUT_DOWN) 
  {
    glutPostRedisplay();
  }
}

void passiveMotionGL(int x, int y)
{
	mouseCoords.x = x;
	mouseCoords.y = y;
}

void keyboardGL(unsigned char key, int x, int y)
{
	keyDown[key] = true;
	switch(key) {
		case 27:
			// ESCAPE
			exit(0);
			break;
	}
}

void keyboardUpGL(unsigned char key, int x, int y)
{
	keyDown[key] = false;
}

void idleGL()
{
	int time1 = glutGet(GLUT_ELAPSED_TIME);
	int dt = time1 - time0;
	// Update every 60Hz
	if(dt > 1000.0/60.0) {
		time0 = time1;
		camera.update(keyDown, mouseCoords);
		glutPostRedisplay();
	}
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(400, 400);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutCreateWindow("Sean Teeling");
	glutPassiveMotionFunc(passiveMotionGL);
	glutKeyboardFunc(keyboardGL);
	glutKeyboardUpFunc(keyboardUpGL);
	glutReshapeFunc(reshapeGL);
	glutDisplayFunc(drawGL);
	glutIdleFunc(idleGL);
	loadScene();
	initGL();
	glutMainLoop();
	return 0;
}
