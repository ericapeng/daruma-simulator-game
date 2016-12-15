#include "Helpers.h"

#include <iostream>
#include <fstream>
#include <cstdio>

#include <cassert>
#include <Eigen/Dense>

void VertexArrayObject::init()
{
  glGenVertexArrays(1, &id);
  check_gl_error();
}

void VertexArrayObject::bind()
{
  glBindVertexArray(id);
  check_gl_error();
}

void VertexArrayObject::free()
{
  glDeleteVertexArrays(1, &id);
  check_gl_error();
}

void VertexBufferObject::init()
{
  glGenBuffers(1,&id);
  check_gl_error();
}

void VertexBufferObject::bind()
{
  glBindBuffer(GL_ARRAY_BUFFER,id);
  check_gl_error();
}

void VertexBufferObject::free()
{
  glDeleteBuffers(1,&id);
  check_gl_error();
}

void VertexBufferObject::update(const Eigen::MatrixXf& M)
{
  assert(id != 0);
  glBindBuffer(GL_ARRAY_BUFFER, id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*M.size(), M.data(), GL_DYNAMIC_DRAW);
  rows = M.rows();
  cols = M.cols();
  check_gl_error();
}

bool Program::init(
  const std::string &vertex_shader_string,
  const std::string &fragment_shader_string,
  const std::string &fragment_data_name)
{
  using namespace std;
  vertex_shader = create_shader_helper(GL_VERTEX_SHADER, vertex_shader_string);
  fragment_shader = create_shader_helper(GL_FRAGMENT_SHADER, fragment_shader_string);

  if (!vertex_shader || !fragment_shader)
    return false;

  program_shader = glCreateProgram();

  glAttachShader(program_shader, vertex_shader);
  glAttachShader(program_shader, fragment_shader);

  glBindFragDataLocation(program_shader, 0, fragment_data_name.c_str());
  glLinkProgram(program_shader);

  GLint status;
  glGetProgramiv(program_shader, GL_LINK_STATUS, &status);

  if (status != GL_TRUE)
  {
    char buffer[512];
    glGetProgramInfoLog(program_shader, 512, NULL, buffer);
    cerr << "Linker error: " << endl << buffer << endl;
    program_shader = 0;
    return false;
  }

  check_gl_error();
  return true;
}

void Program::bind()
{
  glUseProgram(program_shader);
  check_gl_error();
}

GLint Program::attrib(const std::string &name) const
{
  return glGetAttribLocation(program_shader, name.c_str());
}

GLint Program::uniform(const std::string &name) const
{
  return glGetUniformLocation(program_shader, name.c_str());
}

GLint Program::bindVertexAttribArray(
        const std::string &name, VertexBufferObject& VBO) const
{
  GLint id = attrib(name);
  if (id < 0)
    return id;
  if (VBO.id == 0)
  {
    glDisableVertexAttribArray(id);
    return id;
  }
  VBO.bind();
  glEnableVertexAttribArray(id);
  glVertexAttribPointer(id, VBO.rows, GL_FLOAT, GL_FALSE, 0, 0);
  check_gl_error();

  return id;
}

void Program::free()
{
  if (program_shader)
  {
    glDeleteProgram(program_shader);
    program_shader = 0;
  }
  if (vertex_shader)
  {
    glDeleteShader(vertex_shader);
    vertex_shader = 0;
  }
  if (fragment_shader)
  {
    glDeleteShader(fragment_shader);
    fragment_shader = 0;
  }
  check_gl_error();
}

GLuint Program::create_shader_helper(GLint type, const std::string &shader_string)
{
  using namespace std;
  if (shader_string.empty())
    return (GLuint) 0;

  GLuint id = glCreateShader(type);
  const char *shader_string_const = shader_string.c_str();
  glShaderSource(id, 1, &shader_string_const, NULL);
  glCompileShader(id);

  GLint status;
  glGetShaderiv(id, GL_COMPILE_STATUS, &status);

  if (status != GL_TRUE)
  {
    char buffer[512];
    if (type == GL_VERTEX_SHADER)
      cerr << "Vertex shader:" << endl;
    else if (type == GL_FRAGMENT_SHADER)
      cerr << "Fragment shader:" << endl;
    else if (type == GL_GEOMETRY_SHADER)
      cerr << "Geometry shader:" << endl;
    cerr << shader_string << endl << endl;
    glGetShaderInfoLog(id, 512, NULL, buffer);
    cerr << "Error: " << endl << buffer << endl;
    return (GLuint) 0;
  }
  check_gl_error();

  return id;
}

