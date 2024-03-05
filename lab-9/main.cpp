/*#include<windows.h>
#include<GL/glut.h>
int firstobject=0;
void triangle1() {
    glColor3ub(255, 0, 0);
    glBegin(GL_POLYGON);
    glVertex2f(13,4);
    glVertex2f(17,4);
    glVertex2f(15,7);
    glEnd();
}

void triangle2() {
    glColor3ub(0, 250,0.25);
    glBegin(GL_POLYGON);
    glVertex2f(13.50, 4.25);
    glVertex2f(16.50, 4.25);
    glVertex2f(15, 6.25);
    glEnd();
}

void rectangle3() {
    glColor3ub(0, 0, 255);
    glBegin(GL_POLYGON);
    glVertex2f(12.25,3.50);
    glVertex2f(17.25, 3.50);
    glVertex2f(17.25, 7.50);
    glVertex2f(12.50,7.50);
    glEnd();
}

void rectangle4() {
    glColor3ub(255,255,255);
    glBegin(GL_POLYGON);
    glVertex2f(12,3);
    glVertex2f(18,3);
    glVertex2f(18,8);
    glVertex2f(12,8);
    glEnd();
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    switch (firstobject) {
        case 0:
            triangle1();
            break;
        case 1:
            triangle2();
            break;
        case 2:
            rectangle3();
            break;
        case 3:
            rectangle4();
            break;
    }

    glutSwapBuffers();
}

void update(int value) {

    firstobject++;
    if (firstobject > 3) {
        firstobject = 0;
    }

    glutPostRedisplay();
    glutTimerFunc(200, update, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(600, 600);
    glutCreateWindow("lab_task_2");
    glutDisplayFunc(draw);
    glutTimerFunc(0, update, 0);
    gluOrtho2D(-20,20,-10,10);
    glutMainLoop();
    return 0;
}*/

