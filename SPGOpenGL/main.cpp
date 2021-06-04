#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>
#include <stack>
#include "objloader.hpp"

#define PI glm::pi<float>()
// numarul de texturi incarcate cu objloader
#define NrForme 7
GLuint shader_programme;
glm::mat4 projectionMatrix, viewMatrix, modelMatrix;
std::stack<glm::mat4> modelStack;
bool res[100];

std::vector< glm::vec3 > verticesNormals[100];

GLuint vaoObj[100], vboObj[100];

std::vector< glm::vec3 > vertices[100];
std::vector< glm::vec2 > uvs[100];
std::vector< glm::vec3 > normals[100];


float xv = 10, yv = 12, zv = 30; //originea sistemului de observare

glm::vec3 lightPos(0, 1000, 500);

glm::vec3 viewPos(2, 3, 6);


float axisRotAngle[100] = { 
							PI/2.0,
							PI/2.0,
							PI/16.0,
							PI / 16.0,
							PI / 16.0,
							PI / 16.0,
							PI/16.0,
							PI/16.0,
							PI*1.5,
							PI * 1.5,
							PI * 1.5,
							PI/2.0,
							PI / 2,
							PI / 2.0
							};
float radius = 2;
float scaleFactor = 0.01;

glm::vec3 translate[100] = {
							glm::vec3(-450, 0, 0),
							glm::vec3(-300, 0, 200),
							glm::vec3(-500, 0, -1300),
							glm::vec3(500, 0, -1000),
							glm::vec3(-1700 ,0, 150),
							glm::vec3(-2100 ,0, 400),
							glm::vec3(-1000 ,0, -1500),
							glm::vec3(6000, 0, -6000),
							glm::vec3(600, 0, 0),
							glm::vec3(550, 0, 50),
							glm::vec3(550, 0, -100),
							glm::vec3(-270, 0, 50),
							glm::vec3(2100 ,0, -400),
							glm::vec3(0, 0, -500)
							};
glm::vec3 rotate[100] = {
							glm::vec3(0, 1, 0),
							glm::vec3(0, 1, 0),
							glm::vec3(0, 1, 0),
							glm::vec3(0, 1, 0),
							glm::vec3(0, 1, 0),
							glm::vec3(0, 1, 0),
							glm::vec3(0, 1, 0),
							glm::vec3(0, 1, 0),
							glm::vec3(0, 1, 0),
							glm::vec3(0, 1, 0),
							glm::vec3(0, 1, 0),
							glm::vec3(0, 1, 0),
							glm::vec3(0, 1, 0),
							glm::vec3(0, 1, 0)
							};
glm::vec3 scale[100] = { 
							glm::vec3(1.5, 1.5, 1.5) ,
							glm::vec3(2, 2, 2),
							glm::vec3(3,  3,  3),
							glm::vec3(3,  3,  3),
							glm::vec3(3,  3,  3),
							glm::vec3(3,  3,  3),
							glm::vec3(3,  3,  3),
							glm::vec3(10, 1, 10),
							glm::vec3(2, 2, 2),
							glm::vec3(1.5, 1.5, 1.5),
							glm::vec3(1.5, 1.5, 1.5),
							glm::vec3(2, 2, 2),
							glm::vec3(3,  3,  3),
							glm::vec3(3,  3,  3)
							};
glm::vec3 culoare[100] = { 
							glm::vec3(0.8470, 0.3647, 0.3657) ,
							glm::vec3(0.8470, 0.3647, 0.3657) ,
							glm::vec3(0.6470, 0.1647, 0.1657) ,
							glm::vec3(0.6470, 0.1647, 0.1657) ,
							glm::vec3(0.6470, 0.1647, 0.1657) ,
							glm::vec3(0.13, 0.54, 0.13) ,
							glm::vec3(0.13, 0.54, 0.13) ,
							glm::vec3(0.25, 0.32, 0.14),
							glm::vec3(0.8, 0.8, 0.8),
							glm::vec3(0, 0, 0),
							glm::vec3(0, 0, 0),
							glm::vec3(0.6470, 0.1647, 0.1657),
							glm::vec3(0.8, 0.8, 0.8),
							glm::vec3(0.6470, 0.1647, 0.1657)
							};
float replici[100] = {
						2,
						5,
						1,
						3,
						1,
						1,
						1
						};


void printShaderInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}

void printProgramInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}

std::string textFileRead(char *fn)
{
	std::ifstream ifile(fn);
	std::string filetext;
	while (ifile.good()) {
		std::string line;
		std::getline(ifile, line);
		filetext.append(line + "\n");
	}
	return filetext;
}

