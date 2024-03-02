#include <GL/glut.h>
#include <math.h>
#define PI 3.1416f

GLfloat angle = 90.0f;

void ractangle(int x, int y, int w, int h) {
    glRecti(x, y, x + w, y + h);// w= width and h= hight
}

void drawFilledCircle(GLfloat x, GLfloat y, GLfloat radius){
    int i;
    int triangleAmount = 100;

    GLfloat twicePi = 2.0f * PI;

    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for(i = 0; i <= triangleAmount;i++) {
            glVertex2f(
                x + (radius * cos(i * twicePi / triangleAmount)),
                y + (radius * sin(i * twicePi / triangleAmount))
            );
        }
    glEnd();
}

void timer(int val) {
    glutPostRedisplay();
    glutTimerFunc(1, timer, 0);
}
GLfloat position = 0.0f;
GLfloat speed = 5.0f;

void update(int value)
{
    if(position <= -100.0)
        position = 100.0f;
    position -= speed;

	glutPostRedisplay();
	glutTimerFunc(200, update, 0);
}

void RenderScene() {
    glClearColor(0.529, 0.808, 0.922, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);


    // Sun
    glColor3ub(255, 204, 51);
        drawFilledCircle(8, 92, 6);
    // Grass
    glBegin(GL_QUADS);
        glColor3ub(128,128,128);
        glVertex2f(0, 50);
        glVertex2f(100, 50);
        glVertex2f(100, 0);
        glVertex2f(0, 0);
    glEnd();

    // Road
    glBegin(GL_QUADS);
        glColor3ub(52,52,52);
        glVertex2f(0, 32);
        glVertex2f(100, 32);
        glVertex2f(100, 12);
        glVertex2f(0, 12);
    glEnd();
    glPushMatrix();
    glTranslatef(position,0,0);
    glBegin(GL_POLYGON);
        glColor3ub(255,255,255);
        glVertex2f(2, 22+3);
        glVertex2f(2, 28+3);
        glVertex2f(4, 28+3);
        glVertex2f(8, 32+3);
        glVertex2f(14, 32+3);
        glVertex2f(19, 28+3);
        glVertex2f(23, 28+3);
        glVertex2f(23, 22+3);
    glEnd();

    // Car - Windows
    glBegin(GL_QUADS);
        glColor3ub(0,0,0);
        glVertex2f(5, 30);
        glVertex2f(8, 33);
        glVertex2f(9.5, 33);
        glVertex2f(9.5, 30);
    glEnd();
    glBegin(GL_QUADS);
        glColor3ub(0, 0, 0);
        glVertex2f(10, 33);
        glVertex2f(13.8, 33);
        glVertex2f(17.5, 30);
        glVertex2f(10, 30);
    glEnd();

    // Rotate Wheels
    glPushMatrix();
    glTranslatef(17.0f, 24.0f, 0.0f);
    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    glTranslatef(-17.0f, -24.0f, 0.0f);
        // Car - Wheels
        glColor3ub(108, 95, 91);
            drawFilledCircle(17, 24, 3);
        glColor3ub(246, 241, 238);
            drawFilledCircle(17, 24, 2.3);
            glLineWidth(10.0);
        glBegin(GL_LINES);
        //glLineWidth(20);
            glColor3ub(108, 95, 91);
            glVertex2f(17-2.3, 24);
            glVertex2f(17+2.3, 24);

            glVertex2f(17, 24-2.3);
            glVertex2f(17, 24+2.3);
        glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(7.0f, 24.0f, 0.0f);
    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    glTranslatef(-7.0f, -24.0f, 0.0f);
        glColor3ub(108, 95, 91);
            drawFilledCircle(7, 24, 3);
        glColor3ub(246, 241, 238);
            drawFilledCircle(7, 24, 2.3);
            glLineWidth(10.0);
        glBegin(GL_LINES);
        //glLineWidth(20);
            glColor3ub(108, 95, 91);
            glVertex2f(7-2.3, 24);
            glVertex2f(7+2.3, 24);

            glVertex2f(7, 24-2.3);
            glVertex2f(7, 24+2.3);
        glEnd();
    glPopMatrix();
    glPopMatrix();

    glutSwapBuffers();
    angle += 0.2;
}



int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(1024, 768);
    glutCreateWindow("Car");
    glutDisplayFunc(RenderScene);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluOrtho2D(0, 100, 0, 100);
    glutTimerFunc(0,timer, 0);
    glutTimerFunc(100,update,0);
    glutMainLoop();

    return 0;
}