#include<windows.h>
#include<GL/glut.h>
#include<math.h>
float circle1 = 0.0f;
float circle2Y = 0.0f;
float circle3X = 0.0f;
float circle4Y = 0.0f;
float rectangle1=0.0f;
float rectangle2=0.0f;
void forrectangle1(float x, float y)
{
    glColor3ub(169,179,169);
    glBegin(GL_POLYGON);
    glVertex2f(x+0.1,y+0.1);
    glVertex2f(x+0.2,y+0.1);
    glVertex2f(x+0.2,y+0.5);
    glVertex2f(x+0.1,y+0.5);
    glEnd();
}
void drawCircle(float x, float y) {
    const float radius = 0.25f;
    const int numSegments = 360;
    glColor3ub(255, 255, 255); // Red color

    glBegin(GL_POLYGON);
    glVertex2f(x, y);
    for (int i = 0; i <= numSegments; ++i) {
        float theta = 2.0f * 3.1416f * float(i) / float(numSegments);
        float dx = radius * cosf(theta);
        float dy = radius * sinf(theta);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}
void drawCircle1(float x, float y) {
    const float radius = 0.22f;
    const int numSegments = 360;
    glColor3ub(0, 255, 255);

    glBegin(GL_POLYGON);
    glVertex2f(x, y);
    for (int i = 0; i <= numSegments; ++i) {
        float theta = 2.0f * 3.1416f * float(i) / float(numSegments);
        float dx = radius * cosf(theta);
        float dy = radius * sinf(theta);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

void draw() {

    glClear(GL_COLOR_BUFFER_BIT);

    drawCircle(circle1, 0.0f);
    drawCircle1(0.0f, circle2Y);
    drawCircle(circle3X, 0.0f);
    drawCircle1(0.0f, circle4Y);
    forrectangle1(rectangle1,0.0f);
    forrectangle1(0.0f,rectangle2);

    glutSwapBuffers();
}

void update(int value) {
    circle1 -= 0.01f;
    circle2Y -= 0.01f;
    circle3X += 0.01f;
    circle4Y += 0.01f;
    rectangle1-=0.01f;
    rectangle2-=0.01f;

    if (circle1 < -1.1f) circle1 = 1.1f;
    if (circle2Y < -1.1f) circle2Y = 1.1f;
    if (circle3X > 1.1f) circle3X = -1.1f;
    if (circle4Y > 1.1f) circle4Y = -1.1f;
    if (rectangle1 < -1.1f) rectangle1 = 1.1f;
    if (rectangle2 < -1.1f) rectangle2 = 1.1f;

    glutPostRedisplay();
    glutTimerFunc(20, update, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(600, 600);
    glutCreateWindow("lab_task_1");
    glutDisplayFunc(draw);
    glutTimerFunc(0, update, 0);
    glutMainLoop();
    return 0;
}

/*#include <GL/freeglut.h>
#include<math.h>

float circle1X = 0.0f;
float circle2Y = 0.0f;
float circle3X = 0.0f;
float circle4Y = 0.0f;

void drawCircle(float x, float y) {
    const float radius = 0.1f;
    const int numSegments = 100;
    glColor3ub(255, 0, 0); // Red color

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= numSegments; ++i) {
        float theta = 2.0f * 3.1415926f * float(i) / float(numSegments);
        float dx = radius * cosf(theta);
        float dy = radius * sinf(theta);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw four circles at different positions
    drawCircle(circle1X, 0.0f);
    drawCircle(0.0f, circle2Y);
    drawCircle(circle3X, 0.0f);
    drawCircle(0.0f, circle4Y);

    glutSwapBuffers();
}

void keyboard(unsigned char key, int, int) {
    const float moveSpeed = 0.02f;

    switch (key) {
        case 'a': // Move left
            circle1X -= moveSpeed;
            break;
        case 'd': // Move right
            circle2Y -= moveSpeed;
            break;
        case 'w': // Move up
            circle3X += moveSpeed;
            break;
        case 's': // Move down
            circle4Y += moveSpeed;
            break;
        case 27: // Escape key
            exit(0);
    }

    // Redraw the scene
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Moving Circles with Keys");

    glutDisplayFunc(draw);
    glutKeyboardFunc(keyboard);

    glutMainLoop();

    return 0;
}*/
/*
#include <GL/freeglut.h>
#include<math.h>

float circle1X = 0.0f;
float circle2Y = 0.0f;
float circle3X = 0.0f;
float circle4Y = 0.0f;

int mouseButton; // 0 for left button, 1 for right button

void drawCircle(float x, float y) {
    const float radius = 0.1f;
    const int numSegments = 100;
    glColor3ub(255, 0, 0); // Red color

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= numSegments; ++i) {
        float theta = 2.0f * 3.1415926f * float(i) / float(numSegments);
        float dx = radius * cosf(theta);
        float dy = radius * sinf(theta);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw four circles at different positions
    drawCircle(circle1X, 0.0f);
    drawCircle(0.0f, circle2Y);
    drawCircle(circle3X, 0.0f);
    drawCircle(0.0f, circle4Y);

    glutSwapBuffers();
}

void mouseClick(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON) {
            // Left mouse button clicked, set direction to move left
            mouseButton = 0;
        } else if (button == GLUT_RIGHT_BUTTON) {
            // Right mouse button clicked, set direction to move right
            mouseButton = 1;
        }
    }
}

void mouseMotion(int x, int y) {
    // Move the circles based on the direction set by the mouse click
    const float moveSpeed = 0.02f;

    if (mouseButton == 0) {
        // Move left
        circle1X -= moveSpeed;
        circle2Y -= moveSpeed;
    } else if (mouseButton == 1) {
        // Move right
        circle3X += moveSpeed;
        circle4Y += moveSpeed;
    }

    // Redraw the scene
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Moving Circles with Mouse Clicks");

    glutDisplayFunc(draw);
    glutMouseFunc(mouseClick);
    glutMotionFunc(mouseMotion);

    glutMainLoop();

    return 0;
}*/



