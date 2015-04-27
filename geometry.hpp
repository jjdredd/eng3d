#ifndef _GEOMETRY_
#define _GEOMETRY_

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
  draw_face();
  add_vertex(vec3, vec3);

private:
  std::vector<vec3> vectices, normals;
  std::vector<vec2> textures;
};

class body{

public:
  bool textures, normals;
  body(std::string&, std::string&);
  ~body();
  void load_texture(std::string&);

private:
  std::string mtl_name, tex_name, mtllib_file;
  GLuint tex_num;
  float alpha;
  vector<face*> faces;
};

#endif	// _GEOMETRY_
