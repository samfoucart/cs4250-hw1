/**
 * @file gasket.cpp
 * 
 * @author Sam Foucart sf241616@ohio.edu
 * 
 * @brief A modified version of Angel, Shreiner, Chelberg's sierpinski gasket program.
 * The modification changes the color and implements panning and zooming.
 * 
 */
// Two-Dimensional Sierpinski Gasket       
// Generated using randomly selected vertices and bisection
// 
// Uses keyboard function for user interaction.
//
// Modified by Prof. David Chelberg to add more interactivity
// And to clean up the code and add more comments.
//
// last-modified: Tue Sep 15 15:21:54 2020
// 
#include <Angel.h>
#include "drone.h"
using std::cerr;

// Global user variables
GLfloat zoomPercentage = 1.0; // Global to store zoom transformation
vec2 translation = vec2(0.0, 0.0); // Global to store translation information
GLfloat wingTheta = 0;
GLfloat viewTheta = 0;
GLfloat viewPhi = 0;

// initial location to rubber band from.
GLsizei initx = 250;
GLsizei inity = 250; 

// last location to draw rubber band to.
GLsizei lastx = 250;
GLsizei lasty = 250; 

GLfloat missleTime = 0;
GLfloat missleTheta = 0;
GLfloat misslePhi = 0;
bool missleOnScreen;


// Global OpenGL Variables
GLuint buffer; // Identity of buffer object
GLuint vao;    // Identity of Vertex Array Object
GLuint loc;    // Identity of location of vPosition in shader storage
GLuint translate_loc;
GLuint rotate_loc;
GLuint view_loc;
GLuint scale_loc;
//GLuint zoom;   // The location of the zoom uniform in the shader storage
//GLuint col;    // Identity of color attribute in shader storage
//GLuint translate; // location of translate uniform in shader
//GLint windowSizeLoc;  // For uniform variable in shader

// Set up shaders, etc.
void init()
{
  // Create the points array with all the points in it.
  #ifdef __APPLE__
  glGenVertexArraysAPPLE(1, &vao);
  glBindVertexArrayAPPLE(vao);
  #else
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  #endif
    
  // Create and initialize a buffer object
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, NumPoints*sizeof(vec4), points, GL_STATIC_DRAW);

  // Load shaders and use the resulting shader program
  GLuint program = InitShader("../src/shaders/vshaderCube.glsl", "../src/shaders/fshaderSimple.glsl");
  glUseProgram(program);

  // Initialize the vertex position attribute from the vertex shader
  loc = glGetAttribLocation(program, "vPosition");
  if (loc==-1) {
    cerr << "Can't find shader variable: vPosition!\n";
    exit(EXIT_FAILURE);
  }
  glEnableVertexAttribArray(loc);
  glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));


  // Initialize the vertex translation uniform from the vertex shader
  translate_loc = glGetUniformLocation(program, "translation");
  if (translate_loc == -1) {
    cerr << "Can't find shader variable: translation!\n";
    exit(EXIT_FAILURE);
  }
  glUniformMatrix4fv(translate_loc, 1, GL_FALSE, Translate(0, 0, 0));

  // Initialize the vertex translation uniform from the vertex shader
  scale_loc = glGetUniformLocation(program, "scaling");
  if (scale_loc == -1) {
    cerr << "Can't find shader variable: scaling!\n";
    exit(EXIT_FAILURE);
  }
  glUniformMatrix4fv(scale_loc, 1, GL_FALSE, Scale(.25, .25, .25));

  // Initialize the vertex translation uniform from the vertex shader
  rotate_loc = glGetUniformLocation(program, "wingRotation");
  if (rotate_loc == -1) {
    cerr << "Can't find shader variable: wingRotation!\n";
    exit(EXIT_FAILURE);
  }
  glUniformMatrix4fv(rotate_loc, 1, GL_FALSE, RotateX(0));

  // Initialize the vertex translation uniform from the vertex shader
  view_loc = glGetUniformLocation(program, "viewRotation");
  if (view_loc == -1) {
    cerr << "Can't find shader variable: viewRotation!\n";
    exit(EXIT_FAILURE);
  }
  glUniformMatrix4fv(view_loc, 1, GL_FALSE, RotateX(0));

  glClearColor(0, 0, 0, 1.0); // black background

  // Set Callbacks
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutReshapeFunc(reshape_window);
  glutIdleFunc(idle);
  glutMotionFunc(movement);
}

//----------------------------------------------------------------------------
extern "C" void display()
{
  glClear(GL_COLOR_BUFFER_BIT);          // clear the window
  glEnable(GL_PROGRAM_POINT_SIZE_EXT);
  glPointSize(15);
  glutSwapBuffers();
}

//----------------------------------------------------------------------------
// Callback to process normal keyboard characters entered.
// 
extern "C" void keyboard(unsigned char key, int x, int y)
{

  switch (key) {

  case 'h':
    // Help -- give instructions
    break;

    // Escape key (quit)
  case 033:
  case 'Q':
  case 'q':
    exit(EXIT_SUCCESS);
    break;

  case 'p':
    break;
  case 'P':
    break;

  case '+':
    break;
  case '-':
    break;

  case ' ':
    missleOnScreen = true;
    missleTime = 0;
    missleTheta = viewTheta;
    misslePhi = viewPhi;
    break;

  case 'b':
  case 'B':
  break;      

  default:
    // Do nothing.
    break;
  }
}

