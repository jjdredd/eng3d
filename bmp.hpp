#include <vector>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#define FOFS_IMGDAT_PTR 0x0a
#define FOFS_WIDTH 0x12
#define FOFS_HEIGHT 0x16
#define FOFS_IMG_SIZE 0x22
#define BPP 24 //assuming bpp = 24
using namespace std;
class bmp_loader{
 public:
  bmp_loader(const char *);
  ~bmp_loader();
  char *image;
  int  width, height;
  char *load_bmp();
 private:
  unsigned  img_dat_offset, img_size;
  char *raw_img, *file_path;
  FILE *fd;
  char sign[2];
  bool allocated;
};
