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


/*texdemo.c*/
/*Provided by Dr. Palmer*/

typedef struct {
  float Position[2];
  float TexCoord[2];
} Vertex;

// (-1, 1)  (1, 1)
// (-1, -1) (1, -1)

Vertex vertexes[] = {
  {{-1, 1}, {0, 0}},
  {{1, 1},  {1, 0}},
  {{-1, -1},  {0, 1}},

  {{1, 1}, {1, 0}},
  {{1, -1},  {1, 1}},
  {{-1, -1},  {0, 1}}
};

//identifiers
static const char* vertex_shader_text =
"uniform mat4 MVP;\n"
"attribute vec2 TexCoordIn;\n"
"attribute vec2 vPos;\n"
"varying vec2 TexCoordOut;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    TexCoordOut = TexCoordIn;\n"
"}\n";

static const char* fragment_shader_text =
"varying lowp vec2 TexCoordOut;\n"
"uniform sampler2D Texture;\n"
"void main()\n"
"{\n"
"    gl_FragColor = texture2D(Texture, TexCoordOut);\n"
"}\n";

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //If user presses the ESC key, the window will terminate
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, 1);}
    //Rotate left
    else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS){
      rotate += PI/3;
      }
    //Rotate right
    else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
      rotate -= PI/3;
      }
    //Scale up
    else if (key == GLFW_KEY_UP && action == GLFW_PRESS){
      scale += 0.5;
      }
    //Scale down
    else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS){
      scale -= 0.5;
      }
    //Shear negative
    else if (key == GLFW_KEY_Q && action == GLFW_PRESS){
      shear -= 0.5;
      }
    //Shear positive
    else if (key == GLFW_KEY_E && action == GLFW_PRESS){
      shear += 0.5;
      }
    //Translate left
    else if (key == GLFW_KEY_A && action == GLFW_PRESS){
      xTrans -= 0.5;
      }
    //Translate right
    else if (key == GLFW_KEY_D && action == GLFW_PRESS){
      xTrans += 0.5;
      }
}

void glCompileShaderOrDie(GLuint shader) {
  GLint compiled;
  glCompileShader(shader);
  glGetShaderiv(shader,
		GL_COMPILE_STATUS,
		&compiled);
  if (!compiled) {
    GLint infoLen = 0;
    glGetShaderiv(shader,
		  GL_INFO_LOG_LENGTH,
		  &infoLen);
    char* info = malloc(infoLen+1);
    GLint done;
    glGetShaderInfoLog(shader, infoLen, &done, info);
    printf("Unable to compile shader: %s\n", info);
    exit(1);
  }
}

int main(int argc, char *argv[])
{
  GLFWwindow* window;
  GLuint vertex_buffer, vertex_shader, fragment_shader, program, index;
  GLint mvp_location, vpos_location, vcol_location;
  glfwSetErrorCallback(error_callback);

  //Checks if the user has provided the correct command line arguments
  if(argc != 2){
    fprintf(stderr, "ERROR! Enter the ppm file name with correct extension... \n");
    exit(1);
  }
  char *filename = argv[1];
  read_scene(filename);

  if (!glfwInit())
      exit(EXIT_FAILURE);

      glfwDefaultWindowHints();
      glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
      glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  window = glfwCreateWindow(300, 300, "EZ VIEW", NULL, NULL);
  if (!window)
  {
      glfwTerminate();
      exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window, key_callback);

  glfwMakeContextCurrent(window);
  // gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
  glfwSwapInterval(1);

  // NOTE: OpenGL error checks have been omitted for brevity

  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertexes), vertexes, GL_STATIC_DRAW);

  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
  glCompileShaderOrDie(vertex_shader);

  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
  glCompileShaderOrDie(fragment_shader);

  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  // more error checking! glLinkProgramOrDie!

  mvp_location = glGetUniformLocation(program, "MVP");
  assert(mvp_location != -1);

  vpos_location = glGetAttribLocation(program, "vPos");
  assert(vpos_location != -1);

  GLint texcoord_location = glGetAttribLocation(program, "TexCoordIn");
  assert(texcoord_location != -1);

  GLint tex_location = glGetUniformLocation(program, "Texture");
  assert(tex_location != -1);

  glEnableVertexAttribArray(vpos_location);
  glVertexAttribPointer(vpos_location,
  	  2,
  	  GL_FLOAT,
  	  GL_FALSE,
      sizeof(Vertex),
  	  (void*) 0);

  glEnableVertexAttribArray(texcoord_location);
  glVertexAttribPointer(texcoord_location,
  	  2,
  	  GL_FLOAT,
  	  GL_FALSE,
      sizeof(Vertex),
  	  (void*) (sizeof(float) * 2));

  GLuint texID;
  glGenTextures(1, &texID);
  glBindTexture(GL_TEXTURE_2D, texID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pixelImg->width, pixelImg->height, 0, GL_RGB,
   GL_UNSIGNED_BYTE, pixelImg->PixelBuffer);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texID);
  glUniform1i(tex_location, 0);

  while (!glfwWindowShouldClose(window))
  {
      float ratio;
      int width, height;
      mat4x4 m, p, mvp;
      mat4x4 rotation, translation, scaling, shearing, arm;

      glfwGetFramebufferSize(window, &width, &height);
      ratio = width / (float) height;

      glViewport(0, 0, width, height);
      glClear(GL_COLOR_BUFFER_BIT);

      //Creates an identity matrix
      //for our affine transformation
      mat4x4_identity(m);
      mat4x4_identity(rotation);
      mat4x4_identity(translation);
      mat4x4_identity(scaling);
      mat4x4_identity(shearing);

      //Acquiring the data for transformation
      mat4x4_rotate_Z(rotation, rotation, rotate);
      mat4x4_translate(translation,xTrans,yTrans,zTrans);
      mat4x4_scale_aniso(scaling, scaling, scale, scale, scale);
      mat4x4_shear(shearing, shearing, shear, shear);
      mat4x4_add(m,translation,m);
      mat4x4_add(m,scaling,m);
      mat4x4_add(m,shearing,m);
      mat4x4_mul(m,rotation,m);

      mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
      mat4x4_mul(mvp, p, m);
      /*
      mat4x4_identity(m);
      mat4x4_rotate_Z(m, m, (float) glfwGetTime());
      mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
      mat4x4_mul(mvp, p, m);
      */

      glUseProgram(program);
      glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
      glDrawArrays(GL_TRIANGLES, 0, 6);

      glfwSwapBuffers(window);
      glfwPollEvents();
  }

  glfwDestroyWindow(window);

  glfwTerminate();
  exit(EXIT_SUCCESS);
}
