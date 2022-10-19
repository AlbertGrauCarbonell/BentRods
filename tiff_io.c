#include <stdio.h>
#include <tiffio.h>
#include <stdlib.h>
#include <glob.h>
#include "tiff_io.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

typedef struct stat Stat;

//=====================

static int do_mkdir(const char *path, mode_t mode) {
  Stat st;
  int  status = 0;

  if (stat(path, &st) != 0) {  /* Directory does not exist. EEXIST for race condition */
    if(mkdir(path, mode) != 0 && errno != EEXIST)
      status = -1;
  }
  else if (!S_ISDIR(st.st_mode)) {
    errno = ENOTDIR;
    status = -1;
  }

  return(status);
}

int mkpath(const char *path, mode_t mode) {
  char *pp;
  char *sp;
  int status;
  char *copypath = strdup(path);

  status = 0;
  pp = copypath;
  while (status == 0 && (sp = strchr(pp, '/')) != 0) {
    if (sp != pp) {  /* Neither root nor double slash in path */
      *sp = '\0';
      status = do_mkdir(copypath, mode);
      *sp = '/';
    }
    pp = sp + 1;
  }
  if (status == 0)
    status = do_mkdir(path, mode);
  free(copypath);
  return (status);
}

void write_tiff3d_RGB(char *prefname, tImage3d *imgr, tImage3d *imgg, tImage3d *imgb){
  int i;
  tImage im2dr,im2dg,im2db;
  im2dr.width = imgr->width;
  im2dr.height= imgr->height;
  im2dg.width = imgr->width;
  im2dg.height= imgr->height;
  im2db.width = imgr->width;
  im2db.height= imgr->height;
  char filename[255];
  for(i=0;i<imgr->depth;i++) {
    im2dr.bitmap= imgr->bitmap+imgr->width*imgr->height*i;
    im2dg.bitmap= imgg->bitmap+imgr->width*imgr->height*i;
    if(imgb==NULL){ 
      sprintf(filename,"%s%.04i.tiff",prefname,i);
      write_tiff_rgb(filename,&im2dr,&im2dg,NULL);
    } else {
      im2db.bitmap= imgb->bitmap+imgr->width*imgr->height*i;
      sprintf(filename,"%s%.04i.tiff",prefname,i);
      write_tiff_rgb(filename,&im2dr,&im2dg,&im2db);
    }
  }
}

void write_tiff3d(char *prefname, tImage3d *img){
  int i;
  tImage im2d;
  im2d.width = img->width;
  im2d.height= img->height;
  char filename[255];
  if(img->depth > 10000) {printf("Image very large %i, do you want this?\n",img->depth);exit(-1);}
  for(i=0;i<img->depth;i++) {
    im2d.bitmap= img->bitmap+img->width*img->height*i;
    sprintf(filename,"%s%.04i.tiff",prefname,i);
    write_tiff(filename,&im2d);
  }
}

void write_tiff3d_zy(char *prefname, tImage3d *img){
  int i,j,k;
  tImage im2d;
  im2d.width = img->height;
  im2d.height= img->depth;
  char filename[255];
  im2d.bitmap = (float*) malloc(sizeof(float)*im2d.height*im2d.width);
  if(img->width > 10000) {printf("Image very large %i, do you want this?\n",img->width);exit(-1);}
  for(i=0;i<img->width;i++) {
    for(j=0;j<img->height;j++) for(k=0;k<img->depth;k++) 
      im2d.bitmap[j+im2d.height*k] = img->bitmap[i + j*img->width + k*img->width*img->height];
    sprintf(filename,"%s%.04i.tiff",prefname,i);
    write_tiff(filename,&im2d);
  }
  free(im2d.bitmap);
}

void write_tiff3d_z(char *prefname, tImage3d *img){
  int i,j,k;
  tImage im2d;
  im2d.width = img->height;
  im2d.height= img->depth;
  char filename[255];
  im2d.bitmap = (float*) malloc(sizeof(float)*im2d.height*im2d.width);
  if(img->width > 10000) {printf("Image very large %i, do you want this?\n",img->width);exit(-1);}
  for(i=0;i<img->width;i++) {
    for(j=0;j<img->height;j++) for(k=0;k<img->depth;k++) 
      im2d.bitmap[j+im2d.width*k] = img->bitmap[i+j*img->width+k*img->width*img->height];
    sprintf(filename,"%s%.04i.tiff",prefname,i);
    write_tiff(filename,&im2d);
  }
  free(im2d.bitmap);
}

