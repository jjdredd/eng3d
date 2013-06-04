/*
TODO:
!!!At the end every piece of code (if required) must be formed into it's
personal appropriate function & file!!!
everything here (except perhaps parser) should be in body class which is 
inherited by "crature?" class which has function "morph" to manipulate vertex 
data 
TODO: 
!!make the code length-of-line independent!!!
TODO:
vetex/texture & no normal
TODO:
c++
TODO:
proper alpha
TODO:
proper bmp loader
TODO:
!!free in dtors
*/
#include "geometry.h"
#include "bmp.h"
#include <cstring>
#define UP_ARROW 72
#define DOWN_ARROW 80
#define LEFT_ARROW 75
#define RIGHT_ARROW 77
#define bufflen 2048
using namespace std;
vector<vec3> data_vert, data_norm; 
vector<vec2> data_texcor;
vector<body*> body_data;
float phi, theta, fwd, rt, down;
char *dir;
void keyPressed(unsigned char key, int x, int y) 
{
    /* avoid thrashing this procedure */
    usleep(100);

    switch (key) {    
    case 'w':
      fwd+=10.0;
      break;
    case 's':
      fwd-=10.0;
      break;
    case 'a':
      rt+=10.0;
      break;
    case 'd':
      rt-=10.0;
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
    default:
      break;
    }	
}
void mouse(int x, int y){
  phi=360*(x/640.0-0.5);
  theta=360*(y/480.0-0.5);
}

