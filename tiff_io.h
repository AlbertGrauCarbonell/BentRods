#include <stdio.h>
#include <tiffio.h>
#include <stdlib.h>
#include <sys/stat.h>

typedef struct {
  unsigned int width;
  unsigned int height;
  unsigned int bpp;
  unsigned char *bitmap;
  unsigned int depth; //only used in single file stacks
} tImage_raw;

typedef struct {
  unsigned int width;
  unsigned int height;
  float *r;
  float *g;
  float *b;
} tImage_rgb;

typedef struct {
  unsigned int width;
  unsigned int height;
  float *bitmap;
} tImage;

typedef struct {
  unsigned int width;
  unsigned int height;
  unsigned int depth;
  float *bitmap;
} tImage3d;

void write_tiff3d(char *prefname, tImage3d *img);
void write_tiff3d_zy(char *prefname, tImage3d *img);
void write_tiff3d_z(char *prefname, tImage3d *img);

void write_tiff3d_RGB(char *prefname, tImage3d *imgr, tImage3d *imgg, tImage3d *imgb);
void write_tiff3d_RGB_z(char *prefname, tImage3d *imgr, tImage3d *imgg,tImage3d *imgb);
void write_tiff3d_RGB_zy(char *prefname, tImage3d *imgr, tImage3d *imgg,tImage3d *imgb);

void read_tiff_xyz(char **filenames, int nfiles, tImage3d *img);

void read_tiff(char * filename, tImage *img);

void read_tiff_stack(char * filename, tImage3d *img);

void write_tiff(char * filename, tImage *img);

void write_tiff_rgb(char * filename, tImage *imgr, tImage *imgg, tImage *imgb);

void read_tiff_rawargb(char * filename,tImage_raw *img);

void read_tiff_raw(char * filename,tImage_raw *img);

void write_tiff_raw(char * filename, tImage_raw *img);

void read_tiff_raw_stack(char * filename, tImage_raw *img);

int mkpath(const char *path, mode_t mode);
