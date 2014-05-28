#include <gl/glut.h>
#include <stdio.h>

#include <assert.h>


#define MAX 50

const int Width = 800;
const int Height = 480;

GLfloat* AllShape[MAX][MAX];
int lenght_AllShape = 0;

#define NumOfPoints(i) AllShape[(i)][0][0]

void myinit(void)
{
	for (int i = 0; i < MAX; i++)
	{
		AllShape[i][0] = new GLfloat[1]{0};
	}
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
}

void drawSquares(GLenum mode)
{
	for (int i = 0; i < lenght_AllShape; i++) {
		if (NumOfPoints(i) < 1)
		{
			continue;
		}

		if (mode == GL_SELECT)
			glLoadName(i);

		glColor3fv(AllShape[i][(int) NumOfPoints(i) + 1]);

#ifndef NDEBUG
		printf("Draw: %d,%d, %f, %f\n",
			i,
			(int) NumOfPoints(i),
			AllShape[i][(int) NumOfPoints(i)][0],
			AllShape[i][(int) NumOfPoints(i)][1]);
#endif//NDEBUG

		switch ((int)NumOfPoints(i))
		{
		case 1:
			glBegin(GL_POINT);
			glVertex3fv(AllShape[i][1]);
			glEnd();
			break;
		case 2:
			glBegin(GL_LINE);
			glVertex3fv(AllShape[i][1]);
			glVertex3fv(AllShape[i][2]);
			glEnd();
			break;
		case 3:
			glBegin(GL_TRIANGLES);
			glVertex3fv(AllShape[i][1]);
			glVertex3fv(AllShape[i][2]);
			glVertex3fv(AllShape[i][3]);
			glEnd();
			break;
		default:
			glBegin(GL_POLYGON);
			for (int j = 1; j <= NumOfPoints(i); j++)
			{
				glVertex3fv(AllShape[i][j]);
			}
			glEnd();
			break;
		}

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

void myMouseFunc(int mouse, int state, int x, int y)
{
	if (state != GLUT_DOWN)
	{
		return;
	}
	switch (mouse)
	{
	case GLUT_LEFT_BUTTON:
		NumOfPoints(lenght_AllShape) += 1;
		assert(NumOfPoints(lenght_AllShape) < MAX - 1);

		AllShape[lenght_AllShape][(int) NumOfPoints(lenght_AllShape)] = new GLfloat[3]{(GLfloat) x, (GLfloat) y, 0};
		
#ifndef NDEBUG
		printf("Click: %d, %f, %f\n",
			(int) NumOfPoints(lenght_AllShape),
			AllShape[lenght_AllShape][(int) NumOfPoints(lenght_AllShape)][0],
			AllShape[lenght_AllShape][(int) NumOfPoints(lenght_AllShape)][1]);
#endif//NDEBUG

		break;
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
		gluOrtho2D(0.0, Width, 0.0, Height);
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
		AllShape[lenght_AllShape][(int) NumOfPoints(lenght_AllShape) + 1] = new GLfloat[3]{(GLfloat)0.3, (GLfloat)0.4, (GLfloat)0.5};
		lenght_AllShape++;
		drawSquares(GL_SELECT);
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	drawSquares(GL_RENDER);
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(Width, Height);
	glutCreateWindow("Draw");
	myinit();
	glutDisplayFunc(&display);
	glutMouseFunc(&myMouseFunc);
	glutKeyboardFunc(&myKeyboardFunc);
	glutMainLoop();

	return 0;
}
