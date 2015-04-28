#ifndef _VISPHYS_
#define _VISPHYS_

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>		// get rid of this and use stringstream?

#include "geometry.hpp"

class VisPhys{

public:
	VisPhys(std::string&);
	~VisPhys();
	void LoadTextures(void);
	void draw();

private:
	std::string obj_directory;
	std::vector<body*> bodies;

};

#endif
