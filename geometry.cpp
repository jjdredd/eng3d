#include <cmath>

#include "geometry.hpp"
#include "bmp.hpp"


/////////////////
// STRUCT VEC3 //
/////////////////

// vector product this x rhs
vec3 vec3::operator*(vec3& rhs){
	vec3 a;
	a.x = y * rhs.z - z * rhs.y;
	a.y = z * rhs.x - x * rhs.z;
	a.z = x * rhs.y - y * rhs.x;
	return a;
}

vec3 vec3::operator-(vec3& rhs){
	vec3 a;
	a.x = x - rhs.x;
	a.y = y - rhs.y;
	a.z = z - rhs.z;
	return a;
}

vec3 vec3::operator+(vec3& rhs){
	vec3 a;
	a.x = x + rhs.x;
	a.y = y + rhs.y;
	a.z = z + rhs.z;
	return a;
}

vec3 vec3::operator/(float f){
	vec3 a;
	a.x = x/f;
	a.y = y/f;
	a.z = z/f;
	return a;
}

vec3& vec3::operator/=(float f){
	x /= f;
	y /= f;
	z /= f;
	return *this;
}

// scalar (dot) product
inline float s_prod(vec3 *a, vec3 *b){
	return a->x * b->x + a->y * b->y + a->z * b->z;
}

// normalize this vector
void vec3::normalize(){
	float n = sqrtf(x*x + y*y + z*z);
	*this /= n;
}

////////////////
// CLASS FACE //
////////////////

face::face(){ return;}

face::~face(){
	//free everythin'
}

void face::draw(bool t,bool n){
	glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
	if(t) glTexCoordPointer(2, GL_FLOAT, 0, &textures[0]);
	if(n) glNormalPointer(GL_FLOAT, 0, &normals[0]);
	glDrawArrays(GL_POLYGON, 0, vertices.size());
}

void face::add_vertex(vec3& v, vec3& n, vec2& t){
	vertices.push_back(v);
	normals.push_back(n);
	textures.push_back(t);
}

void face::add_vertex(vec3& v, vec3& n){
	vertices.push_back(v);
	normals.push_back(n);
}

void face::add_vertex(vec3& v, vec2& t){
	vertices.push_back(v);
	textures.push_back(t);

}

void face::add_vertex(vec3& v){
	vertices.push_back(v);
}

unsigned face::NumVertices(){
	return vertices.size();
}

// (re)generate normals unconditionally!
void face::GenNormals(){
	vec3 n, a, b;
	unsigned size = vertices.size();

	if(size < 3) return;
	normals.clear();
	// first normal (just a way of wrapping vertex vector)
	a = vertices[1] - vertices[0];
	b = vertices[0] - vertices[size - 1];
	n = b * a;
	n.normalize();
	normals.push_back(n);
	// in the middle
	for(unsigned i = 1; i < size - 1; i++){
		a = vertices[i + 1] - vertices[i];
		b = vertices[i] - vertices[i - 1];
		n = b * a;
		n.normalize();
		normals.push_back(n);
	}
	// last normal
	a = vertices[0] - vertices[size - 1];
	b = vertices[size - 1] - vertices[size - 2];
	n = b * a;
	n.normalize();
	normals.push_back(n);
}


////////////////
// CLASS BODY //
////////////////

body::body() :textures(false), parse_tex(true), normals(true), alpha(1.0){}

