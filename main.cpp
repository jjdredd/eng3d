#include "geometry.hpp"
#include "bmp.hpp"
#include "VisPhys.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#define UP_ARROW 72
#define DOWN_ARROW 80
#define LEFT_ARROW 75
#define RIGHT_ARROW 77
#define Width 800
#define Height 600
using namespace std;
std::vector<VisPhys *> RenderList;
float phi, theta, fwd, rt, down;
bool display_console=false;
void keyPressed(unsigned char key, int x, int y) 
{
  /* avoid thrashing this procedure */
  usleep(100);
  
  switch (key) {    
  case 'w':
    fwd += 1.0;
    break;
  case 's':
    fwd -= 1.0;
    break;
  case 'a':
      rt += 1.0;
      break;
  case 'd':
    rt -= 1.0;
    break;
  case 'o':
    down -= 1.0;
    break;
  case 'l':
    down += 1.0;
    break;
  case 'O':
    down -= 10.0;
    break;
  case 'L':
    down += 10.0;
      break;
  case '`':
    display_console = !display_console;
    break;
  default:
    break;
  }	
}

void mouse(int x, int y){
  phi=360*(x/640.0-0.5);
  theta=360*(y/480.0-0.5);
}

void DrawGLScene()
{
  //clock_t t1;
  // char timestr[10];
  char hlwd[]="Hello World!";
  //t1=clock();
  unsigned bi, i;

  usleep(8000);

  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  if(display_console){
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glOrtho(-1,1,-1,1,0,0.2);
    glBegin(GL_POLYGON);
    glColor3f(0.09,0.09,0.12);
    glVertex3f(-1.0,-0.5,-0.15);
    glVertex3f(0.0,-0.5,-0.15);
    glVertex3f(0.0,-1.0,-0.15);
    glVertex3f(-1.0,-1.0,-0.15);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glColor3f(1.0,0.0,0.0);
    glVertex3f(-1.0,-0.5,-0.15);
    glVertex3f(0.0,-0.5,-0.15);
    glVertex3f(0.0,-1.0,-0.15);
    glVertex3f(-1.0,-1.0,-0.15);
    glEnd();
    glColor3f(1.0,1.0,1.0);
    glRasterPos3f(-1.0,-0.6,-0.1);
     for (i = 0; i < strlen(hlwd); i++) 
      glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10,hlwd[i]);
    glLoadIdentity();
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
  }

  glFrustum(-0.026f, 0.026f, -0.02f, 0.02f, 0.1f, 100000.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  //glTranslatef(rt*cosf(phi/360)+fwd*sinf(phi/360),
  //	       fwd*sinf(theta/360),
  //	       fwd*cosf(theta/360)+rt*sinf(phi));
  glTranslatef(0.0f,0.0f,fwd);
  glTranslatef(rt,0.0f,0.0f);
  glTranslatef(0.0f,down,0.0f);
  glRotatef(theta,1.0,0.0,0.0);
  glRotatef(phi,0.0,1.0,0.0);
  
  for (bi = 0;bi < RenderList.size(); bi++){
    RenderList[bi]->draw();
  }
  /* 
  sprintf(timestr,"%f",(CLOCKS_PER_SEC)/((float)(clock()-t1)));
  glColor3f(1.0,0.5,0.0);
  glRasterPos2f(0.0,0.0);
  for(i=0;(i<strlen(timestr))&&(i<5);i++) 
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10,timestr[i]);
  */
  
  glutSwapBuffers();
}
int main(int argc, char *argv[]){
  unsigned bi;

  if (argc < 2){
    cout << "Usage:you must supply a file name" << endl;
    return -1;
  }

  VisPhys *vp = new VisPhys(argv[1]);
  vp->PrintStats();
  RenderList.push_back(vp);
  
  glutInit(&argc, argv);  
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);   
  glutInitWindowSize(Width, Height);  
  glutInitWindowPosition(0, 0);  
  glutCreateWindow("hi");  
  glutDisplayFunc(&DrawGLScene);  
  glutIdleFunc(&DrawGLScene);
  glutKeyboardFunc(&keyPressed);
  glutMotionFunc(&mouse);

  GLfloat LightAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  GLfloat LightPosition[] = { 0.0f, 0.0f, 1.0f, 1.0f };


  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	
  glClearDepth(1.0);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glDepthFunc(GL_LESS);			
  glEnable(GL_DEPTH_TEST);		
  glShadeModel(GL_SMOOTH);
  glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);  // add lighting. (ambient)
  glLightfv(GL_LIGHT0, GL_POSITION,LightPosition); // set light position.
  glEnable(GL_LIGHT0);                          // turn light 1 on.
  glEnable(GL_LIGHTING);
  // glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE);
  //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
  glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
  glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
  glHint(GL_FOG_HINT,GL_NICEST);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
  glShadeModel(GL_SMOOTH);
  if(GLOBAL_TEX_PRES){
    glEnable(GL_TEXTURE_2D);
    for (bi = 0; bi < RenderList.size(); bi++){
      RenderList[bi]->LoadTextures();
    }
  }

  glutMainLoop();  

  for (bi = 0; bi < RenderList.size(); bi++) delete RenderList[bi];
  return 0;
}