void write_tiff3d_RGB_zy(char *prefname, tImage3d *imgr, tImage3d *imgg,tImage3d *imgb){
  int i,j,k;
  char filename[255];
  tImage *im2dr=NULL; //red
  tImage *im2dg=NULL; //green
  tImage *im2db=NULL; //blue
  if(imgr != NULL) {
    im2dr=(tImage*) malloc(sizeof(tImage));
    im2dr->width = imgr->width;
    im2dr->height= imgr->depth;
    im2dr->bitmap = (float*) malloc(sizeof(float)*im2dr->height*im2dr->width);
  }  
  if(imgg != NULL) {
    im2dg=(tImage*) malloc(sizeof(tImage));
    im2dg->width = imgg->width;
    im2dg->height= imgg->depth;
    im2dg->bitmap = (float*) malloc(sizeof(float)*im2dg->height*im2dg->width);
  }  
  if(imgb != NULL) {
    im2db=(tImage*) malloc(sizeof(tImage));
    im2db->width = imgb->width;
    im2db->height= imgb->depth;
    im2db->bitmap = (float*) malloc(sizeof(float)*im2db->height*im2db->width);
  }  
  for(i=0;i<imgr->height;i++) {
    for(j=0;j<imgr->width;j++) for(k=0;k<imgr->depth;k++) { 
      if(im2dr != NULL) im2dr->bitmap[j+im2dr->width*k] = imgr->bitmap[j+i*imgr->width+k*imgr->width*imgr->height];
      if(im2dg != NULL) im2dg->bitmap[j+im2dg->width*k] = imgg->bitmap[j+i*imgg->width+k*imgg->width*imgg->height];
      if(im2db != NULL) im2db->bitmap[j+im2db->width*k] = imgb->bitmap[j+i*imgb->width+k*imgb->width*imgb->height];
    }  
    sprintf(filename,"%s%.04i.tiff",prefname,i);
    write_tiff_rgb(filename,im2dr,im2dg,im2db);
  }
  if(imgr!=NULL){
    free(im2dr->bitmap);
    free(im2dr);
  }  
  if(imgg!=NULL){
    free(im2dg->bitmap);
    free(im2dg);
  }
  if(imgb!=NULL){
    free(im2db->bitmap);
    free(im2db);
  }
}

void write_tiff3d_RGB_z(char *prefname, tImage3d *imgr, tImage3d *imgg,tImage3d *imgb){
  int i,j,k;
  char filename[255];
  tImage *im2dr=NULL; //red
  tImage *im2dg=NULL; //green
  tImage *im2db=NULL; //blue
  if(imgr != NULL) {
    im2dr=(tImage*) malloc(sizeof(tImage));
    im2dr->width = imgr->height;
    im2dr->height= imgr->depth;
    im2dr->bitmap = (float*) malloc(sizeof(float)*im2dr->height*im2dr->width);
  }  
  if(imgg != NULL) {
    im2dg=(tImage*) malloc(sizeof(tImage));
    im2dg->width = imgg->height;
    im2dg->height= imgg->depth;
    im2dg->bitmap = (float*) malloc(sizeof(float)*im2dg->height*im2dg->width);
  }  
  if(imgb != NULL) {
    im2db=(tImage*) malloc(sizeof(tImage));
    im2db->width = imgb->height;
    im2db->height= imgb->depth;
    im2db->bitmap = (float*) malloc(sizeof(float)*im2db->height*im2db->width);
  }  
  for(i=0;i<imgr->width;i++) {
    for(j=0;j<imgr->height;j++) for(k=0;k<imgr->depth;k++) { 
      if(im2dr != NULL) im2dr->bitmap[j+im2dr->width*k] = imgr->bitmap[i+j*imgr->width+k*imgr->width*imgr->height];
      if(im2dg != NULL) im2dg->bitmap[j+im2dg->width*k] = imgg->bitmap[i+j*imgg->width+k*imgg->width*imgg->height];
      if(im2db != NULL) im2db->bitmap[j+im2db->width*k] = imgb->bitmap[i+j*imgb->width+k*imgb->width*imgb->height];
    }  
    sprintf(filename,"%s%.04i.tiff",prefname,i);
    write_tiff_rgb(filename,im2dr,im2dg,im2db);
  }
  if(imgr!=NULL){
    free(im2dr->bitmap);
    free(im2dr);
  }  
  if(imgg!=NULL){
    free(im2dg->bitmap);
    free(im2dg);
  }
  if(imgb!=NULL){
    free(im2db->bitmap);
    free(im2db);
  }
}