extern "C" void mouse(int button, int state, int x, int y) {
  if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    // Calculate the position of the mouse relative to window coordinates,
    // Then scale down the translation by the zoom percentage so that it translates less the more it's zoomed in
    lastx = x;
    lasty = y;
  }
}

// rehaping routine called whenever window is resized or
// moved 
extern "C" void reshape_window(int width, int height) {
  // adjust viewport and clear
  glViewport(0, 0, width, height);

  glClearColor (0.0, 0.0, 0.0, 1.0);

  //glUniform2f(windowSizeLoc, width, height);             // Pass the window
  glClear(GL_COLOR_BUFFER_BIT);
  glFlush();
}

extern "C" void idle() {
  glClear(GL_COLOR_BUFFER_BIT);          // clear the window

  // increment the wing angle
  wingTheta += 5;

  // Rotate everything down slightly and counterclockwise
  glUniformMatrix4fv(view_loc, 1, GL_FALSE, Scale(.5, .5, .5) * RotateX(viewTheta) * RotateY(viewPhi));
  
  // draw drone body
  glUniformMatrix4fv(scale_loc, 1, GL_FALSE, Scale(.25, .25, .5));
  glUniformMatrix4fv(rotate_loc, 1, GL_FALSE, RotateX(0));
  // make the center lines not translated
  glUniformMatrix4fv(translate_loc, 1, GL_FALSE, Translate(0, 0, 0));
  glDrawArrays(GL_LINE_STRIP, 0, NumPoints); // draw the lines

  drawRotors();
  drawLauncher();
  drawMissle();

  glutSwapBuffers();
}

extern "C" void movement(int x, int y) {
  viewTheta = (y - lasty);
  viewPhi = (x - lastx);
}

void drawRotors() {
  glUniformMatrix4fv(scale_loc, 1, GL_FALSE, Scale(.2, .05, .1));
  glUniformMatrix4fv(rotate_loc, 1, GL_FALSE, RotateY(wingTheta));
  // translate each wing
  glUniformMatrix4fv(translate_loc, 1, GL_TRUE, Translate(.25, .25, .5));
  glDrawArrays(GL_LINE_STRIP, 0, NumPoints); // draw the lines

  glUniformMatrix4fv(scale_loc, 1, GL_FALSE, Scale(.2, .05, .1));
  glUniformMatrix4fv(rotate_loc, 1, GL_FALSE, RotateY(wingTheta));
  // translate each wing
  glUniformMatrix4fv(translate_loc, 1, GL_TRUE, Translate(-.25, .25, .5));
  glDrawArrays(GL_LINE_STRIP, 0, NumPoints); // draw the lines

  glUniformMatrix4fv(scale_loc, 1, GL_FALSE, Scale(.2, .05, .1));
  glUniformMatrix4fv(rotate_loc, 1, GL_FALSE, RotateY(wingTheta));
  // translate each wing
  glUniformMatrix4fv(translate_loc, 1, GL_TRUE, Translate(-.25, .25, -.5));
  glDrawArrays(GL_LINE_STRIP, 0, NumPoints); // draw the lines

  glUniformMatrix4fv(scale_loc, 1, GL_FALSE, Scale(.2, .05, .1));
  glUniformMatrix4fv(rotate_loc, 1, GL_FALSE, RotateY(wingTheta));
  // translate each wing
  glUniformMatrix4fv(translate_loc, 1, GL_TRUE, Translate(.25, .25, -.5));
  glDrawArrays(GL_LINE_STRIP, 0, NumPoints); // draw the lines
}

void drawMissle() {

  if (missleOnScreen) {
    // rotate the missle towards the constant trajectory
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, Scale(.5, .5, .5) * RotateX(missleTheta) * RotateY(misslePhi));

    glUniformMatrix4fv(scale_loc, 1, GL_FALSE, Scale(.01, .01, .3));
    glUniformMatrix4fv(rotate_loc, 1, GL_FALSE, RotateY(0));
    // translate each wing
    glUniformMatrix4fv(translate_loc, 1, GL_TRUE, Translate(0, -.25, .5 + missleTime));
    glDrawArrays(GL_LINE_STRIP, 0, NumPoints); // draw the lines

    // Rotate everything back to normal
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, Scale(.5, .5, .5) * RotateX(viewTheta) * RotateY(viewPhi));
    // tell if missle is on screen
    if (missleTime > 5) {
      missleOnScreen = false;
    } else {
      missleTime += .1;
    }
  }
  
}

void drawLauncher() {
  glUniformMatrix4fv(scale_loc, 1, GL_FALSE, Scale(.01, .01, .3));
  glUniformMatrix4fv(rotate_loc, 1, GL_FALSE, RotateY(0));
  // translate each wing
  glUniformMatrix4fv(translate_loc, 1, GL_TRUE, Translate(0, -.25, .5));
  glDrawArrays(GL_LINE_STRIP, 0, NumPoints); // draw the lines
}

