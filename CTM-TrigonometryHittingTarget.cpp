/*********
  
PROBLEMS:
1. Guns original location cannot return after restart it by pressing f1
----------
ADDITIONAL FEATURES:
1. if it is paused it gives a "PAUSED" message on the screen
2. if all objects are destroyed it gives "YOU WIN!!" message on the screen
3. After you win it gives "F2 == Bonus Game && F1 == Restart" message on the screen
4. If you press F2 after you finish the game, you have a chance to play a bonus game. In this bonus game there is a rectangle that
   moves vertically and in every time that you cant hit it its x axis changes randomly. And there is a timer that counts while you cant hit it
5. After you finish Bonus game in screen you can see your time.
*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT 700
#define TIMER_PERIOD  8 // Period for the timer.
#define TIMER_ON 1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false, obj1 = true, obj2 = true, spacebar = false, mode1 = true, mode2 = true, bonus = false, activeTimer = true, b = false;
int  winWidth, winHeight; // current Window width and height


float A = 100,
fq = 1,
C = 0,
B = 0;
float angle = 0;
double xb = 300, yb = 50, xa = 200, ya = 0;
int t = 0;

typedef struct {
    float x, y;
} point_t;

typedef struct {
    point_t pos;    // position of the object
    float   angle;  // view angle 
    float   r;
} player_t;

typedef struct {
    point_t pos;
    float angle;
    bool active;
} fire_t;


#define MAX_FIRE 1 
#define FIRE_RATE 8

player_t p = { {-winWidth / 2 - 570, winHeight / 2}, 0, 20 };
fire_t   fr[MAX_FIRE];
int fire_rate = 0;
float timer = 1;

//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
    float angle;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
    float angle;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void print(int x, int y, const char* string, void* font)
{
    int len, i;

    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, string[i]);
    }
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...)
{
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(size, size, 1);

    int len, i;
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
    }
    glPopMatrix();
}

void vertex(point_t P, point_t Tr, double angle) {
    double xp = (P.x * cos(angle) - P.y * sin(angle)) + Tr.x;
    double yp = (P.x * sin(angle) + P.y * cos(angle)) + Tr.y;
    glVertex2d(xp, yp);
}

void Gun(player_t tp) {
    glColor3f(1, 1, 0);
    glLineWidth(3);
    int a1 = 45, a2 = -15, b1 = 8, b2 = -8;
    double angle = p.angle * D2R;
    glColor3f(0.5, 0.5, 1.0);

    glColor3b(0, 0, 0);
    glBegin(GL_QUADS);
    // Barrel of the gun
    vertex({ 50, 8 }, p.pos, angle);
    vertex({ 50, -8 }, p.pos, angle);
    vertex({ -15, -8 }, p.pos, angle);
    vertex({ -15, 8 }, p.pos, angle);
    // handle part of the gun
    vertex({ -10, 8 }, p.pos, angle);
    vertex({ -15, -45 }, p.pos, angle);
    vertex({ 0, -45 }, p.pos, angle);
    vertex({ 0, 8 }, p.pos, angle);
    glEnd();
    // trigger part
    glBegin(GL_LINE_STRIP);
    vertex({ -15, -20 }, p.pos, angle);
    vertex({ 20, -20 }, p.pos, angle);
    vertex({ 20, -8 }, p.pos, angle);
    vertex({ 20, -20 }, p.pos, angle);
    glEnd();
    glBegin(GL_TRIANGLES);
    vertex({ 0, -7 }, p.pos, angle);
    vertex({ 10, -12 }, p.pos, angle);
    vertex({ 5, -15 }, p.pos, angle);
    glEnd();

    glColor3f(0.1, 0.1, 1.0);
    glLineWidth(2);


    glColor3f(0.3, 0.3, 1.0);

}


void drawFires() {
    float r = fabs(A);
    for (int i = 0; i < MAX_FIRE; i++) {
        if (fr[i].active) {
            glColor3f(1, 1, 1);
            circle(fr[i].pos.x, fr[i].pos.y, 6);

            if (fr[i].pos.x >= xb - 20 && fr[i].pos.x <= xb + 20 && fr[i].pos.y <= yb + 70 && fr[i].pos.y >= yb - 60)
                mode2 = false;    // bullet/plane intersection

            else if (fr[i].pos.x >= xa - 60 && fr[i].pos.x <= xa && fr[i].pos.y <= ya && fr[i].pos.y >= ya - 60)
            {
                bonus = false;       // bonus rectangle intersection
                b = true;
            }
            else if (fr[i].pos.x >= r * cos(angle * D2R) - 132 && fr[i].pos.x <= r * cos(angle * D2R) - 68 && fr[i].pos.y >= r * sin(angle * D2R) - 7 && fr[i].pos.y <= r * sin(angle * D2R) + 57)
            {
                mode1 = false;  // bullet/helicopter intersection

            }
        }
    }
}

float f(float x) {
    return A * sin((fq * x + C) * D2R) + B;
}


int findAvailableFire() {
    for (int i = 0; i < MAX_FIRE; i++) {
        if (fr[i].active == false) return i;
    }
    return -1;
}


void heli() {
    float r = fabs(A);
    glLineWidth(10);
    glBegin(GL_LINES);


    glColor3ub(80, 80, 80);
    glVertex2f(r * cos(angle * D2R) - 100, r * sin(angle * D2R) + 25);
    glVertex2f(r * cos(angle * D2R) - 100, r * sin(angle * D2R) + 75);

    glColor3ub(100, 0, 0);
    glVertex2f(r * cos(angle * D2R) - 150, r * sin(angle * D2R) + 25);
    glVertex2f(r * cos(angle * D2R) - 200, r * sin(angle * D2R) + 25);

    //landing gear
    glColor3ub(40, 40, 40);
    glVertex2f(r * cos(angle * D2R) - 120, r * sin(angle * D2R) + 25);
    glVertex2f(r * cos(angle * D2R) - 120, r * sin(angle * D2R) - 21);
    glVertex2f(r * cos(angle * D2R) - 80, r * sin(angle * D2R) + 25);
    glVertex2f(r * cos(angle * D2R) - 80, r * sin(angle * D2R) - 21);
    glVertex2f(r * cos(angle * D2R) - 70, r * sin(angle * D2R) - 19);
    glVertex2f(r * cos(angle * D2R) - 130, r * sin(angle * D2R) - 21);

    //propeller
    glColor3ub(80, 80, 80);
    glVertex2f(r * cos(angle * D2R) - 60, r * sin(angle * D2R) + 75);
    glVertex2f(r * cos(angle * D2R) - 140, r * sin(angle * D2R) + 75);
    glEnd();

    // body of helicopter
    glColor3ub(255, 21, 0);
    circle(r * cos(angle * D2R) - 100, r * sin(angle * D2R) + 25, 32);
    glRectf(r * cos(angle * D2R) - 180, r * sin(angle * D2R) + 35, r * cos(angle * D2R) - 70, r * sin(angle * D2R) + 15);
    glColor3ub(0, 255, 255);
    glRectf(r * cos(angle * D2R) - 110, r * sin(angle * D2R) + 35, r * cos(angle * D2R) - 68, r * sin(angle * D2R) + 10);


    // back propeller
    glColor3ub(40, 40, 40);
    glBegin(GL_LINES);
    glVertex2f(r * cos(angle * D2R) - 220, r * sin(angle * D2R) + 40);
    glVertex2f(r * cos(angle * D2R) - 180, r * sin(angle * D2R) + 10);
    glVertex2f(r * cos(angle * D2R) - 180, r * sin(angle * D2R) + 40);
    glVertex2f(r * cos(angle * D2R) - 220, r * sin(angle * D2R) + 10);
    glEnd();
}

void Obj3() {

    glColor3ub(255, 157, 0);
    glRectf(xa, ya, xa - 60, ya - 60);

}

void plane() {

    glColor3ub(255, 255, 255);
    glRectf(xb - 20, yb + 70, xb + 20, yb - 60);
    glBegin(GL_TRIANGLES);
    // head of plane
    glVertex2f(xb, yb + 110);
    glVertex2f(xb - 20, yb + 70);
    glVertex2f(xb + 20, yb + 70);

    // Upper wings
    glVertex2f(xb - 80, yb + 5);
    glVertex2f(xb - 20, yb + 50);
    glVertex2f(xb - 20, yb + 15);
    glVertex2f(xb + 80, yb + 5);
    glVertex2f(xb + 20, yb + 50);
    glVertex2f(xb + 20, yb + 15);

    // Lower Wings && Tail
    glVertex2f(xb - 40, yb - 70);
    glVertex2f(xb - 5, yb - 45);
    glVertex2f(xb - 5, yb - 70);
    glVertex2f(xb + 40, yb - 70);
    glVertex2f(xb + 5, yb - 45);
    glVertex2f(xb + 5, yb - 70);
    glVertex2f(xb, yb - 90);
    glVertex2f(xb - 20, yb - 35);
    glVertex2f(xb + 20, yb - 35);
    glEnd();

    //Window of plane
    glColor3ub(0, 0, 100);
    glRectf(xb - 10, yb + 70, xb + 10, yb + 65);
}

void display() {
    //
    // clear window to black
    //
    glClearColor(0, 0.5, 0.8, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    if (mode1 == true)
        heli();
    if (mode2 == true)
        plane();

    drawFires();
    Gun(p);

    if (bonus == true)
        Obj3();

    glColor3ub(1, 1, 1);
    glRectf(-winWidth / 2, -winHeight / 2, winWidth / 2, -winHeight / 2 + 30);
    glColor3ub(255, 255, 255);
    vprint(-winWidth / 2 + 10, -winHeight / 2 + 12, GLUT_BITMAP_9_BY_15, "<Spacebar>  Fire,  <F1>  Pause/Restart");
    glColor3ub(126, 0, 0);

    if ((mode1 != false || mode2 != false) && timer == 0)
        vprint(-winWidth / 2 + 550, -winHeight / 2 + 400, GLUT_BITMAP_TIMES_ROMAN_24, "PAUSED");

    if (mode1 == false && mode2 == false && bonus == false)
    {
        vprint(-winWidth / 2 + 550, -winHeight / 2 + 400, GLUT_BITMAP_TIMES_ROMAN_24, "YOU WON!!");
        vprint(-winWidth / 2 + 540, -winHeight / 2 + 360, GLUT_BITMAP_HELVETICA_18, "F2 == Bonus Game");
        vprint(-winWidth / 2 + 540, -winHeight / 2 + 340, GLUT_BITMAP_HELVETICA_18, "F1 == Restart");
    }
    if (bonus == true)
        vprint2(-winWidth / 2 + 500, -winHeight / 2 + 500, 0.35, "TIME : %d", t);

    if (b == true && mode1 == false && mode2 == false && bonus == false)
        vprint2(-winWidth / 2 + 500, -winHeight / 2 + 500, 0.35, "TIME : %d", t);

    glutSwapBuffers();

}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);
    if (key == ' ') spacebar = true;
    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);
    if (key == ' ') spacebar = false;
    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = true; break;
    case GLUT_KEY_DOWN: down = true; break;
    case GLUT_KEY_LEFT: left = true; break;
    case GLUT_KEY_RIGHT: right = true; break;
    }

    if (key == GLUT_KEY_F1)
    {

        if (mode1 == false && mode2 == false && bonus == false)
        {
            mode2 = true;
            mode1 = true;
            t = 0;
        }
        else if (timer == 1)
        {
            timer = 0;

        }
        else
        {
            timer = 1;
        }

    }

    if (key == GLUT_KEY_F2) {

        if (mode1 == false && mode2 == false)
        {
            bonus = true;
            t = 0;
        }
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = false; break;
    case GLUT_KEY_DOWN: down = false; break;
    case GLUT_KEY_LEFT: left = false; break;
    case GLUT_KEY_RIGHT: right = false; break;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
    // Write your codes here.



    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    display(); // refresh window.
}

void onMoveDown(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function   
    glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void turnPlayer(player_t* tp, float inc) {
    tp->angle += inc;
    if (tp->angle > 360) tp->angle -= 360;
    if (tp->angle < 0) tp->angle += 360;
}



#if TIMER_ON == 1
void onTimer(int v) {

    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
    // Write your codes here.


    if (timer == 1) {


        if (activeTimer && bonus == true) {

            t++;
        }

        angle += 2;
        if (angle > 360)
            angle -= 360;


        if (obj2 == true)
        {
            if (yb == winHeight / 2 + 20)
                yb = -winHeight / 2 - 20;
            yb = yb + 1;

        }

        if (bonus == true)
        {
            if (ya == winHeight / 2)
            {
                xa = rand() % 600;
                ya = -winHeight / 2 - 30;

            }
            ya = ya + 2;

        }

        if (right) {
            if (p.angle != 270)
                turnPlayer(&p, -3);
        }
        if (left) {
            if (p.angle != 90)
                turnPlayer(&p, 3);
        }
        if (up) {
            if (p.pos.y < winHeight / 2 - 40)
                p.pos.y = p.pos.y + 3;
        }
        if (down) {
            if (p.pos.y > -winHeight / 2 + 70.0)
                p.pos.y = p.pos.y - 3;
        }

        if (spacebar && fire_rate == 0) {
            int availFire = findAvailableFire();
            if (availFire != -1) {
                fr[availFire].pos = p.pos;
                fr[availFire].angle = p.angle;
                fr[availFire].active = true;
                fire_rate = FIRE_RATE;
            }
        }

        if (fire_rate > 0) fire_rate--;

        // Move all fires that are active.
        for (int i = 0; i < MAX_FIRE; i++) {
            if (fr[i].active) {
                fr[i].pos.x += 9 * cos(fr[i].angle * D2R);
                fr[i].pos.y += 9 * sin(fr[i].angle * D2R);

                if (fr[i].pos.x > 1000 || fr[i].pos.x < -1000 || fr[i].pos.y > 1000 || fr[i].pos.y < -1000) {
                    fr[i].active = false;
                }
            }
        }
    }
    // to refresh the window it calls display() function
    glutPostRedisplay(); // display()

}
#endif

void Init() {

    // Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //glutInitWindowPosition(100, 100);
    glutCreateWindow("HW 3: 'Hitting the Target' Mert Köroðlu");

    glutDisplayFunc(display);
    glutReshapeFunc(onResize);

    //
    // keyboard registration
    //
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);

    glutKeyboardUpFunc(onKeyUp);
    glutSpecialUpFunc(onSpecialKeyUp);

    //
    // mouse registration
    //
    glutMouseFunc(onClick);
    glutMotionFunc(onMoveDown);
    glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
    // timer event
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

    Init();

    glutMainLoop();
}
