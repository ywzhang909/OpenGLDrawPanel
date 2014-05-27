#include <gl/glut.h>
#include <stdio.h>

#include <assert.h>


#define MAX 50

GLfloat* AllShape[MAX][MAX];
int lenght_AllShape = 0;

#define NumOfPoints(i) AllShape[(i)][0][0]

void myinit(void)
{
	for (int i = 0; i < MAX; i++)
	{
		AllShape[i][0] = new GLfloat[1]{0};
	}
	glClearColor(0.0, 0.0, 0.0, 0.0);
}

void drawSquares(GLenum mode)
{
	for (int i = 0; i < lenght_AllShape; i++) {
		if (mode == GL_SELECT)
			glLoadName(i);

		assert(NumOfPoints(i) > 2);

		glColor3fv(AllShape[i][(int)NumOfPoints(i)+1]);

		glBegin(GL_POLYGON);
		for (int j = 1; j <= NumOfPoints(i); j++)
		{
			glVertex3fv(AllShape[i][j]);
		}
		glEnd();
		
		if (mode == GL_SELECT)
		{
			glPopName();
		}
	}
	glFlush();
}

void selectHandler(int *index, int length)
{

	glutPostRedisplay();
}

void processHits(GLint hits, GLuint buffer [])
{
	GLuint names, *ptr;

	printf("hits = %d\n", hits);
	ptr = (GLuint *) buffer;

	int i;
	int *index = new int[hits];
	for (i = 0; i < hits; i++) {
		names = *ptr;
		printf(" number of names for this hit = %d\n", names);
		ptr++;
		printf(" z1 is %u;", *ptr); ptr++;
		printf(" z2 is %u\n", *ptr); ptr++;
		index[i] = *ptr;
		printf(" name is %d", *ptr); ptr++;
		printf("\n");
	}

	selectHandler(index, i);
}

#define BUFSIZE 512

void pickSquares(int mouse, int state, int x, int y)
{
	if (state != GLUT_DOWN)
	{
		return;
	}
	switch (mouse)
	{
	case GLUT_LEFT_BUTTON:
		assert(NumOfPoints(lenght_AllShape) < MAX - 1);

		AllShape[lenght_AllShape][(int)NumOfPoints(lenght_AllShape)+1] = new GLfloat[2]{(GLfloat)x, (GLfloat)y};

#ifndef NDEBUG
		printf("AllShape: %d, &f, %f\n", 
			(int) NumOfPoints(lenght_AllShape),
			AllShape[lenght_AllShape][(int) NumOfPoints(lenght_AllShape) + 1][0],
			AllShape[lenght_AllShape][(int) NumOfPoints(lenght_AllShape) + 1][1]);

		NumOfPoints(lenght_AllShape) += 1;
		break;
#endif//NDEBUG
		

	case GLUT_RIGHT_BUTTON:
		GLuint selectBuf[BUFSIZE];
		GLint hits;
		GLint viewport[4];

		glGetIntegerv(GL_VIEWPORT, viewport);

		glSelectBuffer(BUFSIZE, selectBuf);
		(void) glRenderMode(GL_SELECT);

		glInitNames();
		glPushName(-1);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		gluPickMatrix((GLdouble) x,
			(GLdouble) (viewport[3] - y), 5.0, 5.0, viewport);
		gluOrtho2D(0.0, 3.0, 0.0, 3.0);
		drawSquares(GL_SELECT);
		glPopMatrix();
		glFlush();

		hits = glRenderMode(GL_RENDER);
		processHits(hits, selectBuf);

		break;
	default:
		break;
	}
	
}

void myKeyboardFunc(unsigned char key, int x, int y)
{
	if (key == 'a')
	{
		AllShape[lenght_AllShape][(int)NumOfPoints(lenght_AllShape)+1] = new GLfloat[3]{0, 0, 0};
		lenght_AllShape++;
		glutPostRedisplay();
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	drawSquares(GL_RENDER);
}

void myReshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 3.0, 0.0, 3.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(800, 400);
	myinit();
	glutCreateWindow("Draw");
	glutDisplayFunc(&display);
	glutMouseFunc(&pickSquares);
	glutKeyboardFunc(&myKeyboardFunc);
	glutReshapeFunc(&myReshape);
	glutMainLoop();

	return 0;
}
