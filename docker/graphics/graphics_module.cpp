#define GLM_ENABLE_EXPERIMENTAL
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <algorithm>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
using namespace glm;
using namespace std;

#include "common/shader.hpp"
#include "common/texture.hpp"
#include "common/controls.hpp"
#include "graphics_module.hpp"

// CPU representation of a particle
typedef struct Particle{
	glm::vec3 pos, speed;
	float size;
	int stage;
	int movement_type;
	int direction;
	float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

	bool operator<(const Particle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}
} Particle;

void GraphicsModule::SortParticles(){
  std::sort(&ParticlesContainer[0], &ParticlesContainer[MaxParticles]);
}

int GraphicsModule::update_particles(vector<int> x, vector<int> y, vector<int> stage, vector<int> direction){
  if((int)x.size() != MaxParticles || \
     (int)y.size() != MaxParticles || \
     (int)stage.size() != MaxParticles || \
     (int)direction.size() != MaxParticles){
    return -1;
  }

  for(int i = 0; i < MaxParticles; i++){
    ParticlesContainer[i].pos = glm::vec3(x.at(i), y.at(i),
                                          (rand()%2000-1000.0f)/500.0f);
    ParticlesContainer[i].stage = stage.at(i);
    ParticlesContainer[i].direction = direction.at(i);
  }
  return 0;
}

int GraphicsModule::update_particles(vector<Particle> new_particles){
  return 0;
}

