#include "VisPhys.hpp"

static void parse_v3(std::vector<vec3>& v, std::string *s){
	vec3 a;
	sscanf(s->c_str() + s->find(' '), "%f %f %f", &a.x, &a.y, &a.z);
	v.push_back(a);
}

static void parse_v2(std::vector<vec2>& t, std::string *s){
	vec2 a;
	sscanf(s->c_str() + s->find(' '), "%f %f", &a.x, &a.y);
	t.push_back(a);
}

VisPhys::VisPhys(char *file){
	std::string obj_filename(file);
	VisPhys(obj_filename);
}

VisPhys::VisPhys(std::string& file_name){
	std::vector<std::string> lines;
	std::vector<vec3> v, n;
	std::vector<vec2> t;
	std::ifstream obj_file(file_name);
	std::string last_mtllib;
	std::string basename = file_name.substr(0, file_name.rfind('.'));

	obj_directory = file_name.substr(0, file_name.rfind('/') + 1);
	if(!obj_file.is_open()){
		cout << file_name << ": no such file" << endl;
		return;
	}
	// read all the lines first
	while( obj_file.good() ){
		std::string s;
		std::getline(obj_file, s);
		s.erase(s.find('#'), s.length()); // get rid of comments
		lines.push_back(s);
	}
	for(int l = 0; l < lines.size(); l++){

		// avoid empty or comment-only lines
		if(lines[l].empty()) continue;
		switch(lines[l][0]){

		case 'u':
			if(lines[l].compare(0, 7, "usemtl ")) break;
			
			// new usemtl declaration
			if(last_mtllib.empty())
				last_mtllib = basename + "mtl";
			body *b = new body(lines[l].substr(7,
							   lines[l].length()),
					   last_mtllib);
			bodies.push_back(b);
			break;

		case 'f':
			// parse face line
			bodies.rbegin()->add_face(lines[l], v, n, t);
			break;

		case 'm':
			if(lines[l].compare(0, 7, "mtllib ")) break;
			last_mtllib = lines[l].substr(7, lines[l].length());
			break;

		case 'v':
			if(lines[l][1] == ' ') parse_v3(v, lines[l]);
			else if(lines[l][1] == 'n') parse_v3(n, lines[l]);
			else if(lines[l][1] == 't') parse_v2(t, lines[l]);

		default:

		}
	}
}

#if 0				// comment old constructor

#define BUFFLEN 2048

VisPhys::VisPhys(char *file){
	int read, nvert, ntex, nnorm, indm = 0, i = 0;
	unsigned body_cnt = 0, bi;
	vec3 auxv3;
	vec2 auxv2;
	char *buff;
	bool v = false, no_mtl = false;

	std::string obj_file_name (file);
	// get directory the file is in
	obj_directory = obj_file_name.substr(0, obj_file_name.rfind('/') + 1);
	std::ifstream obj_file(file);
	if( !obj_file.is_open() ){
		cout << file << ": no such file" << endl;
		// set error
	}
	// cout<<"parsing object file"<<endl;
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
				body_data[body_cnt]->mtl_name = &buff[7];
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
	// cout<<"finished parsing object file"<<endl;
	obj_file.close();
	i = 0;
	for (bi = 0; bi < body_data.size(); bi++){
		if (body_data[bi]->mtl_name.empty()){
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
		// cout<<"parsing material library file"<<endl;
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
					       && body_data[body_cnt]->mtl_name.compare(&buff[7]) )
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
				i = 0;
				// TODO
				//map_...
				// correctly treat unwanted spaces 
				if( (strncmp(buff,"map_Kd", 6) == 0) && (body_cnt < body_data.size()) ){
					body_data[body_cnt]->tex_name = &buff[7];
					body_data[body_cnt]->TEXTURE_PRESENT = true;
				}
				break;
			default:
				if(v) cout<<"skipping unknown statement in .mtl"<<endl;
			}
		}
	}
	else{
		// cout<<"material library file not found"<<endl;
		//do whatever required to proceed without this file
	}
	// cout<<"finished parsing material library file"<<endl;
	delete[] buff;
	mtl_file.close();

}

#endif	// commenting old constructor

void VisPhys::PrintStats(){
	int i = 0;
	for (unsigned bi = 0; bi < body_data.size(); bi++)
		i += body_data[bi]->face_data.size();
	cout<<"read:"<<endl<<"\t"<<data_vert.size()<<" vertices,"<<endl
	    <<"\t"<<data_norm.size()<<" normals,"<<endl
	    <<"\t"<<data_texcor.size()<<" texture coordinates,"<<endl
	    <<"\t"<<i<<" faces,"<<endl
	    <<"\t"<<body_data.size()<<" bodies."<<endl;

}

VisPhys::~VisPhys(){
	// do we really need this?
	for (unsigned bi = 0; bi < body_data.size(); bi++) delete body_data[bi];
}

void VisPhys::LoadTextures(){
	// error reporting and handling?
	for (unsigned bi = 0; bi < body_data.size(); bi++){
		bool loaded = false;
		unsigned bbi;
		if (body_data[bi]->tex_name.empty()) continue;
		for (bbi = 0; bbi < bi; bbi++){
			// optimize this code via map/hash table?
			if (body_data[bbi]->tex_name.empty()) continue;
			if ( !body_data[bbi]->tex_name.compare(body_data[bi]->tex_name) ){
				loaded = true;
				break;
			}
		}
		if (!loaded) body_data[bi]->load_texture(obj_directory);
		else body_data[bi]->tex_num = body_data[bbi]->tex_num;
	}
}

void VisPhys::draw(){
   
	for (unsigned bi = 0; bi < body_data.size(); bi++){
		// remove GLOBAL_TEX_PRES ????
		if(body_data[bi]->TEXTURE_PRESENT && GLOBAL_TEX_PRES)
			glBindTexture(GL_TEXTURE_2D, body_data[bi]->tex_num);
		// move the following loop to body draw function
		for(unsigned fi = 0;fi < body_data[bi]->face_data.size(); fi++){
			//glColor4f(1.0,1.0,1.0,
			//	body_data[bi]->alpha);
			body_data[bi]->face_data[fi]->draw(&data_vert, &data_texcor, &data_norm);
		}
	}
}