body::body(std::string& m, std::string& f)
	:mtl_name(m), mtllib_file(f), textures(true), parse_tex(true),
	 // assume there are normals and textures - we can change this later
	 normals(true), alpha(1.0){

	std::string s;
	std::size_t cpos, rpos;
	std::ifstream fmtl(mtllib_file.c_str());
	bool ourmtl = false;

	if(!fmtl.is_open()){
		std::cout << "no such file: " << mtllib_file << std::endl;
		textures = false;
		return;
	}

	while(fmtl.good()){
		std::getline(fmtl, s);
		cpos = s.find('#');
		rpos = s.find('\r');
		if(cpos != std::string::npos)
			s.erase(cpos); // get rid of comments
		else if(rpos != std::string::npos)
			s.erase(rpos);

		if(s.empty()) continue;

		switch(s[0]){

		case 'n':
			if(s.compare(0, 7, "newmtl ")) break;
			if(ourmtl){
				ourmtl = !ourmtl;
				break;
			}
			// is this newmtl describing our material?
			// if yes - start parsing the rest of this section
			if(s.substr(7, s.length()) == mtl_name)
				ourmtl = true;
			break;

		case 'm':
			if(!ourmtl || s.compare(0, 7, "map_Kd ")) break;
			texture_name = s.substr(7);
			break;

		case 'd':
		case 'T':
			// TODO implement transparency

		default: break;
		}
	}
	if(texture_name.empty()) textures = false;
}

body::~body(){
	if(textures && parse_tex) glDeleteTextures(1, &tex_num);
}

void body::add_face(std::string& s, std::vector<vec3>& v,
		    std::vector<vec3>& n, std::vector<vec2>& t){

	std::stringstream ss(s);
	std::string sp;
	face *f = new face;
	int nv, nn, nt;
	// could do w/o stringstream and getline
	while(std::getline(ss, sp, ' ')){
		if(parse_tex && normals){
			if(sscanf(sp.c_str(), "%i/%i/%i", &nv, &nt, &nn)
			   == 3){
				f->add_vertex(v[nv - 1], n[nn - 1], t[nt - 1]);
				continue;
			}
		}
		if(normals){
			if(sscanf(sp.c_str(), "%i//%i", &nv, &nn) == 2){
				parse_tex = false;
				f->add_vertex(v[nv - 1], n[nn - 1]);
				continue;
			}
		}
		if(parse_tex){
			if(sscanf(sp.c_str(), "%i/%i", &nv, &nt) == 2){
				normals = false;
				f->add_vertex(v[nv - 1], t[nt - 1]);
				continue;
			}
		}
		if(sscanf(sp.c_str(), "%i", &nv) == 1){
			parse_tex = false;
			normals = false;
			f->add_vertex(v[nv - 1]);
		}
		// none of the above, probably 'f' or '\'
		// treat nexline '\' here
	}
	faces.push_back(f);
}

GLuint body::load_texture(std::string& dir){
	std::string tex_path;
	if (!textures || !parse_tex) return tex_num;
	glGenTextures(1, &tex_num);
	glBindTexture(GL_TEXTURE_2D, tex_num);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);
	/*glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);*/    
	tex_path = dir + texture_name;
	cout << "loading texture: " << tex_path << endl;
	bmp_loader bmp(tex_path);

	if (bmp.load_bmp()){
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, bmp.width, bmp.height,
				  GL_BGR, GL_UNSIGNED_BYTE, bmp.image);

	} else {
		cout << "failed to load " << texture_name << '\n';
		textures = false;
	}

	return tex_num;
}

void body::draw(){

	glEnableClientState(GL_VERTEX_ARRAY);
	if(normals)
		glEnableClientState(GL_NORMAL_ARRAY);
	if(textures){
		glBindTexture(GL_TEXTURE_2D, tex_num);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	for(unsigned i = 0; i < faces.size(); i++)
		faces[i]->draw(textures, normals);

	if(textures)
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	if(normals)
		glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

}

unsigned body::NumFaces(){
	return faces.size();
}

unsigned body::NumVertices(){
	unsigned n = 0;
	for(unsigned i = 0; i < faces.size(); i++)
		n += faces[i]->NumVertices();
	return n;
}

std::string& body::GetTextureName(){
	return texture_name;
}

GLuint body::GetTextureNumber(){
	return tex_num;
}

void body::SetTextureNumber(GLuint tn){
	tex_num = tn;
}

void body::GenNormals(bool check){
	if(check && normals) return;
	for(unsigned i = 0; i < faces.size(); i++)
		faces[i]->GenNormals();
	normals = true;
}