//=====================
void read_tiff_xyz(char **filenames, int nfiles, tImage3d *img){
  int width=0,height=0,depth=0;
  int i,x,y;
  glob_t globbuf;
  for(i=0;i<nfiles;i++){
    if(i==0) {
      if(glob(filenames[i],GLOB_TILDE,NULL,&globbuf) != 0) {
        printf("Error reading %s \n",filenames[i]);
        exit(666);
      }  
    } else {
      if(glob(filenames[i],GLOB_TILDE | GLOB_APPEND,NULL,&globbuf) != 0) {
        printf("Error reading %s \n",filenames[i]);
        exit(666);
      }  
    }
  }
  depth=globbuf.gl_pathc;
  img->depth=depth;
  tImage_raw *raw = (tImage_raw*) malloc(sizeof(tImage_raw));
  if(globbuf.gl_pathc > 0){
    for(i=0;i<globbuf.gl_pathc;i++){
//      printf("Reading: %s \n",globbuf.gl_pathv[i]);
      read_tiff_raw(globbuf.gl_pathv[i],raw);
      if(i==0){
        width=raw->width; 
        height=raw->height;
        img->width=width;
        img->height=height;
        img->bitmap= (float*) malloc(sizeof(float)*depth*width*height);
      }
      if(raw->width != width || raw->height != height) {
        fprintf(stderr,"ERROR: Images not of the same size!\n");
        exit(666);
      }
      if(raw->bpp==1){
      for(y=0;y<height;y++) for(x=0;x<width;x++){
        *(img->bitmap + x + y*width + i*width*height)=((float) *(raw->bitmap+(x*raw->bpp+y*width*raw->bpp)))/(255.0);
      }
      } else if(raw->bpp==2)
        for(y=0;y<height;y++) for(x=0;x<width;x++){
          *(img->bitmap + x + y*width + i*width*height)=((float) *(raw->bitmap+(1+x*raw->bpp+y*width*raw->bpp)))/(255.0);
          *(img->bitmap + x + y*width + i*width*height)+=((float) *(raw->bitmap+(x*raw->bpp+y*width*raw->bpp)))/(65535.0);
        }  
      _TIFFfree(raw->bitmap);
    }
  }
  free(raw);
  globfree(&globbuf);
}

void read_tiff(char * filename, tImage *img){
  tImage_raw *raw = (tImage_raw*) malloc(sizeof(tImage_raw));
  read_tiff_raw(filename,raw);
  if(raw->bpp != 1) {
    fprintf(stderr,"Warning: Not a single channel image taking red.\n");
  }
  int i,j;
  img->height=raw->height;
  img->width=raw->width;
  img->bitmap = (float*) malloc(img->height*img->width*sizeof(float));
  for(j=0;j<raw->height;j++) for(i=0;i<raw->width;i++){
    *(img->bitmap + i + j*img->width)=((float) *(raw->bitmap+(i*raw->bpp+j*img->width*raw->bpp)))/255.0;
  }
  _TIFFfree(raw->bitmap);
  free(raw);
}

