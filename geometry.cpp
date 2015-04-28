#include "geometry.hpp"
#include "bmp.hpp"


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



////////////////
// CLASS BODY //
////////////////


body::body() :textures(false), normals(true), alpha(1.0){}

body::body(std::string& m, std::string& f)
	 :mtl_name(m), mtllib_file(f), textures(true),
	 // assume there are normals and textures - we can change this later
	 normals(true), alpha(1.0){

	std::string s;
	std::size_t cpos, rpos;
	std::ifstream fmtl(mtllib_file.c_str());

	if(!fmtl.is_open()){
		std::cout << "no such file: " << mtllib_file << std::endl;
		textures = false;
		return;
	}
	// read all the lines first

	// BUGGY! REWRITE TO DETECT OUT MTL AND DON'T PARSE THOSE MTLS THAT
	// HAPPEN AFTER!
	while(fmtl.good()){
		std::getline(fmtl, s);
		cpos = s.find('#');
		rpos = s.find('\r');
		if(cpos != std::string::npos)
			s.erase(cpos); // get rid of comments
		else if(rpos != std::string::npos)
			s.erase(rpos);
		// skip unnecessary info before our material
		if(!s.compare(0, 7, "newmtl ")
		   && s.substr(7, s.length()) == mtl_name)
			break;
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

		case 'm':
			if(s.compare(0, 7, "map_Kd ")) break;
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
	glDeleteTextures(1, &tex_num);
}

void body::add_face(std::string& s, std::vector<vec3>& v,
		    std::vector<vec3>& n, std::vector<vec2>& t){

	std::stringstream ss(s);
	std::string sp;
	face *f = new face;
	int nv, nn, nt;
	// could do w/o stringstream and getline
	while(std::getline(ss, sp, ' ')){
		// bug here!!! if textures == false and textures are provided
		// within faces then normals aren't parsed!!!
		if(textures && normals){
			if(sscanf(sp.c_str(), "%i/%i/%i", &nv, &nt, &nn)
			   == 3){
				f->add_vertex(v[nv - 1], n[nn - 1], t[nt - 1]);
				continue;
			}
		}
		if(normals){
			if(sscanf(sp.c_str(), "%i//%i", &nv, &nn) == 2){
				textures = false;
				f->add_vertex(v[nv - 1], n[nn - 1]);
				continue;
			}
		}
		if(textures){
			if(sscanf(sp.c_str(), "%i/%i", &nv, &nt) == 2){
				normals = false;
				f->add_vertex(v[nv - 1], t[nt - 1]);
				continue;
			}
		}
		if(sscanf(sp.c_str(), "%i", &nv) == 1){
			textures = false;
			normals = false;
			f->add_vertex(v[nv - 1]);
		}
		// none of the above, probably 'f' or '\'
		// treat nexline '\' here
	}
	faces.push_back(f);
}

void body::load_texture(std::string& dir){
	std::string tex_path;
	if (!textures) return;
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

	return;
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
