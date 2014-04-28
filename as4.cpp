
#include "includes.h"
#include "Bone.h"
#include "Kinematics.h"

#define PI 3.14159265

//****************************************************
// Some Classes
//****************************************************

class Viewport;

class Viewport {
public:
	int w, h; // width and height
};


//****************************************************
// Global Variables
//****************************************************
Viewport viewport;
int currColor=0;
bool shading=0, wireframe=0;
GLuint object;
GLfloat lights[5][4]= {{0.2, 0.2, 0.2, 0.5}, {0.2, 0.0, 0.4, 0.5}, {0.0, 0.3, 0.3, 0.5}, {0.25, 0.25, 0.05, 0.5}, {0.0, 0.0, 0.2, 0.5}};
std::vector<Bone> bones(3);


//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
	viewport.w = w;
	viewport.h = h;

	glViewport (0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30, (GLfloat) w/(GLfloat) h, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 2, 0, 0, 0, 0, 0, 0, 1);
}


//****************************************************
// Simple init function
//****************************************************
void initScene(){
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glEnable(GL_DEPTH_TEST);
	// glEnable(GL_LIGHTING);
	
	// glLightfv(GL_LIGHT0, GL_AMBIENT, lights[0]);
	// glLightfv(GL_LIGHT0, GL_DIFFUSE, lights[0]);
	// glLightfv(GL_LIGHT0, GL_SPECULAR, lights[0]);
	// GLfloat pos[] = {2000,2000,2000};
	// glLightfv(GL_LIGHT0, GL_POSITION, pos);
	// glEnable(GL_LIGHT0);
	// glShadeModel(GL_FLAT);

	object = glGenLists(1);

	Kinematics test(1,0.01);


	test.solveFK(bones,0, PI/4, PI/2);
	// std::cout<<test.solveFK(bones,1, 0, -PI/2)<<std::endl;
	// std::cout<<test.solveFK(bones,2, PI/3, 0)<<std::endl;
	// std::cout<<test.solveFK(bones,2, -PI/3, 0)<<std::endl;
	// std::cout<<test.solveFK(bones,1, 0, PI/2)<<std::endl;
	// std::cout<<test.jacobian(bones, 0.01);
	test.solveIK(bones, Eigen::Vector3d(5, 5, 5));

	glNewList(object, GL_COMPILE);

	glLineWidth(1);
	glBegin(GL_LINES);

	glColor3f(1, 1, 0);
	glVertex3f(0,0,0);
	glVertex3f(5,5,5);

	glColor3f(1, 0, 0);
	glVertex3f(0,0,0);
	glVertex3f(100,0,0);

	glColor3f(0, 1, 0);
	glVertex3f(0,0,0);
	glVertex3f(0,100,0);

	glColor3f(0, 0, 1);
	glVertex3f(0,0,0);
	glVertex3f(0,0,100);
	glEnd();

	glLineWidth(10);
	glColor3f(0, 1, 1);
	glBegin(GL_LINE_STRIP);
	glVertex3f(0,0,0);
	for (int i=0; i<bones.size(); i++) {
		glVertex3f(bones[i].currPos[0], bones[i].currPos[1], bones[i].currPos[2]);
	}
	glEnd();
	glEndList();

	glClearColor(0.0, 0.0, 0.0, 0.0);
}


//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f (1.0, 0.0, 0.0);
	glCallList(object);
	glFlush();
	glutSwapBuffers();  
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 's':
		if (shading) { 
			glShadeModel(GL_FLAT);
		} else {
			glShadeModel(GL_SMOOTH);
		}
		glutPostRedisplay();
		shading = !shading;
		break;
	case 'w':
		if (wireframe) { 
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		} else {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		glutPostRedisplay();
		wireframe = !wireframe;
		break;
	case '+':
		glScalef(1.1,1.1,1.1);
		glutPostRedisplay();
		break;
	case '-':
		glScalef(1/1.1,1/1.1,1/1.1);
		glutPostRedisplay();
		break;
	case 'c':
		currColor++;
		glLightfv(GL_LIGHT0, GL_AMBIENT, lights[currColor%5]);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lights[currColor%5]);
		glLightfv(GL_LIGHT0, GL_SPECULAR, lights[currColor%5]);
		glutPostRedisplay();
		break;
	case 27:
		exit(0);
		break;
	}
}

void arrows(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP:
		glRotatef(15.,1.0,0.0,0.0);
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		glRotatef(-15.,1.0,0.0,0.0);
		glutPostRedisplay();
		break;
	case GLUT_KEY_LEFT:
		glRotatef(15.,0.0,1.0,0.0);
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		glRotatef(-15.,0.0,1.0,0.0);
		glutPostRedisplay();
		break;
	}
}

//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {
	bones[0]=Bone(2);
	bones[1]=Bone(1.3f);
	bones[2]=Bone(1);

	//This initializes glut
	glutInit(&argc, argv);

	//This tells glut to use a double-buffered window with red, green, and blue channels 
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	// Initalize theviewport size
	viewport.w = 640;
	viewport.h = 480;

	//The size and position of the window
	glutInitWindowSize(viewport.w, viewport.h);
	glutInitWindowPosition(0,0);
	glutCreateWindow(argv[0]);

	initScene();							// quick function to set up scene

	glutDisplayFunc(myDisplay);				// function to run when its time to draw something
	glutReshapeFunc(myReshape);				// function to run when the window gets resized
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(arrows);
	glutMainLoop();							// infinite loop that will keep drawing and resizing

	return 0;
}