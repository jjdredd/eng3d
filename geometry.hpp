#include <vector>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <GL/freeglut.h>    
#include <GL/gl.h>	
#include <GL/glu.h>	
#include <unistd.h> 
#define GLOBAL_TEX_PRES true

// this is bad
using namespace std;

typedef struct {
  float x; float y; float z;
} vec3;

typedef struct {
  float x; float y;
} vec2;

typedef struct {
  vec3 vertice; vec2 texture;
} univec;

typedef struct{
  int a; int b; int c;
} intvec3;

class face{
  //vector of textures , vertices & a normal vector
 public:
  face();
  ~face();
  void draw(vector<vec3>* v, vector<vec2>* t=NULL, vector<vec3>* n=NULL);
  void put_vert_tex_norm(int a, int b, int c = 0);
  void put_vert_tex(int a1, int c1 = 0);
  bool TEXTURE_PRESENT;
  vector<intvec3> vert_tex_norm; //does it belong in private?
 private:
  int nvert; //number of vertices
};

class body{
 public:
  vector<face*> face_data;
  bool TEXTURE_PRESENT;
  int face_cnt;
  GLuint tex_num;
  char *mtl_name, *tex_name;
  float alpha;
  body();
  ~body();
  void load_texture(std::string &);
};