GraphicsModule::GraphicsModule(int num_particles, int MaxX, int MaxY, int MinX, int MinY){
  camera_position = glm::vec3(0, 0, 20);
  MaxParticles = num_particles;
  ParticlesContainer = (Particle*)malloc(sizeof(Particle)*MaxParticles);

  max_x = MaxX; max_y = MaxY; min_x = MinX; min_y = MinY;

  // Initialise GLFW
  if( !glfwInit() )
  {
    fprintf( stderr, "Failed to initialize GLFW\n" );
    getchar();
    return;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open a window and create its OpenGL context
  window = glfwCreateWindow( 1024, 768, "Tutorial 18 - Particules", NULL, NULL);
  if( window == NULL ){
    fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
    getchar();
    glfwTerminate();
    return;
  }
  glfwMakeContextCurrent(window);

  // Initialize GLEW
  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
    return;
  }

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  // Hide the mouse and enable unlimited mouvement
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // black background
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);

  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  // Create and compile our GLSL program from the shaders
  programID = LoadShaders( "Particle.vertexshader", "Particle.fragmentshader" );

  //load our texture
  Texture = loadPNG("up2.png");

  // Vertex shader
  CameraRight_worldspace_ID  = glGetUniformLocation(programID, "CameraRight_worldspace");
  CameraUp_worldspace_ID  = glGetUniformLocation(programID, "CameraUp_worldspace");
  ViewProjMatrixID = glGetUniformLocation(programID, "VP");

  // fragment shader
  TextureID  = glGetUniformLocation(programID, "myTextureSampler");

  g_particule_position_size_data = new GLfloat[MaxParticles * 4];
  g_particule_stage_data = new GLfloat[MaxParticles * 2];

  int pos_spread = 10;
  for(int i=0; i<MaxParticles; i++){
    ParticlesContainer[i].pos = glm::vec3((rand()%2000-1000.0f)*pos_spread/1000.0f,
                                          (rand()%2000-1000.0f)*pos_spread/1000.0f,
                                          (rand()%2000-1000.0f)/500.0f);
    glm::vec3 randomdir = glm::vec3(
      (rand()%2000 - 1000.0f)/1000.0f,
      (rand()%2000 - 1000.0f)/1000.0f,
      (rand()%2000 - 1000.0f)/1000.0f
    );

    ParticlesContainer[i].speed = randomdir;
    ParticlesContainer[i].stage = 0;
    ParticlesContainer[i].size = 1.0f;

    //set the camera distance to current position - where camera is placed
    ParticlesContainer[i].cameradistance = glm::length2( ParticlesContainer[i].pos -  camera_position);
  }

  // The VBO containing the 4 vertices of the particles.
  // Thanks to instancing, they will be shared by all particles.
  static const GLfloat g_vertex_buffer_data[] = {
     -0.5f, -0.5f, 0.0f,
      0.5f, -0.5f, 0.0f,
     -0.5f,  0.5f, 0.0f,
      0.5f,  0.5f, 0.0f,
  };
  glGenBuffers(1, &billboard_vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

  // The VBO containing the information (position, size) of the particles
  glGenBuffers(1, &particles_position_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);

  // Initialize with empty (NULL) buffer : it will be updated later, each frame.
  glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

  // The VBO containing the information about stages of each of the particles
  glGenBuffers(1, &particles_stage_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, particles_stage_buffer);

  // Initialize with empty (NULL) buffer : it will be updated later, each frame.
  glBufferData(GL_ARRAY_BUFFER, MaxParticles * 2 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
}

void GraphicsModule::update_display(){
  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Camera matrix
  glm::mat4 ViewMatrix       = glm::lookAt(
                camera_position, // Camera is at (4,3,3), in World Space
                glm::vec3(0,0,0), // and looks at the origin
                glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
               );
  glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

  // We will need the camera's position in order to sort the particles
  // w.r.t the camera's distance.
  // There should be a getCameraPosition() function in common/controls.cpp,
  // but this works too.
  glm::vec3 CameraPosition(glm::inverse(ViewMatrix)[3]);
  glm::mat4 ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

  // Simulate all particles
  for(int i=0; i<MaxParticles; i++){

    Particle& p = ParticlesContainer[i]; // shortcut

    //toroidal boonds
    if(p.pos.x > max_x)
      p.pos.x = min_x;
    if(p.pos.x < min_x)
      p.pos.x = max_x;

    if(p.pos.y > max_y)
      p.pos.y = min_y;
    if(p.pos.y < min_y)
      p.pos.y = max_y;

    p.cameradistance = glm::length2( p.pos - CameraPosition );

    // Fill the GPU buffer of positions
    g_particule_position_size_data[4*i+0] = p.pos.x;
    g_particule_position_size_data[4*i+1] = p.pos.y;
    g_particule_position_size_data[4*i+2] = p.pos.z;

    g_particule_position_size_data[4*i+3] = p.size;

    //fill the GPU buffer with stage information
    g_particule_stage_data[2*i+0] = p.stage;
    g_particule_stage_data[2*i+1] = p.direction;

  }

  SortParticles();

  // Update the buffers that OpenGL uses for rendering.
  // There are much more sophisticated means to stream data from the CPU to the GPU,
  // but this is outside the scope of this tutorial.
  // http://www.opengl.org/wiki/Buffer_Object_Streaming
  glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
  glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
  glBufferSubData(GL_ARRAY_BUFFER, 0, MaxParticles * sizeof(GLfloat) * 4, g_particule_position_size_data);

  glBindBuffer(GL_ARRAY_BUFFER, particles_stage_buffer);
  glBufferData(GL_ARRAY_BUFFER, MaxParticles * 2 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
  glBufferSubData(GL_ARRAY_BUFFER, 0, MaxParticles * sizeof(GLfloat) * 2, g_particule_stage_data);


  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Use our shader
  glUseProgram(programID);

  // Bind our texture in Texture Unit 0
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, Texture);
  // Set our "myTextureSampler" sampler to use Texture Unit 0
  glUniform1i(TextureID, 0);

  // Same as the billboards tutorial
  glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
  glUniform3f(CameraUp_worldspace_ID   , ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);

  glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

  // 1rst attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
  glVertexAttribPointer(
    0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );

  // 2nd attribute buffer : positions of particles' centers
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
  glVertexAttribPointer(
    1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
    4,                                // size : x + y + z + size => 4
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
  );

  //3rd attribute buffer : information about particle stage
  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, particles_stage_buffer);
  glVertexAttribPointer(
    2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
    2,                                // size : x + y + z + size => 4
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
  );

  // These functions are specific to glDrawArrays*Instanced*.
  // The first parameter is the attribute buffer we're talking about.
  // The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
  // http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
  glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
  glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
  glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

  // Draw the particules !
  // This draws many times a small triangle_strip (which looks like a quad).
  // This is equivalent to :
  // for(i in MaxParticles) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4),
  // but faster.
  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, MaxParticles);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);

  // Swap buffers
  glfwSwapBuffers(window);
  glfwPollEvents();
}

void GraphicsModule::cleanup(){
  free(ParticlesContainer);

  delete[] g_particule_position_size_data;

  // Cleanup VBO and shader
  glDeleteBuffers(1, &particles_position_buffer);
  glDeleteBuffers(1, &particles_stage_buffer);
  glDeleteBuffers(1, &billboard_vertex_buffer);
  glDeleteProgram(programID);
  glDeleteTextures(1, &Texture);
  glDeleteVertexArrays(1, &VertexArrayID);

  // Close OpenGL window and terminate GLFW
  glfwTerminate();
}

bool GraphicsModule::should_close(){
	return !(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		     glfwWindowShouldClose(window) == 0 );
}