void _check_gl_error(const char *file, int line)
{
  GLenum err (glGetError());

  while(err!=GL_NO_ERROR)
  {
    std::string error;

    switch(err)
    {
      case GL_INVALID_OPERATION:      error="INVALID_OPERATION";      break;
      case GL_INVALID_ENUM:           error="INVALID_ENUM";           break;
      case GL_INVALID_VALUE:          error="INVALID_VALUE";          break;
      case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";          break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:  error="INVALID_FRAMEBUFFER_OPERATION";  break;
    }

    std::cerr << "GL_" << error.c_str() << " - " << file << ":" << line << std::endl;
    err = glGetError();
  }
}



void update_pointer(float* M_p, Eigen::MatrixXf M){
    for(int i = 0; i < 4; i++){
        M_p[i] = M.col(i).x();
        M_p[i+4] = M.col(i).y();
        M_p[i+8] = M.col(i).z();
        M_p[i+12] = M.col(i).w();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
//MESHOBJECTS IMPLEMENTED METHODS
//
////////////////////////////////////////////////////////////////////////////////////////////////

MeshObject::MeshObject(Eigen::MatrixXf V, Eigen::MatrixXf TC, Eigen::MatrixXf N, Eigen::MatrixXf F, Eigen::MatrixXf FTC, Eigen::MatrixXf FN) : V(V), TC(TC), N(N), F(F), FTC(FTC), FN(FN)
{
    VBO = new VertexBufferObject();
    VBO->init();
    VBO->update(V);
}



////////////////////////////////////////////////////////////////////////////////////////////////
//
//ALL CODE BENEATH THIS LINE IS ADAPTED FROM libigl
//
////////////////////////////////////////////////////////////////////////////////////////////////

template <typename TestClass>
bool igl::test(TestClass obj) {
    std::cout << obj << "\n";
    return true;
}
#define IGL_INLINE inline
template <typename Scalar, typename Index>
bool igl::readOBJ(
                             const std::string obj_file_name, int pass,
                             std::vector<std::vector<Scalar > > & V,
                             std::vector<std::vector<Scalar > > & TC,
                             std::vector<std::vector<Scalar > > & N,
                             std::vector<std::vector<Index > > & F,
                             std::vector<std::vector<Index > > & FTC,
                             std::vector<std::vector<Index > > & FN)
{
    // Open file, and check for error
    FILE * obj_file = fopen(obj_file_name.c_str(),"r");
    if(NULL==obj_file)
    {
        fprintf(stderr,"IOError: %s could not be opened...\n",
                obj_file_name.c_str());
        return false;
    }
    
    // File open was succesfull so clear outputs
    V.clear();
    TC.clear();
    N.clear();
    F.clear();
    FTC.clear();
    FN.clear();
    
    
    // variables an constants to assist parsing the .obj file
    // Constant strings to compare against
    std::string v("v");
    std::string vn("vn");
    std::string vt("vt");
    std::string f("f");
    std::string tic_tac_toe("#");
    
#ifndef IGL_LINE_MAX
#  define IGL_LINE_MAX 2048
#endif
    
    int currPass = -1;
    std::string lastType = f;
    
    char line[IGL_LINE_MAX];
    int line_no = 1;
    while (fgets(line, IGL_LINE_MAX, obj_file) != NULL)
    {
        char type[IGL_LINE_MAX];

        // Read first word containing type
        if(sscanf(line, "%s",type) == 1)
        {
            if(type == v && lastType == f)
            {
                currPass++;
                lastType = v;
            }
            if(type == f && lastType == v)
            {
                lastType = f;
            }
            
            if(currPass == pass){
                // Get pointer to rest of line right after type
                char * l = &line[strlen(type)];
                if(type == v)
                {
                    double x[4];
                    int count =
                    sscanf(l,"%lf %lf %lf %lf\n",&x[0],&x[1],&x[2],&x[3]);
                    if(count != 3 && count != 4)
                    {
                        fprintf(stderr,
                                "Error: readOBJ() vertex on line %d should have 3 or 4 coordinates",
                                line_no);
                        fclose(obj_file);
                        return false;
                    }
                    std::vector<Scalar > vertex(count);
                    for(int i = 0;i<count;i++)
                    {
                        vertex[i] = x[i];
                    }
                    V.push_back(vertex);
                }else if(type == vn)
                {
                    double x[3];
                    int count =
                    sscanf(l,"%lf %lf %lf\n",&x[0],&x[1],&x[2]);
                    if(count != 3)
                    {
                        fprintf(stderr,
                                "Error: readOBJ() normal on line %d should have 3 coordinates",
                                line_no);
                        fclose(obj_file);
                        return false;
                    }
                    std::vector<Scalar > normal(count);
                    for(int i = 0;i<count;i++)
                    {
                        normal[i] = x[i];
                    }
                    N.push_back(normal);
                }else if(type == vt)
                {
                    double x[3];
                    int count =
                    sscanf(l,"%lf %lf %lf\n",&x[0],&x[1],&x[2]);
                    if(count != 2 && count != 3)
                    {
                        fprintf(stderr,
                                "Error: readOBJ() texture coords on line %d should have 2 "
                                "or 3 coordinates (%d)",
                                line_no,count);
                        fclose(obj_file);
                        return false;
                    }
                    std::vector<Scalar > tex(count);
                    for(int i = 0;i<count;i++)
                    {
                        tex[i] = x[i];
                    }
                    TC.push_back(tex);
                }else if(type == f)
                {
                    const auto & shift = [&V](const int i)->int
                    {
                        return i<0 ? i+V.size() : i-1;
                    };
                    const auto & shift_t = [&TC](const int i)->int
                    {
                        return i<0 ? i+TC.size() : i-1;
                    };
                    const auto & shift_n = [&N](const int i)->int
                    {
                        return i<0 ? i+N.size() : i-1;
                    };
                    std::vector<Index > f;
                    std::vector<Index > ftc;
                    std::vector<Index > fn;
                    
                    // Read each "word" after type
                    char word[IGL_LINE_MAX];
                    int offset;
                    while(sscanf(l,"%s%n",word,&offset) == 1)
                    {
                        
                        // adjust offset
                        l += offset;
                        
                        // Process word
                        long int i,it,in;
                        if(sscanf(word,"%ld/%ld/%ld",&i,&it,&in) == 3)
                        {
                            f.push_back(shift(i));
                            ftc.push_back(shift_t(it));
                            fn.push_back(shift_n(in));
                        }else if(sscanf(word,"%ld/%ld",&i,&it) == 2)
                        {
                            f.push_back(shift(i));
                            ftc.push_back(shift_t(it));
                        }else if(sscanf(word,"%ld//%ld",&i,&in) == 2)
                        {
                            f.push_back(shift(i));
                            fn.push_back(shift_n(in));
                        }else if(sscanf(word,"%ld",&i) == 1)
                        {
                            f.push_back(shift(i));
                        }else
                        {
                            fprintf(stderr,
                                    "Error: readOBJ() face on line %d has invalid element format\n",
                                    line_no);
                            fclose(obj_file);
                            return false;
                        }
                    }
                    if(
                       (f.size()>0 && fn.size() == 0 && ftc.size() == 0) ||
                       (f.size()>0 && fn.size() == f.size() && ftc.size() == 0) ||
                       (f.size()>0 && fn.size() == 0 && ftc.size() == f.size()) ||
                       (f.size()>0 && fn.size() == f.size() && ftc.size() == f.size()))
                    {
                        
                        // No matter what add each type to lists so that lists are the
                        // correct lengths
                        F.push_back(f);
                        FTC.push_back(ftc);
                        FN.push_back(fn);
                    }else
                    {
                        fprintf(stderr,
                                "Error: readOBJ() face on line %d has invalid format\n", line_no);
                        fclose(obj_file);
                        return false;
                    }
                }else if(strlen(type) >= 1 && (type[0] == '#' ||
                                               type[0] == 'g'  ||
                                               type[0] == 's'  ||
                                               strcmp("usemtl",type)==0 ||
                                               strcmp("mtllib",type)==0))
                {
                    //ignore comments or other shit
                }else
                {
                    //ignore any other lines
                    fprintf(stderr,
                            "Warning: readOBJ() ignored non-comment line %d:\n  %s",
                            line_no,
                            line);
                }
            }
    }else
    {
      // ignore empty line
    }
    line_no++;
  }
  fclose(obj_file);

  assert(F.size() == FN.size());
  assert(F.size() == FTC.size());

  return true;
}

template <typename DerivedV, typename DerivedF, typename DerivedT>
bool igl::readOBJ(
                             const std::string str, int pass,
                             Eigen::PlainObjectBase<DerivedV>& V,
                             Eigen::PlainObjectBase<DerivedT>& TC,
                             Eigen::PlainObjectBase<DerivedV>& CN,
                             Eigen::PlainObjectBase<DerivedF>& F,
                             Eigen::PlainObjectBase<DerivedF>& FTC,
                             Eigen::PlainObjectBase<DerivedF>& FN)
{
    std::vector<std::vector<double> > vV,vTC,vN;
    std::vector<std::vector<int> > vF,vFTC,vFN;
    bool success = igl::readOBJ(str, pass, vV,vTC,vN,vF,vFTC,vFN);
    if(!success)
    {
        // readOBJ(str,vV,vTC,vN,vF,vFTC,vFN) should have already printed an error
        // message to stderr
        return false;
    }
    bool V_rect = igl::list_to_matrix(vV,V);
    const char * format = "Failed to cast %s to matrix: min (%d) != max (%d)\n";
    if(!V_rect)
    {
        printf(format,"V",igl::min_size(vV),igl::max_size(vV));
        return false;
    }
    bool F_rect = igl::list_to_matrix(vF,F);
    if(!F_rect)
    {
        printf(format,"F",igl::min_size(vF),igl::max_size(vF));
        return false;
    }
    if(!vN.empty())
    {
        bool VN_rect = igl::list_to_matrix(vN,CN);
        if(!VN_rect)
        {
            printf(format,"CN",igl::min_size(vN),igl::max_size(vN));
            return false;
        }
    }
    
    if(!vFN.empty() && !vFN[0].empty())
    {
        bool FN_rect = igl::list_to_matrix(vFN,FN);
        if(!FN_rect)
        {
            printf(format,"FN",igl::min_size(vFN),igl::max_size(vFN));
            return false;
        }
    }
    
    if(!vTC.empty())
    {
        
        bool T_rect = igl::list_to_matrix(vTC,TC);
        if(!T_rect)
        {
            printf(format,"TC",igl::min_size(vTC),igl::max_size(vTC));
            return false;
        }
    }
    if(!vFTC.empty()&& !vFTC[0].empty())
    {
        
        bool FTC_rect = igl::list_to_matrix(vFTC,FTC);
        if(!FTC_rect)
        {
            printf(format,"FTC",igl::min_size(vFTC),igl::max_size(vFTC));
      return false;
    }
  }
  return true;
}

template <typename T, typename Derived>
IGL_INLINE bool igl::list_to_matrix(const std::vector<std::vector<T > > & V,Eigen::PlainObjectBase<Derived>& M)
{
    // number of rows
    int m = V.size();
    if(m == 0)
    {
        M.resize(0,0);
        return true;
    }
    // number of columns
    int n = igl::min_size(V);
    if(n != igl::max_size(V))
    {
        std::cout << "shit...\n";
        return false;
    }
    assert(n != -1);
    // Resize output
    M.resize(m,n);
    
    // Loop over rows
    for(int i = 0;i<m;i++)
    {
        // Loop over cols
        for(int j = 0;j<n;j++)
        {
            M(i,j) = V[i][j];
    }
  }

    return true;
}

template <typename T>
IGL_INLINE int igl::min_size(const std::vector<T> & V)
{
    int min_size = -1;
    for(
        typename std::vector<T>::const_iterator iter = V.begin();
        iter != V.end();
        iter++)
    {
        int size = (int)iter->size();
        // have to handle base case
        if(min_size == -1)
        {
            min_size = size;
        }else{
            if(min_size > size){
                std::cout << "new min_size at: " << iter-V.begin() << " of value " << size << "\n";
                T babybug = *iter;
                for(size_t i = 0; i < babybug.size(); i++) {
                    std::cout << babybug[i] << ", ";
                }
                std::cout << "\n";
            }
            min_size = (min_size < size ? min_size : size);
        }
    }
    return min_size;
}

template <typename T>
IGL_INLINE int igl::max_size(const std::vector<T> & V)
{
    int max_size = -1;
    for(
        typename std::vector<T>::const_iterator iter = V.begin();
        iter != V.end();
        iter++)
    {
        int size = (int)iter->size();
        max_size = (max_size > size ? max_size : size);
    }
    return max_size;
}

//template bool igl::test<std::string>(std::string obj);
template bool igl::test<char const*>(char const*);
template bool igl::readOBJ<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(std::basic_string<char, std::char_traits<char>, std::allocator<char> >, int, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&);






