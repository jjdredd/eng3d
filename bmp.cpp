#include "bmp.hpp"

bmp_loader::bmp_loader(std::string &s){
  this->file_path = strdup(s.c_str());
  allocated = false;
  return;
}

bmp_loader::bmp_loader(const char* path){
  this->file_path = strdup(path);
  allocated = false;
  return;
}

bmp_loader::~bmp_loader(){
  if(allocated) delete[] raw_img;
  free(this->file_path);
}

// move this constructor!!!
char *bmp_loader::load_bmp(){
  //int i,j,k;
  if ( !(fd = fopen(this->file_path, "r")) ){
    cout << "BMP LOADER:: couldn't open file: " << this->file_path << '\n';
    return NULL;
  }
  fread(sign, sizeof(char), 2, fd);
  if(strncmp(sign, "BM", 2)!=0) return NULL;
  fseek(fd, FOFS_IMGDAT_PTR, SEEK_SET);
  if (fread(&img_dat_offset, sizeof(int), 1, fd)!=1) return NULL;
  fseek(fd, FOFS_WIDTH, SEEK_SET);
  if (fread(&width, sizeof(int), 1, fd)!=1) return NULL;
  fseek(fd, FOFS_HEIGHT, SEEK_SET);
  if (fread(&height, sizeof(int), 1, fd)!=1) return NULL;
  fseek(fd, FOFS_IMG_SIZE, SEEK_SET);
  if (fread(&img_size, sizeof(int), 1, fd)!=1) return NULL;
  raw_img = new char[img_size];
  allocated = true;
  fseek(fd, img_dat_offset, SEEK_SET);
  if (fread(raw_img, sizeof(char), img_size, fd) != img_size) return NULL;
  /*
  image =(char ***) new char*[height];
  for(i=0;i<height;i++){
    image[i] =(char **) new char*[width];
    for(j=0;j<width;j++){
      	image[i][j] = new char[3];
    }
  }
  for(i=height-1;i>=0;i--){
    for(j=0;j<width;j++){
      for(k=2;k>=0;k--){
	image[i][j][k]=raw_img[2*(width-1)*i+2*j+k];//down->up, bgr->rgb
      }
    }
  }
  */
  fclose(fd);
  image = raw_img;
  return image;
}