void read_tiff_stack(char * filename, tImage3d *img){
  tImage_raw *raw = (tImage_raw*) malloc(sizeof(tImage_raw));
  read_tiff_raw_stack(filename,raw);
  if(raw->bpp != 1) {
    fprintf(stderr,"Warning: Not a single channel image taking red.\n");
  }
  int i,j,k;
  img->height=raw->height;
  img->width=raw->width;
  img->depth=raw->depth;
  img->bitmap = (float*) malloc(img->height*img->width*img->depth*sizeof(float));
  for(j=0;j<raw->height;j++) for(i=0;i<raw->width;i++) for(k=0;k<raw->depth;k++){
    if(raw->bpp==1) {
      *(img->bitmap + i + j*img->width + k*img->width*img->height) = 
        ((float) *(raw->bitmap+(i + j*img->width + k*img->width*img->height)))/255.0;
    } else if(raw->bpp==2) {
      *(img->bitmap + i + j*img->width + k*img->width*img->height) = 
//        ((float) *(raw->bitmap+0+raw->bpp*(i + j*img->width + k*img->width*img->height)))/255.0/64.0+
//        ((float) (*(raw->bitmap+1+raw->bpp*(i + j*img->width + k*img->width*img->height)) & 63) )/64.0;
        ((float) *(raw->bitmap+0+raw->bpp*(i + j*img->width + k*img->width*img->height)))/255.0/64.0+
        ((float) (*(raw->bitmap+1+raw->bpp*(i + j*img->width + k*img->width*img->height)) & 127) )/128.0;
//      if( *(img->bitmap + i + j*img->width + k*img->width*img->height) > 0.9)
//        printf("%f\n",*(img->bitmap + i + j*img->width + k*img->width*img->height));
    }
  }
  _TIFFfree(raw->bitmap);
  free(raw);
}

void write_tiff(char *filename, tImage *img){
  tImage_raw *raw = (tImage_raw*) malloc(sizeof(tImage_raw));
  int i,j;
  raw->height=img->height;
  raw->width=img->width;
  raw->bpp=1;
  raw->bitmap = (unsigned char*) malloc(raw->height*raw->width*sizeof(char));
  for(j=0;j<raw->height;j++){
    for(i=0;i<raw->width;i++){
      *(raw->bitmap + i + j*img->width)=(unsigned char) (*(img->bitmap+i+j*img->width)*255.0);
    }
  }
  write_tiff_raw(filename,raw);
  _TIFFfree(raw->bitmap);
}

void write_tiff_rgb(char *filename, tImage *imgr, tImage *imgg, tImage *imgb){
  tImage_raw *raw = (tImage_raw*) malloc(sizeof(tImage_raw));
  int i,j;
  raw->height=imgr->height;
  raw->width=imgr->width;
  raw->bpp=3;
  raw->bitmap = (unsigned char*) malloc(raw->height*raw->width*sizeof(char)*3);
  for(j=0;j<raw->height;j++){
    for(i=0;i<raw->width;i++){
      *(raw->bitmap + (i + j*imgr->width)*3+0)=(unsigned char) (*(imgr->bitmap+i+j*imgr->width)*255.0);
      if(imgg) *(raw->bitmap + (i + j*imgr->width)*3+1)=(unsigned char) (*(imgg->bitmap+i+j*imgr->width)*255.0);
      else     *(raw->bitmap + (i + j*imgr->width)*3+1)=(unsigned char) 0;
      if(imgb) *(raw->bitmap + (i + j*imgr->width)*3+2)=(unsigned char) (*(imgb->bitmap+i+j*imgr->width)*255.0);
      else     *(raw->bitmap + (i + j*imgr->width)*3+2)=(unsigned char) 0;
    }
  }
  write_tiff_raw(filename,raw);
  _TIFFfree(raw->bitmap);
}

void read_tiff_rawargb(char * filename,tImage_raw *img){
  TIFF* tif = TIFFOpen(filename, "r");
  if (tif) {
    uint32 w, h;
    size_t npixels;

    if (TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w)==0 ) {
      fprintf(stderr,"Image does not have a width.\n");
      exit(42);
    }
    img->width=w;
    if (TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h)==0 ) {
      fprintf(stderr,"Image does not have a height.\n");
      exit(42);
    }
    img->height=h;
    npixels = w * h;
    img->bitmap = (unsigned char *) _TIFFmalloc(npixels * sizeof (uint32));
    img->bpp=4;
    if (img->bitmap != NULL) {
      if (!TIFFReadRGBAImage(tif, w, h, (uint32*) img->bitmap, 0)) {
        fprintf(stderr,"Error reading image: %s\n",filename);
        exit(42);
      }
    }
    TIFFClose(tif);
  } else {
    fprintf(stderr,"Error reading file %s \n",filename);
    exit(42);
  }
}

