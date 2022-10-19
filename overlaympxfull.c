//reads in quasi 3D coordinate files 

#include <stdio.h>
#include <tiffio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "tiff_io.h"


int min(int a,int b){
  return a<b?a:b;
}

int max(int a,int b){
  return a>b?a:b;
}

double sqr(double x){
  return x*x;
}


/**
 * Calculates the distance between a point and a line segment
 * Input:
 *   x,y,z the vector containing the coordinates of one end point of the line segment
 *   bx,by,bz the vector containing the other end points of the line segment
 *   The point is placed in the origin
 **/
float pointpline(float x, float y,float z, float bx, float by, float bz){                                                                 
  float lx=x-bx,ly=y-by,lz=z-bz;
  float dot=x*lx+y*ly+z*lz;
  float lengthsq=lx*lx+ly*ly+lz*lz;
  if(dot <= 0){
    return sqr(x) + sqr(y)+ sqr(z);
  }
  if(dot < lengthsq) {
    return sqr(x)+sqr(y)+sqr(z) - dot*dot/lengthsq;
  }
  return sqr(bx)+sqr(by)+sqr(bz);
}

void drawRod(tImage3d *img,float ax, float ay,float az,float bx,float by,float bz,float diameterp){                            
  int w=img->width,h=img->height,d=img->depth;
  int x,y,z;
  double distsq;
  int rc=(int) ceil(diameterp);
  for(x=max(min(ax-rc,bx-rc),0);x<w && x < max(ax+rc,bx+rc);x++)
  for(y=max(min(ay-rc,by-rc),0);y<h && y < max(ay+rc,by+rc);y++)
  for(z=max(min(az-rc,bz-rc),0);z<d && z < max(az+rc,bz+rc);z++) {
    distsq=pointpline((ax-x),(ay-y),(az-z),(bx-x),(by-y),(bz-z))/sqr(diameterp*0.5);
    if(distsq < 1.5){
      img->bitmap[x+y*w+z*w*h]+=exp(-sqr(1.0-distsq)*20.0); //<- this factor 20 determines the width of the line
    }
  }
}


void drawDoubleRod(tImage3d *img,float ax, float ay, float az,
                                 float bx, float by, float bz,
                                 float cx, float cy, float cz, float diameterp){ 
  int w=img->width,h=img->height,d=img->depth;
  int x,y,z;
  double distsq;
  for(x=0;x<w;x++)
  for(y=0;y<h;y++)
  for(z=0;z<d;z++) {
    double distsq1=pointpline((ax-x),(ay-y),(az-z),(bx-x),(by-y),(bz-z))/sqr(diameterp*0.5);
    double distsq2=pointpline((cx-x),(cy-y),(cz-z),(bx-x),(by-y),(bz-z))/sqr(diameterp*0.5);
    if (distsq1<distsq2) distsq=distsq1; else distsq=distsq2;
    if(distsq < 1.5){
      img->bitmap[x+y*w+z*w*h]+=exp(-sqr(1.0-distsq)*20.0); //<- this factor 20 determines the width of the line
    }
  }
}



int main(int argc, char *argv[]){

  tImage3d img;
  img.width=360;
  img.height=780;
  img.depth=2100;
  img.bitmap=malloc(sizeof(float)*img.width*img.height*img.depth);
  for (int i=1;i<img.width*img.height*img.depth;i++) {
    img.bitmap[i]=0.0;
  }

int i=0;
int j=0;

FILE *file;  /* declare a FILE pointer  */
file = fopen("Configuration1.dat", "r");  /* open a text file for reading */

double** matrix=malloc(40000*sizeof(double*)); 
for(i=0;i<40000;++i)
matrix[i]=malloc(8*sizeof(double));


 for(i = 0; i < 40000; i++)
  {
      for(j = 0; j < 8; j++) 
      { 
       if (!fscanf(file, "%lf", &matrix[i][j])) 
           break;
     
       //printf("%lf\n",matrix[i][j]);
      }

  }
fclose(file);

for(i = 1; i < 20000; i++)
  {
        drawDoubleRod(&img, 6*matrix[2*i][0]+3*matrix[2*i][3]*matrix[2*i][6] , 6*matrix[2*i][1]+3*matrix[2*i][4]*matrix[2*i][6], 6*matrix[2*i][2]+3*matrix[2*i][5]*matrix[2*i][6],
                      6*matrix[2*i+1][0]-3*matrix[2*i+1][3]*matrix[2*i+1][6],6*matrix[2*i][1]-3*matrix[2*i][4]*matrix[2*i][6], 6*matrix[2*i][2]-3*matrix[2*i][5]*matrix[2*i][6],
                      6*matrix[2*i+1][0]+3*matrix[2*i+1][3]*matrix[2*i+1][6], 6*matrix[2*i+1][1]+3*matrix[2*i+1][4]*matrix[2*i+1][6] , 6*matrix[2*i+1][2]+3*matrix[2*i+1][5]*matrix[2*i+1][6],6);

printf("%i\n",i);

  }




  write_tiff3d("SplayBenRod_",&img);
  free(img.bitmap);

  return 0;
}
