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
using std::cerr;

// Global user variables
int NumPoints = 1000;  // Global to hold how many points we want to generate
vec2 *points;          // Global to hold the generated points
		       // (dynamically allocated).
GLfloat zoomPercentage = 1.0; // Global to store zoom transformation
vec2 translation = vec2(0.0, 0.0); // Global to store translation information


// Global OpenGL Variables
GLuint buffer; // Identity of buffer object
GLuint vao;    // Identity of Vertex Array Object
GLuint loc;    // Identity of location of vPosition in shader storage
GLuint zoom;   // The location of the zoom uniform in the shader storage
GLuint col;    // Identity of color attribute in shader storage
GLuint translate; // location of translate uniform in shader
GLint windowSizeLoc;  // For uniform variable in shader

//----------------------------------------------------------------------------
// Start with a triangle.  Pick any point inside the triangle, and
// find the point halfway towards any vertex (draw this point), and
// repeat with this new point as the point inside the triangle until
// you have sufficient points.
//
void generate_points (int npoints)
{
  points = new vec2[npoints];
  // Specifiy the vertices for a triangle to be upside down
  vec2 vertices[3] = {vec2(-1.0, 1.0), 
		      vec2( 0.0,  -1.0),
		      vec2( 1.0, 1.0)};

  // Select an arbitrary initial point inside of the triangle
  points[0] = vec2(0.25, 0.50);

  // compute and store N-1 new points
  for (int i = 1; i < npoints; ++i) {
    int j = rand() % 3;   // pick a vertex at random

    // Compute the point halfway between the selected vertex
    //   and the previous point
    points[i] = (points[i - 1] + vertices[j]) / 2.0;
  }
}

// Set up shaders, etc.
void init()
{
  // Create the points array with all the points in it.
  generate_points(NumPoints);

  // Create a vertex array object
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
  glBufferData(GL_ARRAY_BUFFER, NumPoints*sizeof(vec2), points, GL_STATIC_DRAW);

  // Load shaders and use the resulting shader program
  GLuint program = InitShader("../src/shaders/vshaderSimple.glsl", "../src/shaders/fshaderSimple.glsl");
  glUseProgram(program);

  // Initialize the vertex position attribute from the vertex shader
  loc = glGetAttribLocation(program, "vPosition");
  if (loc==-1) {
    cerr << "Can't find shader variable: vPosition!\n";
    exit(EXIT_FAILURE);
  }
  glEnableVertexAttribArray(loc);
  glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

  // Initialize the vertex zoom uniform from the vertex shader
  zoom = glGetUniformLocation(program, "zoomPercentage");
  if (zoom==-1) {
    cerr << "Can't find shader variable: zoomPercentage!\n";
    exit(EXIT_FAILURE);
  }
  glUniform1f(zoom, zoomPercentage);

  // Initialize the vertex translation uniform from the vertex shader
  translate = glGetUniformLocation(program, "translation");
  if (translate==-1) {
    cerr << "Can't find shader variable: translation!\n";
    exit(EXIT_FAILURE);
  }
  glUniform2f(translate, translation.x, translation.y);

    // Initialize the window size uniform from the vertex shader
  windowSizeLoc = glGetUniformLocation(program, "windowSize");
  if (windowSizeLoc==-1) {
    std::cerr << "Unable to find windowSize parameter" << std::endl;
  }

  // Initialize the vertex color attribute from the vertex shader
  col = glGetAttribLocation(program, "vColor");
  if (col==-1) {
    cerr << "Can't find shader variable: oijoijojvColor!\n";
    exit(EXIT_FAILURE);
  }
  glEnableVertexAttribArray(col);
  // The buffer offset is 0, because the color information and the location information are the same
  glVertexAttribPointer(col, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

  glClearColor(0, 0, 0, 1.0); // white background
}

//----------------------------------------------------------------------------
extern "C" void display()
{
  glClear(GL_COLOR_BUFFER_BIT);          // clear the window
  //glEnable(GL_PROGRAM_POINT_SIZE_EXT);
  //glPointSize(5);
  glDrawArrays(GL_POINTS, 0, NumPoints); // draw the points
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
    std::cout << "Commands are p/P for point size, +/- for number of points" << std::endl;
    break;

    // Escape key (quit)
  case 033:
  case 'Q':
  case 'q':
    exit(EXIT_SUCCESS);
    break;

  case 'p':
    // Make points small
    // The next lines may make the code draw faster
    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_BLEND);
    glPointSize(1.0);
    glutPostRedisplay();
    break;
  case 'P':
    // Make points big
    glEnable(GL_POINT_SMOOTH);
    // In order to enable antialiasing, we need the following two lines
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glPointSize(2.0);
    glutPostRedisplay();
    break;

  case '+':
    // Increase number of points.
    NumPoints*=2;
    // Let the user know how many points are being displayed
    std::cout << "NumPoints = " << NumPoints << std::endl;
    // Reclaim memory for current points array
    delete points;
    generate_points(NumPoints);

    // Send the new points to the GPU
    // Next line isn't necessary, but doesn't hurt anything either.
    //    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBufferData(GL_ARRAY_BUFFER, NumPoints*sizeof(vec2), points, GL_STATIC_DRAW);      
    glutPostRedisplay();

    break;
  case '-':
    // Decrease number of points.
    NumPoints/=2;
    // Be sure we don't go to 0.
    if (NumPoints <1){
      NumPoints=1;
    }
    // Let the user know how many points are being displayed
    std::cout << "NumPoints = " << NumPoints << std::endl;
    // Reclaim memory for current points array
    delete points;
    generate_points(NumPoints);

    // Send the new points to the GPU
    //    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBufferData(GL_ARRAY_BUFFER, NumPoints*sizeof(vec2), points, GL_STATIC_DRAW);      
    glutPostRedisplay();

    break;

  case ' ':
    // Increase zoom percentage
    zoomPercentage += .1;
    // Let the user know the zoom percentage
    std::cout << "Zoom Percentage: " << zoomPercentage << std::endl;
    // Send the new value to the GPU
    glUniform1f(zoom, zoomPercentage);
    glutPostRedisplay();
    break;

  case 'b':
  case 'B':
    // Increase zoom percentage
    zoomPercentage -= .1;
    // Let the user know the zoom percentage
    std::cout << "Zoom Percentage: " << zoomPercentage << std::endl;
    // Send the new value to the GPU
    glUniform1f(zoom, zoomPercentage);
    glutPostRedisplay();
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
    translation.x += ((float) x / (glutGet(GLUT_WINDOW_WIDTH)) - 0.5) / zoomPercentage;
    translation.y += ((float) y / (glutGet(GLUT_WINDOW_HEIGHT)) - 0.5) / zoomPercentage;
    // Let the user know the zoom percentage
    std::cout << "x: " << (float) x / glutGet(GLUT_WINDOW_WIDTH) << " y: " << (float) x / glutGet(GLUT_WINDOW_WIDTH) << std::endl;
    std::cout << "translation x: " << translation.x << " translation y: " << translation.y << std::endl;
    // Send the new value to the GPU
    glUniform2f(translate, translation.x, translation.y);
    glutPostRedisplay();
  }
}

// rehaping routine called whenever window is resized or
// moved 
extern "C" void reshape_window(int width, int height) {
  // adjust viewport and clear
  glViewport(0, 0, width, height);

  glClearColor (0.0, 0.0, 0.0, 1.0);

  glUniform2f(windowSizeLoc, width, height);             // Pass the window
  glClear(GL_COLOR_BUFFER_BIT);
  glFlush();
}