void read_tiff_raw_stack(char * filename,tImage_raw *img){ //single file image stack 
  TIFF* tif = TIFFOpen(filename, "r");
  if (tif) {
    uint32 w, h,d;
    uint16 bps, spp;

    if (TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w)==0 ) {
      fprintf(stderr,"ERROR: Image does not have a width.\n");
      exit(42);
    }
    img->width=w;
    if (TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h)==0 ) {
      fprintf(stderr,"ERROR: Image does not have a height.\n");
      exit(42);
    }
    img->height=h;
 
    d=0;
    while(!TIFFLastDirectory(tif)){
      d++;
      TIFFSetDirectory(tif,d);
    }
    d++;
    img->depth=d;

    if ((TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bps)==0) || !((bps == 8)||(bps == 16)) ) {
      fprintf(stderr,"ERROR: Can not read image with %u bits per sample!\n",bps);
      exit(42);
    }
    if ((TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp)==0)) {
      fprintf(stderr,"ERROR: Image does not have a valid number of samples per pixel.\n");
      exit(42);
    }
    img->bpp=bps*spp/8;

    unsigned int j;
    unsigned int scanlinesize = TIFFScanlineSize(tif);
    printf("%i %i %i %i %i\n",scanlinesize,h,d,bps,spp);
    img->bitmap = (unsigned char *) _TIFFmalloc(d*h*scanlinesize);
    for(j=0;j<img->depth;j++) {
      TIFFSetDirectory(tif,j);
      if (img->bitmap != NULL) {
        unsigned int i;
        for(i=0;i<h;i++){
          if (!TIFFReadScanline(tif,(img->bitmap+i*scanlinesize+j*scanlinesize*h),i,0) ) {
            fprintf(stderr,"Error reading image: %s\n",filename);
            exit(42);
          }
        }  
      }
    }  
    TIFFClose(tif);
  } else {
    fprintf(stderr,"Error reading file %s \n",filename);
    exit(42);
  }
}

void read_tiff_raw(char * filename,tImage_raw *img){
  TIFF* tif = TIFFOpen(filename, "r");
  if (tif) {
    uint32 w, h;
    uint16 bps, spp;

    if (TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w)==0 ) {
      fprintf(stderr,"ERROR: Image does not have a width.\n");
      exit(42);
    }
    img->width=w;
    if (TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h)==0 ) {
      fprintf(stderr,"ERROR: Image does not have a height.\n");
      exit(42);
    }
    img->height=h;
    if ((TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bps)==0) || !((bps == 8)||(bps == 16)) ) {
      fprintf(stderr,"ERROR: Can not read image with %u bits per sample!\n",bps);
      exit(42);
    }
    if ((TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp)==0)) {
      fprintf(stderr,"ERROR: Image does not have a valid number of samples per pixel.\n");
      exit(42);
    }
    img->bpp=bps*spp/8;
    
    unsigned int scanlinesize = TIFFScanlineSize(tif);
    img->bitmap = (unsigned char *) _TIFFmalloc(h*scanlinesize);
    if (img->bitmap != NULL) {
      unsigned int i;
      for(i=0;i<h;i++){
        if (!TIFFReadScanline(tif,(img->bitmap+i*scanlinesize),i,0) ) {
          fprintf(stderr,"Error reading image: %s\n",filename);
          exit(42);
        }
      }  
    }
    TIFFClose(tif);
  } else {
    fprintf(stderr,"Error reading file %s \n",filename);
    exit(42);
  }
}

void write_tiff_raw(char * filename, tImage_raw *img){
  TIFF* tif = TIFFOpen(filename, "w");
  if (tif){
    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, img->width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, img->height);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, img->bpp);

    TIFFSetField(tif, TIFFTAG_COMPRESSION, 1); //No compression
    uint16 photo;
    if(img->bpp==1) photo = 1; /*b&w*/ else photo=2; /*rgb*/
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

    // Write the information to the file
    int i;
    for(i=0;i<img->height;i++){
      TIFFWriteScanline(tif, (uint32*) &(img->bitmap[img->width*i*img->bpp]),i,0);
    }  
    TIFFClose(tif);
  } else {
    fprintf(stderr,"Error reading file %s \n",filename);
    exit(42);
  }
}
