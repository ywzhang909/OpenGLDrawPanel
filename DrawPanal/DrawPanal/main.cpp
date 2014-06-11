#include <gl/glut.h>
#include <stdio.h>

#include <assert.h>

GLfloat Colors[8][3] = { { 0.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 }, { 0.0, 0.0, 1.0 }, { 0.0, 1.0, 1.0 },
{ 1.0, 0.0, 1.0 }, { 1.0, 1.0, 0.0 }, { 1.0, 1.0, 1.0 } };

#define MAX 50

const int Width = 800;
const int Height = 480;

GLfloat* AllShape[MAX][MAX];
int lenght_AllShape = 0;

#define NumOfPoints(i) AllShape[(i)][0][0]

bool isMoveMode = false;

int click_x, click_y;

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

		switch ((int) NumOfPoints(i))
		{
		case 0:
		case 1:
		case 2:
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
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, Width, Height, 0);
	glMatrixMode(GL_MODELVIEW);

	glFlush();
}

int hit_num;
int hit_index[MAX];

#define CON_DEL 1
#define CON_COLOR 2

void processHits(GLint hits, GLuint buffer [])
{
	GLuint *ptr;
	ptr = (GLuint *) buffer;

	hit_num = hits;
	for (int i = 0; i < hits; i++) {
		ptr += 3;
		hit_index[i] = *ptr;
		printf("----Chosen No.%d Shape-----\n", *ptr);
		ptr++;
	}

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void myMainMenuFunc(int select)
{
	if (hit_num > 0)
	{
		switch (select)
		{
		case CON_DEL:
			for (int i = 0; i < hit_num; i++)
			{
				printf("----Change No.%d Shape-----\n", hit_index[i]);
				NumOfPoints(hit_index[i]) = 0;
			}
		default:
			break;
		}
	}
	glutPostRedisplay();
}

void mySubMenuFunc(int select)
{
	if (hit_num > 0)
	{
		for (int i = 0; i < hit_num; i++)
		{
			AllShape[hit_index[i]][(int) NumOfPoints(hit_index[i]) + 1] = Colors[select];
		}
	}
	glutPostRedisplay();
}

#define BUFSIZE 512

void myMouseFunc(int mouse, int state, int x, int y)
{

	if (state == GLUT_UP)
	{
		isMoveMode = false;
		click_x = click_y = 0;

		return;
	}


	if (state == GLUT_DOWN)

		switch (mouse)
	{
		case GLUT_LEFT_BUTTON:

			NumOfPoints(lenght_AllShape) += 1;
			if (NumOfPoints(lenght_AllShape) == MAX - 1)
			{
				return;
			}

			AllShape[lenght_AllShape][(int) NumOfPoints(lenght_AllShape)] = new GLfloat[3]{(GLfloat) x, (GLfloat) y, 0};

			break;

		case GLUT_MIDDLE_BUTTON:
			isMoveMode = true;
			click_x = x;
			click_y = y;

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
			gluOrtho2D(0.0, Width, Height, 0);
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

	switch (key)
	{
	case 'a':
		if (NumOfPoints(lenght_AllShape) < 3)
		{
			NumOfPoints(lenght_AllShape) = 0;
			return;
		}
		AllShape[lenght_AllShape][(int) NumOfPoints(lenght_AllShape) + 1]
			= new GLfloat[3]{(GLfloat) 0, (GLfloat) 0, (GLfloat) 1};
		lenght_AllShape++;
		break;
	case 'e':
		//isEditMode = !isEditMode;
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void myMotionFunc(int x, int y)
{
	if (hit_num == 0 || !isMoveMode)
	{
		return;
	}

	float dx = x - click_x;
	click_x = x;
	float dy = y - click_y;
	click_y = y;

	for (int i = 0; i < hit_num; i++)
	{
		for (int j = 1; j <= NumOfPoints(hit_index[i]); j++)
		{
			AllShape[hit_index[i]][j][0] += dx;
			AllShape[hit_index[i]][j][1] += dy;
		}
	}

	glutPostRedisplay();
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
	glutMotionFunc(&myMotionFunc);

	int id_subMenu = glutCreateMenu(&mySubMenuFunc);

	glutAddMenuEntry("Black", 0);
	glutAddMenuEntry("Red", 1);
	glutAddMenuEntry("Green", 2);
	glutAddMenuEntry("Blue", 3);
	glutAddMenuEntry("Light blue", 4);
	glutAddMenuEntry("Purple", 5);
	glutAddMenuEntry("Yellow", 6);

	glutCreateMenu(&myMainMenuFunc);
	glutAddMenuEntry("Delete", CON_DEL);
	glutAddSubMenu("Color", id_subMenu);

	glutMainLoop();

	return 0;
}
