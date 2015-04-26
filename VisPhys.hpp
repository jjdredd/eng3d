#ifndef _VISPHYS_
#define _VISPHYS_

#include <iostream>
#include <fstream>

#include "geometry.hpp"

class VisPhys{

public:
  // VisPhys(std::string &);
  VisPhys(char *);
  VisPhys(std::string);
  ~VisPhys();
  void draw(void);
  void LoadTextures(void);
  void PrintStats(void);

private:
  std::string obj_directory;
  std::vector<vec3> data_vert, data_norm; 
  std::vector<vec2> data_texcor;
  std::vector<body*> body_data;

};

#endif
