#include "geometry.hpp"
#include "bmp.hpp"
face::face(){ return;}
face::~face(){
    //free everythin'
  }
void face::draw(vector<vec3>* v,vector<vec2>* t,vector<vec3>* n){
  unsigned  fi;
  /*  bool normpres=true;
  float x, y, z;
  if(((*n)[vert_tex_norm[0].b-1].x==0)&&
     ((*n)[vert_tex_norm[0].b-1].y==0)&&
     ((*n)[vert_tex_norm[0].b-1].z==0))
    {
       //no normals
       normpres=false;
             
    }
  */

  //draw lines if GLOBAL_LINES_FLAG&&THIS->LOCAL_TEXTURE_NOT_PRESENT
  //VVVIS SHIT
  if (GLOBAL_TEX_PRES && this->TEXTURE_PRESENT && (t != NULL)){ //with textures
    glBegin(GL_POLYGON);
    for( fi=0; fi < vert_tex_norm.size(); fi++){
      //if(normpres)
      glNormal3f((*n)[vert_tex_norm[fi].b-1].x,
      		 (*n)[vert_tex_norm[fi].b-1].y,
      		 (*n)[vert_tex_norm[fi].b-1].z);
      glTexCoord2f((*t)[vert_tex_norm[fi].c-1].x,
		   (*t)[vert_tex_norm[fi].c-1].y);
      glVertex3f((*v)[vert_tex_norm[fi].a-1].x,
		 (*v)[vert_tex_norm[fi].a-1].y,
		 (*v)[vert_tex_norm[fi].a-1].z);
    }
    glEnd();
    
  }
  else{ //no textures
    glBegin(GL_POLYGON);
    for( fi = 0; fi < vert_tex_norm.size(); fi++){
      //if(normpres)
      glNormal3f((*n)[vert_tex_norm[fi].b-1].x,
      		 (*n)[vert_tex_norm[fi].b-1].y,
      		 (*n)[vert_tex_norm[fi].b-1].z);
      glVertex3f((*v)[vert_tex_norm[fi].a-1].x,
		 (*v)[vert_tex_norm[fi].a-1].y,
		 (*v)[vert_tex_norm[fi].a-1].z);
    }
    glEnd();
  }
}
void face::put_vert_tex_norm(int a1, int b1, int c1){
  //if (c1==0) notexture;
  intvec3 d;
  d.a = a1; d.b = b1; d.c = c1;
  vert_tex_norm.push_back(d);
}
void face::put_vert_tex(int a1, int c1){
  intvec3 d;
  d.a = a1; d.b = 0; d.c = c1;
  vert_tex_norm.push_back(d);
}
body::body(){
  this->face_cnt = 0; 
  this->alpha = 1.0;
  this->TEXTURE_PRESENT = false;
  return;
}
body::~body(){
    //free everythin'
  for (unsigned i = 0; i < face_data.size(); i++) delete face_data[i];
}
void body::load_texture(std::string& dir){
  string tex_path;
  if ( !(this->TEXTURE_PRESENT))
    return;
  glGenTextures(1, &this->tex_num);
  glBindTexture(GL_TEXTURE_2D, this->tex_num);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
		  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,
		  GL_LINEAR_MIPMAP_LINEAR);
  /*glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);*/    
  tex_path = dir + this->tex_name;
  cout << "loading texture: " << tex_path << endl;
  bmp_loader bmp(tex_path.c_str());

  if (bmp.load_bmp()){
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, bmp.width, bmp.height,
		      GL_BGR, GL_UNSIGNED_BYTE, bmp.image);

  } else {
    cout << "failed to load " << this->tex_name << '\n';
    this->TEXTURE_PRESENT = false;
  }

  return;
}
