#include "glapp.h"
#include "glut.h"


void reshape(int w, int h) 
{
 	glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);  
	glLoadIdentity();
	gluPerspective(60.0, 1.0, 0.1, 500.0);
	glMatrixMode(GL_MODELVIEW);  
}

void display()
{
	appDisplay();
	glutSwapBuffers();
}
void idle()
{
	appIdle();
	glutSwapBuffers();
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(400, 400);
	glutCreateWindow("struc");
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	//glutKeyboardFunc(keyboard);
	//glutMotionFunc(drag);
	
	appInit();

	glutMainLoop();
	return 0;
}