void init()
{
	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	res[0] = loadOBJ("obj/caprioara.obj", vertices[0], uvs[0], normals[0]);
	res[1] = loadOBJ("obj/padure.obj", vertices[1], uvs[1], normals[1]);
	res[2]=loadOBJ("obj/terrain.obj", vertices[2], uvs[2], normals[2]);
	res[3]= loadOBJ("obj/lup.obj", vertices[3], uvs[3], normals[3]);
	res[4]= loadOBJ("obj/cerb.obj", vertices[4], uvs[4], normals[4]);
	res[5] = loadOBJ("obj/pestera.obj", vertices[5], uvs[5], normals[5]);
	res[6]= loadOBJ("obj/buturuga.obj", vertices[6], uvs[6], normals[6]);


	glEnable(GL_DEPTH_TEST);
	glClearColor(1, 1, 1, 0);

	glewInit();

	for (int i = 0; i <= NrForme; i++)
	{
		verticesNormals[i] = vertices[i];

		verticesNormals[i].insert(verticesNormals[i].end(), normals[i].begin(), normals[i].end());

		glGenBuffers(1, &vboObj[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vboObj[i]);
		glBufferData(GL_ARRAY_BUFFER, verticesNormals[i].size() * sizeof(glm::vec3), verticesNormals[i].data(), GL_STATIC_DRAW);

		glGenVertexArrays(1, &vaoObj[i]);
		glBindVertexArray(vaoObj[i]);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(3 * vertices[i].size() * sizeof(float)));
	}
	std::string vstext = textFileRead("vertex.vert");
	std::string fstext = textFileRead("fragment.frag");
	const char* vertex_shader = vstext.c_str();
	const char* fragment_shader = fstext.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);

	shader_programme = glCreateProgram();
	glAttachShader(shader_programme, fs);
	glAttachShader(shader_programme, vs);
	glLinkProgram(shader_programme);

	printShaderInfoLog(fs);
	printShaderInfoLog(vs);
	printProgramInfoLog(shader_programme);
}

void display()
{

	std::stack<glm::mat4> modelStack;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shader_programme);

	int indexTextura = 0;
	int indexObiect = 0;
	int nrReplici = 0;
	int NrObiecte = 0;
	for (int i = 0; i < NrForme; i++)
	{
		NrObiecte += replici[i];
	}
	while(indexTextura<NrForme)
	{
		nrReplici = replici[indexTextura];
		while (nrReplici != 0)
		{
			nrReplici--;
			glBindVertexArray(vaoObj[indexTextura]);

			GLuint lightPosLoc = glGetUniformLocation(shader_programme, "lightPos");
			glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

			GLuint viewPosLoc = glGetUniformLocation(shader_programme, "viewPos");
			glUniform3fv(viewPosLoc, 1, glm::value_ptr(viewPos));

			GLuint personajCuloare = glGetUniformLocation(shader_programme, "culoare");
			glUniform3fv(personajCuloare, 1, glm::value_ptr(culoare[indexObiect]));

			modelMatrix = glm::mat4();
			modelMatrix *= glm::translate(translate[indexObiect] * glm::vec3(scaleFactor, scaleFactor, scaleFactor));
			modelMatrix *= glm::rotate(axisRotAngle[indexObiect], rotate[indexObiect]);
			modelMatrix *= glm::translate(translate[indexObiect] * glm::vec3(scaleFactor, scaleFactor, scaleFactor)*glm::vec3(-1,-1,-1));
			modelMatrix *= glm::translate(translate[indexObiect] * glm::vec3(scaleFactor, scaleFactor, scaleFactor));
			modelMatrix *= glm::scale(scale[indexObiect] * glm::vec3(scaleFactor, scaleFactor, scaleFactor));
			GLuint modelMatrixLoc = glGetUniformLocation(shader_programme, "modelViewProjectionMatrix");
			glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix * viewMatrix * modelMatrix));

			glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));
			GLuint normalMatrixLoc = glGetUniformLocation(shader_programme, "normalMatrix");
			glUniformMatrix4fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

			glDrawArrays(GL_TRIANGLES, 0, vertices[indexTextura].size());
			indexObiect++;
		}
		indexTextura++;
	}

	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	projectionMatrix = glm::perspective(PI / 4, (float)w / h, 0.1f, 1000.0f);
	viewMatrix = glm::lookAt(glm::vec3(xv, yv, zv), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
		axisRotAngle[0] += 0.1;
		if (axisRotAngle[0] > 2 * PI) 
			axisRotAngle[0] = 0;

		break;
	case 's':
		
		axisRotAngle[0] -= 0.1;
		if (axisRotAngle[0] < 0)
			axisRotAngle[0] = 2 * PI;
		break;
	case 'd':
		axisRotAngle[1] += 0.1;
		if (axisRotAngle[1] > 2 * PI)
			axisRotAngle[1] = 0;

		break;
	case 'f':

		axisRotAngle[1] -= 0.1;
		if (axisRotAngle[1] < 0)
			axisRotAngle[1] = 2 * PI;
		break;

	case 'q':
		xv += 1;
		viewMatrix = glm::lookAt(glm::vec3(xv, yv, zv), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		break;
	case 'w':
		xv -= 1;
		viewMatrix = glm::lookAt(glm::vec3(xv, yv, zv), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		break;
	case 'e':

		yv += 1;
		viewMatrix = glm::lookAt(glm::vec3(xv, yv, zv), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		break;
	case 'r':

		yv -= 1;
		viewMatrix = glm::lookAt(glm::vec3(xv, yv, zv), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		break;
	case 't':
		zv += 1;
		viewMatrix = glm::lookAt(glm::vec3(xv, yv, zv), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		break;
	case 'y':
		zv -= 1;
		viewMatrix = glm::lookAt(glm::vec3(xv, yv, zv), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		break;
	case '+':
		scaleFactor += 0.001;
		break;
	case '-':
		scaleFactor -= 0.001;
		break;
	case 'g':
		translate[0].x += 10;
		break;
	case 'h':
		translate[0].x -= 10;
		break;
	case 'j':
		translate[0].z += 10;
		break;
	case 'k':
		translate[0].z -= 10;
	};
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(700, 700);
	glutCreateWindow("SPG");
	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}