void InitGL(int Width, int Height){
  GLfloat LightAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  GLfloat LightPosition[] = { 0.0f, 0.0f, 1.0, 1.0f };
  unsigned bi;
  bmp_loader *bmp;
  string dir_path(dir), tex_path;
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	
  glClearDepth(1.0);			
  glDepthFunc(GL_LESS);			
  glEnable(GL_DEPTH_TEST);		
  glShadeModel(GL_SMOOTH);		
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100000.0f);
  glMatrixMode(GL_MODELVIEW);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, LightAmbient);  // add lighting. (ambient)
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
    for(bi=0;bi<body_data.size();bi++){
      if(body_data[bi]->TEXTURE_PRESENT){
	glGenTextures(1, &body_data[bi]->tex_num);
	glBindTexture(GL_TEXTURE_2D, body_data[bi]->tex_num);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	tex_path=dir_path+body_data[bi]->tex_name;
	cout<<"loading texture: "<<tex_path<<endl;
	bmp = new bmp_loader((char*)tex_path.c_str());
	if(bmp->load_bmp()==NULL){
	  cout<<"failed to load "; //puts(body_data[bi]->tex_name);
	  body_data[bi]->TEXTURE_PRESENT = false;
	}
	else{
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp->width, bmp->height, 0, 
		     GL_BGR, GL_UNSIGNED_BYTE, bmp->image);
	}
	delete bmp;
      }
    }
  }
}
void DrawGLScene()
{
  //  clock_t t1;
  //int i;
  //char timestr[10];
  //t1=clock();
  unsigned bi, fi, p;
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glLoadIdentity();
  //glTranslatef(rt*cosf(phi/360)+fwd*sinf(phi/360),
  //	       fwd*sinf(theta/360),
  //	       fwd*cosf(theta/360)+rt*sinf(phi));
  glRotatef(theta,1.0,0.0,0.0);
  glRotatef(phi,0.0,1.0,0.0);
  glTranslatef(0.0f,down,0.0f);
  glTranslatef(0.0f,0.0f,fwd);
  glTranslatef(rt,0.0f,0.0f);
  for(bi=0;bi<body_data.size();bi++){
    if(body_data[bi]->TEXTURE_PRESENT && GLOBAL_TEX_PRES)
      glBindTexture(GL_TEXTURE_2D, body_data[bi]->tex_num);
    for(fi=0;fi<body_data[bi]->face_data.size();fi++){
      //glColor4f(1.0,1.0,1.0,
      //	body_data[bi]->alpha);
      //body_data[bi]->face_data[fi]->draw(&data_vert,&data_texcor,&data_norm);


      if(GLOBAL_TEX_PRES 
	 && body_data[bi]->face_data[fi]->TEXTURE_PRESENT 
	 /*&& t!=NULL*/){ //with textures
	glBegin(GL_POLYGON);
	for(p=0;p<body_data[bi]->face_data[fi]->vert_tex_norm.size();p++){
	  //if(normpres)
	  glNormal3f(data_norm[body_data[bi]->
			  face_data[fi]->
			  vert_tex_norm[p].b-1].x,
		     data_norm[body_data[bi]->
			  face_data[fi]->
			  vert_tex_norm[p].b-1].y,
		     data_norm[body_data[bi]->
			  face_data[fi]->
			  vert_tex_norm[p].b-1].z);
	  glTexCoord2f(data_texcor[body_data[bi]->
			    face_data[fi]->
			    vert_tex_norm[p].c-1].x,
		       data_texcor[body_data[bi]->
			    face_data[fi]->
			    vert_tex_norm[p].c-1].y);
	  glVertex3f(data_vert[body_data[bi]->
			  face_data[fi]->
			  vert_tex_norm[p].a-1].x,
		     data_vert[body_data[bi]->
			  face_data[fi]->
			  vert_tex_norm[p].a-1].y,
		     data_vert[body_data[bi]->
			  face_data[fi]->
			  vert_tex_norm[p].a-1].z);
	}
	glEnd();
	
      }
      else{ //no textures
	glBegin(GL_POLYGON);
	for(p=0;p<body_data[bi]->face_data[fi]->vert_tex_norm.size();p++){
	  //if(normpres)
	  glNormal3f(data_norm[body_data[bi]->
			  face_data[fi]->
			  vert_tex_norm[p].b-1].x,
		     data_norm[body_data[bi]->
			  face_data[fi]->
			  vert_tex_norm[p].b-1].y,
		     data_norm[body_data[bi]->
			  face_data[fi]->
			  vert_tex_norm[p].b-1].z);
	  glVertex3f(data_vert[body_data[bi]->
			  face_data[fi]->
			  vert_tex_norm[p].a-1].x,
		     data_vert[body_data[bi]->
			  face_data[fi]->
			  vert_tex_norm[p].a-1].y,
		     data_vert[body_data[bi]->
			  face_data[fi]->
			  vert_tex_norm[p].a-1].z);
	}
	glEnd();
      }
    }
  }
  /*
  sprintf(timestr,"%f\0",(CLOCKS_PER_SEC)/((float)(clock()-t1)));
  glColor3f(1.0,0.5,0.0);
  glRasterPos2f(0.0,0.0);
  for(i=0;(i<strlen(timestr))&&(i<5);i++) 
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10,timestr[i]);
  */
  glutSwapBuffers();
}
int main(int argc, char *argv[]){
  FILE *data;
  int read, window, nvert,ntex,nnorm, indm=0, i=0;
  unsigned body_cnt=0, bi;
  vec3 auxv3;
  vec2 auxv2;
  char mode[]="r",*buff;
  bool have_tex=false, file_finished=false,v=false,no_mtl=false;
  /*==================================
FILE PARSING CODE. PUT INTO ANOTHER PROCEDURE, FILE LATER
    ==================================*/
  if(argc<2){cout<<"Usage:you must supply a file name"<< endl; return -1;}
  while(argv[1][++i]!=0); while((argv[1][i]!='/') && (i>=0)) --i;
  dir = new char[i+2];
  strncpy(dir, argv[1], i+1);
  dir[i+1]=0;
  data=fopen(argv[1],mode);
  if(data==NULL){cout<<argv[1]<<": no such file"<<endl; return -1;}
  cout<<"parsing object file"<<endl;
  //TODO:
  //assume we have avalid .obj file, problem w/ comments in the middle after
  //valid data line
  //get vertices
  buff = new char[bufflen];
  fgets(buff, bufflen, data);
  indm=0; i=0;
  body_data.push_back(new body);
  while(!file_finished){
    switch(buff[0]){
    case '#':
    case ' ':
    case 0x0a:
      //comment - skip line
      break;
    case 'u':
      if(strncmp(buff,"usemtl", 6)==0){
	if(!((body_data.size()==1)&&(body_data[0]->face_data.size()==0))){
	  body_data.push_back(new body); ++body_cnt;
	}
	i=6; while(buff[++i]!=0x0a);
	body_data[body_cnt]->mtl_name= new char[i-7];
	strncpy(body_data[body_cnt]->mtl_name, &buff[7],i-7);
	//c+? use stl string! use string ctor here
	i=0;
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
	have_tex=true;
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
      read=sscanf(&buff[2],"%i/",&nvert);
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
	    while((buff[indm]!=' ')&&(buff[indm]!=0x0a)) ++indm;
	    body_data[body_cnt]->
	      face_data[body_data[body_cnt]->face_cnt]->
	      put_vert_tex_norm(nvert,nnorm);
	  }while(buff[indm]!=0x0a);
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
	    while((buff[indm]!=' ')&&(buff[indm]!=0x0a)) ++indm;
	    body_data[body_cnt]->
	      face_data[body_data[body_cnt]->face_cnt]->
	      put_vert_tex_norm(nvert,nnorm,ntex);
	  }while(buff[indm]!=0x0a);
	} 
	else if(sscanf(&buff[indm],"%i",&ntex)==1){
	  // vertex/texture or vertex
	  if(v) cout<<"vertex/texture or vertex only"<<endl;
	  while(buff[++indm]!=' ');
	  body_data[body_cnt]->
	    face_data[body_data[body_cnt]->face_cnt]->put_vert_tex(nvert,ntex);
	  do{ //one-line loop
	    sscanf(&buff[++indm],"%i/%i", &nvert, &ntex);
	    while((buff[indm]!=' ')&&(buff[indm]!=0x0a)) ++indm;
	    body_data[body_cnt]->
	      face_data[body_data[body_cnt]->face_cnt]->
	      put_vert_tex_norm(nvert,ntex);
	  }while(buff[indm]!=0x0a);
	}
      }
      else{
	sscanf(&buff[indm],"%i",&nvert);
	while(buff[++indm]!=' ');
	body_data[body_cnt]->
	  face_data[body_data[body_cnt]->face_cnt]->put_vert_tex(nvert);
	do{ //one-line loop
	  sscanf(&buff[++indm],"%i", &nvert);
	  while((buff[indm]!=' ')&&(buff[indm]!=0x0a)) ++indm;
	  body_data[body_cnt]->
	    face_data[body_data[body_cnt]->face_cnt]->put_vert_tex(nvert);
	}while(buff[indm]!=0x0a);
      }
      ++body_data[body_cnt]->face_cnt;
      break;
    default:
      if(v) cout<<"skipping unknown statement"<<endl;
    }
    if(buff[++i]==EOF) file_finished=true;
    else {
      if(fgets(buff, bufflen, data)==NULL) file_finished=true; 
      indm=0; i=0;
    }
  }
  cout<<"finished parsing object file"<<endl;
  fclose(data); i=0;
  for(bi=0;bi<body_data.size();bi++)
    if(body_data[bi]->mtl_name==NULL){
      no_mtl=true;
      if(v) cout<<"couldn't find material"<<endl;
    }
  //now parsing .mtl
  indm=0; i=0; file_finished=false; body_cnt=0;
  strncpy(strrchr(argv[1], '.')+1, "mtl", 3);
  data=fopen(argv[1],mode);
  if((data!=NULL)&&(!no_mtl)){//proceed parsing .mtl
    cout<<"parsing material library file"<<endl;
    fgets(buff, bufflen, data);
    while(!file_finished){
      switch(buff[0]){
      case 'n':
	//possibly newmtl	
	if(strncmp(buff,"newmtl", 6)==0){
	  body_cnt=0;
	  i=6; while(buff[++i]!=0x0a);
	  //use string here also
	  while((body_cnt<body_data.size())
		&&(strncmp(body_data[body_cnt]->mtl_name, &buff[7],i-7)!=0))
	    ++body_cnt;

	  i=0;
	}
	break;
      case 'd':
	if((buff[1]==' ')&&(body_cnt<body_data.size())) 
	  sscanf(&buff[2],"%f",&body_data[body_cnt]->alpha);
	break;
      case 'T':
	if((buff[1]=='r')&&(body_cnt<body_data.size())) 
	  sscanf(&buff[2]," %f",&body_data[body_cnt]->alpha);
	break;
      case 'm':
	i=0;
	//map_...
	if((strncmp(buff,"map_Kd", 6)==0)&&(body_cnt<body_data.size())){
	  i=6; while(buff[++i]!=0x0a);
	  body_data[body_cnt]->tex_name = new char[i-7];
	  strncpy(body_data[body_cnt]->tex_name,&buff[7], i-7);
	  *(body_data[body_cnt]->tex_name+i-8) = 0;
	  body_data[body_cnt]->TEXTURE_PRESENT = true;
	}
	break;
      default:
	if(v) cout<<"skipping unknown statement in .mtl"<<endl;
      }
      if(buff[++i]==EOF) file_finished=true;
      else {
	if(fgets(buff, bufflen, data)==NULL)
	  file_finished=true;
	indm=0; i=0;
      }
    }
  }
  else{
    cout<<"material library file not found"<<endl;
    //do whatever required to proceed without this file
  }
  cout<<"finished parsing material library file"<<endl;
  delete buff;
  for(bi=0;bi<body_data.size();bi++)
    i+=body_data[bi]->face_data.size();
  cout<<"read:"<<endl<<"\t"<<data_vert.size()<<" vertices,"<<endl
      <<"\t"<<data_norm.size()<<" normals,"<<endl
      <<"\t"<<data_texcor.size()<<" texture coordinates,"<<endl
      <<"\t"<<i<<" faces,"<<endl
      <<"\t"<<body_data.size()<<" bodies."<<endl;
  //init opengl
  glutInit(&argc, argv);  
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);   
  glutInitWindowSize(640, 480);  
  glutInitWindowPosition(0, 0);  
  window = glutCreateWindow("hi");  
  glutDisplayFunc(&DrawGLScene);  
  glutIdleFunc(&DrawGLScene);
  glutKeyboardFunc(&keyPressed);
  glutMotionFunc(&mouse);
  InitGL(640, 480);
  glutMainLoop();  


  return 0;
}

