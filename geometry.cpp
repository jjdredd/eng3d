#include "geometry.hpp"
#include "bmp.hpp"

face::face(){ return;}
face::~face(){
	//free everythin'
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


body::body(std::string& m, std::string& f)
	:mtl_name(m), mtllib_file(f), textures(true),
	 // assume there are normals and textures - we can change this later
	 normals(true), alpha(1.0){

	std::vector<std::string> lines;
	std::ifstream fmtl(mtllib_file.c_str());
	unsigned l;

	if(!fmtl.is_open()){
		cout << mtllib_file << ": no such file" << endl;
		return;
	}
	// read all the lines first
	while(fmtl.good()){
		std::string s;
		std::getline(fmtl, s);
		s.erase(s.find('#'), s.length()); // get rid of comments
		lines.push_back(s);
	}

	for(l = 0; l < lines.size(); l++){
		// skip unnecessary info before our material
		if(!lines[l].compare(0, 7, "newmtl ")
		   && lines[l].substr(7, lines[l].length()) == mtl_name)
			break;
	}
	for(++l; l < lines.size(); l++){

		if(lines[l].empty()) continue;

		switch(lines[l][0]){

		case 'm':
			if(lines[l].compare(0, 7, "map_Kd ")) break;
			texture_name = lines[l].substr(7, lines[l].length());
			break;

		case 'd':
		case 'T':
			// TODO implement transparency

		default: break;
		}
	}
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
	while(std::getline(ss, sp, ' ')){
		if(textures && normals){
			if(sscanf(sp.c_str(), "%i/%i/%i", &nv, &nt, &nn)
			   == 3){
				f->add_vertex(v[nv], n[nn], t[nt]);
				continue;
			}
		}
		if(normals){
			if(sscanf(sp.c_str(), "%i//%i", &nv, &nn) == 2){
				textures = false;
				f->add_vertex(v[nv], n[nn]);
				continue;
			}
		}
		if(textures){
			if(sscanf(sp.c_str(), "%i/%i", &nv, &nt) == 2){
				normals = false;
				f->add_vertex(v[nv], t[nt]);
				continue;
			}
		}
		if(sscanf(sp.c_str(), "%i", &nv) == 1){
			textures = false;
			normals = false;
			f->add_vertex(v[nv]);
		}
	}
	faces.push_back(f);
}

void body::load_texture(std::string& dir){
	string tex_path;
	if (!textures) return;
	glGenTextures(1, &this->tex_num);
	glBindTexture(GL_TEXTURE_2D, this->tex_num);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);
	/*glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);*/    
	tex_path = dir + this->tex_name;
	cout << "loading texture: " << tex_path << endl;
	bmp_loader bmp(tex_path);

	if (bmp.load_bmp()){
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, bmp.width, bmp.height,
				  GL_BGR, GL_UNSIGNED_BYTE, bmp.image);

	} else {
		cout << "failed to load " << this->tex_name << '\n';
		textures = false;
	}

	return;
}
