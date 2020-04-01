#ifndef GRAPHICS_MODULE_H
#define GRAPHICS_MODULE_H

#define GLM_ENABLE_EXPERIMENTAL
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <algorithm>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
using namespace glm;
using namespace std;

// CPU representation of a particle
typedef struct Particle Particle;

class GraphicsModule{
private:
  glm::vec3 camera_position;
  int MaxParticles;
  float max_x;
  float max_y;
  float min_x;
  float min_y;
  float scale;
  float screen_ratio;
  float screen_scale;
  bool is_init;
  bool qr_enabled;
  Particle* ParticlesContainer;
  GLfloat* g_particule_position_size_data;
  GLfloat* g_particule_stage_data;
  GLuint particles_position_buffer;
  GLuint billboard_vertex_buffer;
  GLuint particles_stage_buffer;
  GLuint TextureID;
  GLuint programID;
  GLuint Texture;
  GLuint QRTexture;
  GLuint VertexArrayID;
  GLuint CameraRight_worldspace_ID;
  GLuint CameraUp_worldspace_ID;
  GLuint ViewProjMatrixID;

public:
  void SortParticles();
  GraphicsModule( int num_particles, int maxX, int maxY,
                  int screenScale, float beeSize,
                  const char* texture_fp,
                  const char* vertexshader_fp,
                  const char* fragmentshader_fp);
  int update_particles(vector<int> x, vector<int> y, vector<int> stage, vector<int> direction);
  void update_qr(bool enabled, const char* qrcode_fp);
  float to_opengl_world_x(int x);
  float to_opengl_world_y(int y);
  void update_display();
  void cleanup();
  bool should_close();
};

#endif
