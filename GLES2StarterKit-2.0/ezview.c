/*Header files and dependencies */
#define GLFW_DLL 1
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <GLFW/glfw3.h>
#include "linmath.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

//John E. Sadie
//CS430 Computer Graphics
//Project 5 - Image Viewer
//12.8.2016

/*
Description:
In this project you will create an image viewer. Your image viewer should be able to load PPM
images in P3 or P6 format and display the image in a window. You should also have mouse
and/or keyboard controls to perform all the affine transformations on the image:
• Translate	(pan)
• Rotate
• Scale
• Shear
*/

typedef struct{
    unsigned char r,g,b;
}pixelData;

typedef struct Scene{
    int width, height;
    int File;
    pixelData *PixelBuffer;
}Scene;

Scene *pixelImg;

/* Instantiating variables for affine transformation */
float rotate = 0;
float scale = 1;
float xTrans = 0;
float yTrans = 0;
float zTrans = 0;
float shear = 0;
float AspectRatio = 0;


#define PI 3.14
#define Color 255

//Function: read_scene
//reads and checks our ppm file
int read_scene(char* filename){
    int c;
    int width, height;
    int max;

    FILE *fh;
    fh = fopen(filename, "r");
    if(fh == 0){
       fh = fopen(filename, "rb");
    }
    /*Error Checking*/
    if(fh == 0){
        fclose(fh);
        fprintf(stderr, "ERROR! The file: '%s' does not exist...\n", filename);
        exit(1);
    }
    c = fgetc(fh);
    if (c!= 'P'){
        fprintf(stderr, "ERROR! Not a ppm file...\n");
        exit(1);
    }
    ungetc(c, fh);
    //Allocating memory
     pixelImg = (Scene*)malloc(sizeof(Scene));
    if(pixelImg == 0){
        fprintf(stderr, "ERROR! Can't allocate memory... \n");
        exit(1);
    }
    c = fgetc(fh);
    c = fgetc(fh);
    pixelImg->File = c;
    //Skipping the magic number
    while((c = fgetc(fh)) != '\n'){
    }
    //Parsing past the comments
    c = fgetc(fh);
    while (c=='#'){
        while (fgetc(fh) != '\n');
            c=fgetc(fh);
    }
    ungetc(c, fh);
    //Get input size
    if(fscanf(fh, "%d %d", &pixelImg->width, &pixelImg->height) !=2){
        fprintf(stderr, "ERROR! Image dimensions is missing in the file...");
        exit(1);
    }
    width = pixelImg->width;
    height = pixelImg->height;
    //Ensures correct color
    fscanf(fh, "%d", &max);
    if(max != Color){
        fprintf(stderr, "ERROR! Formatted color incorrect...");
        exit(1);
    }
    //Allocating memory
    pixelImg->PixelBuffer = (pixelData*)malloc((Color+1)*width*height);
    fgetc(fh);
    //Accounting for a P6 ppm file
    if(pixelImg->File == '6'){
        fread(pixelImg->PixelBuffer, (sizeof(&pixelImg->PixelBuffer)), height*width, fh);
    }
    fclose(fh);
    return 0;
}

int main(int argc, char *argv[]){
    int c;

    pixelImage *image;
      //Check if arguments are not equal to 4
       if(argc != 4){
        fprintf(stderr, "ERROR\n");
        return 1;
    }
    FILE *fp;
    fp = fopen(inputType, "r");
    if(fp == 0){
        fp = fopen(inputType, "rb");
    }
    fgetc(fp);
    c = fgetc(fp);
    ungetc(c, fp);


}
