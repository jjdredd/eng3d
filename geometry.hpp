#ifndef _GEOMETRY_
#define _GEOMETRY_

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <GL/freeglut.h>    
#include <GL/gl.h>	
#include <GL/glu.h>	
#include <unistd.h> 
#define GLOBAL_TEX_PRES true

// this is bad
using namespace std;

struct vec3 {
	float x, y, z;
	vec3 operator*(vec3&);
	vec3 operator-(vec3&);
	vec3 operator+( vec3&);
	vec3 operator/(float);
	vec3& operator/=(float);
	void normalize();
};

typedef struct {
	float x; float y;
} vec2;

typedef struct{
	int a; int b; int c;
} intvec3;

class face{
	//vector of textures , vertices & a normal vector
public:
	face();
	~face();
	void draw(bool, bool);
	void add_vertex(vec3&, vec3&, vec2&); // vert/tex/norm
	void add_vertex(vec3&, vec3&);	 // vert//norm
	void add_vertex(vec3&, vec2&);	 // vert/tex
	void add_vertex(vec3&);		 // vert
	unsigned NumVertices();
	void GenNormals();

private:
	std::vector<vec3> vertices, normals;
	std::vector<vec2> textures;
};

class body{

public:
	body();
	body(std::string&, std::string&);
	~body();
	void add_face(std::string&, std::vector<vec3>&,
		      std::vector<vec3>&, std::vector<vec2>&x);
	GLuint load_texture(std::string&);
	void draw();
	unsigned NumFaces();
	unsigned NumVertices();
	std::string& GetTextureName();
	GLuint GetTextureNumber();
	void SetTextureNumber(GLuint);
	void GenNormals(bool);

private:
	std::string mtl_name, texture_name, mtllib_file;
	GLuint tex_num;
	// the initial value of parse_tex should always be true !
	bool textures, normals, parse_tex;
	float alpha;
	vector<face*> faces;
};

#endif	// _GEOMETRY_
