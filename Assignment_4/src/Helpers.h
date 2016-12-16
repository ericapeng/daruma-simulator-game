#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <vector>
#include <Eigen/Core>
#include <SOIL.h>

#ifdef _WIN32
#  include <windows.h>
#  undef max
#  undef min
#  undef DrawText
#endif

#ifndef __APPLE__
#  define GLEW_STATIC
#  include <GL/glew.h>
#endif

#ifdef __APPLE__
#   include <OpenGL/gl3.h>
#   define __gl_h_ /* Prevent inclusion of the old gl.h */
#else
#   ifdef _WIN32
#       include <windows.h>
#   endif
#   include <GL/gl.h>
#endif

class VertexArrayObject
{
public:
    unsigned int id;

    VertexArrayObject() : id(0) {}

    // Create a new VAO
    void init();

    // Select this VAO for subsequent draw calls
    void bind();

    // Release the id
    void free();
};

class VertexBufferObject
{
public:
    typedef unsigned int GLuint;
    typedef int GLint;

    GLuint id;
    GLuint rows;
    GLuint cols;

    VertexBufferObject() : id(0), rows(0), cols(0) {}

    // Create a new empty VBO
    void init();

    // Updates the VBO with a matrix M
    void update(const Eigen::MatrixXf& M);

    // Select this VBO for subsequent draw calls
    void bind();

    // Release the id
    void free();
};

// This class wraps an OpenGL program composed of two shaders
class Program
{
public:
  typedef unsigned int GLuint;
  typedef int GLint;

  GLuint vertex_shader;
  GLuint fragment_shader;
  GLuint program_shader;

  Program() : vertex_shader(0), fragment_shader(0), program_shader(0) { }

  // Create a new shader from the specified source strings
  bool init(const std::string &vertex_shader_string,
  const std::string &fragment_shader_string,
  const std::string &fragment_data_name);

  // Select this shader for subsequent draw calls
  void bind();

  // Release all OpenGL objects
  void free();

  // Return the OpenGL handle of a named shader attribute (-1 if it does not exist)
  GLint attrib(const std::string &name) const;

  // Return the OpenGL handle of a uniform attribute (-1 if it does not exist)
  GLint uniform(const std::string &name) const;

  // Bind a per-vertex array attribute
  GLint bindVertexAttribArray(const std::string &name, VertexBufferObject& VBO) const;

  GLuint create_shader_helper(GLint type, const std::string &shader_string);

};

// From: https://blog.nobel-joergensen.com/2013/01/29/debugging-opengl-using-glgeterror/
void _check_gl_error(const char *file, int line);

///
/// Usage
/// [... some opengl calls]
/// glCheckError();
///
#define check_gl_error() _check_gl_error(__FILE__,__LINE__)

//convert eigen matrix to dynamic array to bind the uniform
void update_pointer(float* M_p, Eigen::MatrixXf M);

class MeshObject
{
public:
    MeshObject(Eigen::MatrixXf V,
               Eigen::MatrixXf TC,
               Eigen::MatrixXf N,
               Eigen::MatrixXf F,
               Eigen::MatrixXf FTC,
               Eigen::MatrixXf FN);
    
    Eigen::MatrixXf V;
    Eigen::MatrixXf TC;
    Eigen::MatrixXf N;
    Eigen::MatrixXf F;
    Eigen::MatrixXf FTC;
    Eigen::MatrixXf FN;
    VertexBufferObject* VBO;
    VertexBufferObject* TCBO;
    Eigen::MatrixXf VFull;
    
    int textured;
    
private:
    Eigen::MatrixXf trianglify(Eigen::MatrixXf& M, Eigen::MatrixXf& Verts);
};

//ALL CODE BENEATH THIS LINE IS ADAPTED FROM libigl
#define IGL_INLINE inline
namespace igl
{
    template <typename Scalar, typename Index>
    bool readOBJ(
                            const std::string obj_file_name, int pass,
                            std::vector<std::vector<Scalar > > & V,
                            std::vector<std::vector<Scalar > > & TC,
                            std::vector<std::vector<Scalar > > & N,
                            std::vector<std::vector<Index > > & F,
                            std::vector<std::vector<Index > > & FTC,
                            std::vector<std::vector<Index > > & FN);
    
    template <typename DerivedV, typename DerivedF, typename DerivedT>
    bool readOBJ(
                            const std::string str, int pass,
                            Eigen::PlainObjectBase<DerivedV>& V,
                            Eigen::PlainObjectBase<DerivedT>& TC,
                            Eigen::PlainObjectBase<DerivedV>& CN,
                            Eigen::PlainObjectBase<DerivedF>& F,
                            Eigen::PlainObjectBase<DerivedF>& FTC,
                            Eigen::PlainObjectBase<DerivedF>& FN);

    template <typename T, typename Derived>
    IGL_INLINE bool list_to_matrix(
                                   const std::vector<std::vector<T > > & V,
                                   Eigen::PlainObjectBase<Derived>& M);
    
    template <typename T>
    IGL_INLINE int min_size(const std::vector<T> & V);

    template <typename T>
    IGL_INLINE int max_size(const std::vector<T> & V);

}


#endif
