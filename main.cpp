/*
TODO:
!!!At the end every piece of code (if required) must be formed into it's
personal appropriate function (class) & file!!!
body must beinherited by "crature?" class which has function "morph" 
to manipulate vertex data 
TODO: 
!!make the code length-of-line independent!!!
TODO:
vetex/texture & no normal
TODO:
c++, not c+
TODO:
proper alpha
TODO:
proper bmp loader
TODO:
!!free in dtors
TODO:
console class
TODO:
glDeleteTexture()
TODO:
!what if different bodies use 1 texture. allow no texture duplicates 
in memory!!! for this use std::map. map materials to body numbers.
when loading the texture check if it has been loaded by searching minor body 
number with the same texture name. if it has already been loaded then don't 
gen/load a new texture, just copy the old gl texture number into new body.
*/
#include "geometry.hpp"
#include "bmp.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#define UP_ARROW 72
#define DOWN_ARROW 80
#define LEFT_ARROW 75
#define RIGHT_ARROW 77
#define BUFFLEN 2048
#define Width 800
#define Height 600
using namespace std;
vector<vec3> data_vert, data_norm; 
vector<vec2> data_texcor;
vector<body*> body_data;
float phi, theta, fwd, rt, down;
bool display_console=false;
void keyPressed(unsigned char key, int x, int y) 
{
  /* avoid thrashing this procedure */
  usleep(100);
  
  switch (key) {    
  case 'w':
    fwd+=1.0;
    break;
  case 's':
    fwd-=1.0;
    break;
  case 'a':
      rt+=1.0;
      break;
  case 'd':
    rt-=1.0;
    break;
  case 'o':
    down-=1.0;
    break;
  case 'l':
      down+=1.0;
      break;
  case 'O':
    down-=10.0;
    break;
  case 'L':
    down+=10.0;
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
  unsigned bi, fi, i;

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
  glRotatef(theta,1.0,0.0,0.0);
  glRotatef(phi,0.0,1.0,0.0);
  glTranslatef(0.0f,0.0f,fwd);
  glTranslatef(rt,0.0f,0.0f);
  glTranslatef(0.0f,down,0.0f);
  
  for (bi = 0;bi < body_data.size(); bi++){
    if(body_data[bi]->TEXTURE_PRESENT && GLOBAL_TEX_PRES)
      glBindTexture(GL_TEXTURE_2D, body_data[bi]->tex_num);
    for(fi = 0;fi < body_data[bi]->face_data.size(); fi++){
      //glColor4f(1.0,1.0,1.0,
      //	body_data[bi]->alpha);
      body_data[bi]->face_data[fi]->draw(&data_vert,&data_texcor,&data_norm);
    }
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
  int read, nvert, ntex, nnorm, indm = 0, i = 0;
  unsigned body_cnt = 0, bi;
  vec3 auxv3;
  vec2 auxv2;
  char *buff;
  bool v = false, no_mtl = false;
  /*==================================
FILE PARSING CODE. PUT INTO ANOTHER PROCEDURE, FILE LATER
    ==================================*/
  if (argc < 2){
    cout << "Usage:you must supply a file name" << endl;
    return -1;
  }
  std::string obj_file_name (argv[1]);
  // get directory the file is in
  std::string obj_directory(obj_file_name, 0, obj_file_name.rfind('/') + 1);
  std::ifstream obj_file(argv[1]);
  if( !obj_file.is_open() ){
    cout << argv[1] << ": no such file" << endl;
    return -1;
  }
  cout<<"parsing object file"<<endl;
  //TODO:
  //assume we have avalid .obj file, problem w/ comments in the middle after
  //valid data line
  //get vertices
  buff = new char[BUFFLEN];
  body_data.push_back(new body);
  while( obj_file.good() ){
    obj_file.getline(buff, BUFFLEN);
    int rd_gcount = obj_file.gcount();
    if ( (rd_gcount > 1) && (buff[rd_gcount - 2] == '\r') )
      buff[rd_gcount - 2] = 0; 
    indm = i = 0;
    switch(buff[0]){
    case '#':
    case ' ':
    case '\0':
      //comment - skip line
      break;
    case 'u':
      if(strncmp(buff,"usemtl", 6) == 0){
	if (!( (body_data.size() == 1)
	       && (body_data[0]->face_data.size() == 0))){
	  body_data.push_back(new body);
	  ++body_cnt;
	}
	body_data[body_cnt]->mtl_name= new char[strlen(&buff[7]) + 1];
	strcpy(body_data[body_cnt]->mtl_name, &buff[7]);
	//c+? use stl string! use string ctor here
	i = 0;
      }
      break;
    case 'd':
    case 'b':
    case 'c':
    case 's':
    case 'p':
    case 'l':
    case 'h':
    case 'g':
    case 'o':
    case 'm':
    case 'e':
    case 't':
      //we don't need these statements for now
      if(v) cout<<"skipping unneeded statement"<<endl;
    break;
    case 'v':
      switch(buff[1]){
      case ' ':
	sscanf(&buff[2],"%f %f %f",&auxv3.x, &auxv3.y, &auxv3.z);
	data_vert.push_back(auxv3);
	break;
      case 't':
	if(buff[2]!=' ') {if(v) cout<<"skipped line after vt"<<endl; break;}
	sscanf(&buff[3],"%f %f",&auxv2.x, &auxv2.y);
      	data_texcor.push_back(auxv2);
	break;
      case 'n':
	if(buff[2]!=' ') {if(v) cout<<"skipped line after vn"<<endl; break;}
	sscanf(&buff[3],"%f %f %f\n",&auxv3.x, &auxv3.y, &auxv3.z);
	data_norm.push_back(auxv3);
	break;
      case 'p':
      default:
	if(v) cout<<"skipped a line with a 'v'"<<endl;
      }
      break;
    case 'f':
      if(buff[1]!=' ') 	break;
      body_data[body_cnt]->face_data.push_back(new face);
      read = sscanf(&buff[2],"%i/",&nvert);
      if (read==1){
	indm=0; while(buff[++indm]!='/'); 
	if(buff[++indm]=='/'){//no textures vertex//normal
	  body_data[body_cnt]->face_data[body_data[body_cnt]->face_cnt]
	    ->TEXTURE_PRESENT = false;
	  if(v) cout<<"no texture on face "<<
		  body_data[body_cnt]->face_cnt<<endl;
	  sscanf(&buff[++indm],"%i",&nnorm);
	  while(buff[++indm]!=' ');
	  body_data[body_cnt]->
	    face_data[body_data[body_cnt]->face_cnt]->
	    put_vert_tex_norm(nvert,nnorm);
	  do{ //one-line loop
	    sscanf(&buff[++indm],"%i//%i", &nvert, &nnorm);
	    while ((buff[indm] != ' ') && (buff[indm] != '\0')) ++indm;
	    body_data[body_cnt]->
	      face_data[body_data[body_cnt]->face_cnt]->
	      put_vert_tex_norm(nvert,nnorm);
	  } while (buff[indm] != '\0');
	}
	else if(sscanf(&buff[indm],"%i/%i",&ntex,&nnorm)==2){
	  while(buff[++indm]!=' ');
	  body_data[body_cnt]->face_data[body_data[body_cnt]->face_cnt]
	    ->TEXTURE_PRESENT = true;
	  if(v) cout<<"texture on face "<<body_data[body_cnt]->face_cnt<<endl;
	  body_data[body_cnt]->
	    face_data[body_data[body_cnt]->face_cnt]->
	    put_vert_tex_norm(nvert,nnorm,ntex);
	  do{ //one-line loop
	    sscanf(&buff[++indm],"%i/%i/%i", &nvert, &ntex,  &nnorm);
	    while ((buff[indm] != ' ') && (buff[indm] != '\0')) ++indm;
	    body_data[body_cnt]->
	      face_data[body_data[body_cnt]->face_cnt]->
	      put_vert_tex_norm(nvert,nnorm,ntex);
	  }while (buff[indm] != '\0');
	} 
	else if(sscanf(&buff[indm],"%i",&ntex)==1){
	  // vertex/texture or vertex
	  if(v) cout<<"vertex/texture or vertex only"<<endl;
	  while(buff[++indm]!=' ');
	  body_data[body_cnt]->
	    face_data[body_data[body_cnt]->face_cnt]->put_vert_tex(nvert,ntex);
	  do{ //one-line loop
	    sscanf(&buff[++indm],"%i/%i", &nvert, &ntex);
	    while ((buff[indm] != ' ') && (buff[indm] != '\0')) ++indm;
	    body_data[body_cnt]->
	      face_data[body_data[body_cnt]->face_cnt]->
	      put_vert_tex_norm(nvert,ntex);
	  } while (buff[indm] != '\0');
	}
      }
      else{
	sscanf(&buff[indm],"%i",&nvert);
	while(buff[++indm]!=' ');
	body_data[body_cnt]->
	  face_data[body_data[body_cnt]->face_cnt]->put_vert_tex(nvert);
	do{ //one-line loop
	  sscanf(&buff[++indm],"%i", &nvert);
	  while ((buff[indm]!=' ') && (buff[indm] != '\0')) ++indm;
	  body_data[body_cnt]->
	    face_data[body_data[body_cnt]->face_cnt]->put_vert_tex(nvert);
	} while (buff[indm] != '\0');
      }
      ++body_data[body_cnt]->face_cnt;
      break;
    default:
      if(v) cout<<"skipping unknown statement"<<endl;
    }
  }
  cout<<"finished parsing object file"<<endl;
  obj_file.close();
  i = 0;
  for (bi = 0; bi < body_data.size(); bi++){
    if (body_data[bi]->mtl_name == NULL){
      no_mtl = true;
      if(v) cout<<"couldn't find material"<<endl;
    }
  }
  //now parsing .mtl
  body_cnt = 0;
  std::string mtl_file_name(obj_file_name);
  mtl_file_name.replace(mtl_file_name.rfind('.'), 4, ".mtl");
  std::ifstream mtl_file(mtl_file_name.c_str());
  if( mtl_file.is_open() && (!no_mtl) ){//proceed parsing .mtl
    cout<<"parsing material library file"<<endl;
    while (mtl_file.good()){
      indm = i = 0;
      mtl_file.getline(buff, BUFFLEN);
      int rd_gcount = mtl_file.gcount();
      if ( (rd_gcount > 1) && (buff[rd_gcount - 2] == '\r') )
	buff[rd_gcount - 2] = '\0';
      switch(buff[0]){
      case 'n':
	//possibly newmtl	
	if( strncmp(buff,"newmtl", 6) == 0 ){
	  body_cnt = 0;
	  i = 6; while (buff[++i] != '\0');
	  //use string here also
	  while( (body_cnt < body_data.size())
		&& (strncmp(body_data[body_cnt]->mtl_name,
			    &buff[7], i - 7) != 0) )
	    ++body_cnt;
	  i = 0;
	}
	break;
      case 'd':
	if ( (buff[1] == ' ') && (body_cnt < body_data.size()) ) 
	  sscanf(&buff[2], "%f", &body_data[body_cnt]->alpha);
	break;
      case 'T':
	if((buff[1]=='r')&&(body_cnt<body_data.size())) 
	  sscanf(&buff[2]," %f",&body_data[body_cnt]->alpha);
	break;
      case 'm':
	i=0;
	// TODO
	//map_...
	// correctly treat unwanted spaces 
	if( (strncmp(buff,"map_Kd", 6) == 0) && (body_cnt < body_data.size()) ){
	  body_data[body_cnt]->tex_name = new char[strlen(&buff[7]) + 1];
	  strcpy(body_data[body_cnt]->tex_name, &buff[7]);
	  body_data[body_cnt]->TEXTURE_PRESENT = true;
	}
	break;
      default:
	if(v) cout<<"skipping unknown statement in .mtl"<<endl;
      }
    }
  }
  else{
    cout<<"material library file not found"<<endl;
    //do whatever required to proceed without this file
  }
  cout<<"finished parsing material library file"<<endl;
  delete[] buff;
  mtl_file.close();
  for( bi = 0; bi < body_data.size(); bi++)
    i += body_data[bi]->face_data.size();
  cout<<"read:"<<endl<<"\t"<<data_vert.size()<<" vertices,"<<endl
      <<"\t"<<data_norm.size()<<" normals,"<<endl
      <<"\t"<<data_texcor.size()<<" texture coordinates,"<<endl
      <<"\t"<<i<<" faces,"<<endl
      <<"\t"<<body_data.size()<<" bodies."<<endl;
  //init opengl
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
    //last! 
    for( bi = 0; bi < body_data.size(); bi++){
      bool loaded = false;
      unsigned bbi;
      if (!body_data[bi]->tex_name) continue;
      for (bbi = 0; bbi < bi; bbi++){
      	// optimize this code via map/hash table?
      	if (!body_data[bbi]->tex_name) continue;
      	if ( !strcmp(body_data[bbi]->tex_name, body_data[bi]->tex_name) ){
      	  loaded = true;
      	  break;
      	}
      }
      if (!loaded) body_data[bi]->load_texture(obj_directory);
      else body_data[bi]->tex_num = body_data[bbi]->tex_num;
    }
  }

  glutMainLoop();  

  for (bi = 0; bi < body_data.size(); bi++) delete body_data[i];
  return 0;
}

