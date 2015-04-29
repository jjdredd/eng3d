#include "VisPhys.hpp"

static void parse_v3(std::vector<vec3>& v, std::string& s){
	vec3 a;
	sscanf(s.c_str() + s.find(' '), "%f %f %f", &a.x, &a.y, &a.z);
	v.push_back(a);
}

static void parse_v2(std::vector<vec2>& t, std::string& s){
	vec2 a;
	sscanf(s.c_str() + s.find(' '), "%f %f", &a.x, &a.y);
	t.push_back(a);
}

VisPhys::VisPhys(std::string& file_name){
	std::vector<vec3> v, n;
	std::vector<vec2> t;
	std::ifstream obj_file(file_name.c_str());
	std::string last_mtllib;
	std::string basename = file_name.substr(0, file_name.rfind('.') + 1);

	// parse .obj file first
	obj_directory = file_name.substr(0, file_name.rfind('/') + 1);
	if(!obj_file.is_open()){
		std::cout << file_name << ": no such file" << std::endl;
		return;
	}

	while(obj_file.good()){

		body *b;
		std::string Nmtl;
		std::string s;
		std::size_t cpos, rpos;

		std::getline(obj_file, s);
		cpos = s.find('#');
		rpos = s.find('\r');
		if(cpos != std::string::npos)
			s.erase(cpos); // get rid of comments
		else if(rpos != std::string::npos)
			s.erase(rpos);

		// avoid empty or comment-only lines
		if(s.empty()) continue;
		switch(s[0]){

		case 'u':
			if(s.compare(0, 7, "usemtl ")) break;

			// new usemtl declaration
			if(last_mtllib.empty())
				last_mtllib = basename + "mtl";
			Nmtl = s.substr(7);
			b = new body(Nmtl, last_mtllib);

			bodies.push_back(b);
			break;

		case 'f':
			// parse face line
			if(!bodies.size()) bodies.push_back(new body);
			(*bodies.rbegin())->add_face(s, v, n, t);
			break;

		case 'm':
			if(s.compare(0, 7, "mtllib ")) break;
			last_mtllib = obj_directory + s.substr(7);
			break;

		case 'v':
			if(s[1] == ' ') parse_v3(v, s);
			else if(s[1] == 'n') parse_v3(n, s);
			else if(s[1] == 't') parse_v2(t, s);

		default: break;

		}
	}
}

VisPhys::~VisPhys(){}

void VisPhys::LoadTextures(){

	// error reporting and handling?
	std::map<std::string, GLuint> Mt;
	for(unsigned i = 0; i < bodies.size(); i++){
		std::string name = bodies[i]->GetTextureName();
		if(Mt.find(name) == Mt.end())
			Mt[name] = bodies[i]->load_texture(obj_directory);
		else
			bodies[i]->SetTextureNumber(Mt[name]);
	}
}

void VisPhys::draw(){

	for(unsigned i = 0; i < bodies.size(); i++)
		bodies[i]->draw();
}

void VisPhys::PrintStats(std::ostream& s){

	unsigned nfaces = 0, nvert = 0;
	s << bodies.size() << "\t bodies,\n";
	for(unsigned i = 0; i < bodies.size(); i++){
		nfaces += bodies[i]->NumFaces();
		nvert += bodies[i]->NumVertices();
	}
	s << nfaces << "\t faces,\n" << nvert << "\t vertices.\n";
}
