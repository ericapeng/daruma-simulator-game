#include "Helpers.h"

#include <iostream>
#include <fstream>
#include <cstdio>

#include <cassert>
#include <Eigen/Dense>
#include <math.h>
#include <limits>

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

Eigen::VectorXf getObjCenter(Eigen::MatrixXf V){
    if(V.rows() == 4){
        Eigen::Vector4f sum(0,0,0,0);
        for(int i = 0; i < V.cols(); i++)
            sum = sum + V.col(i);
        sum = sum/V.cols();
        return sum;
    }
    
    Eigen::Vector3f sum(0,0,0);
    for(int i = 0; i < V.cols(); i++)
        sum = sum + V.col(i);
    sum = sum/V.cols();
    return sum;
}

double getPreviousFromDeque(std::deque<double> queue, double next) {
    for(std::deque<double>::iterator i = queue.begin(); i < queue.end(); i++){
        if(*(i+1) == next)
            return *i;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
//MESHOBJECTS IMPLEMENTED METHODS
//
////////////////////////////////////////////////////////////////////////////////////////////////

MeshObject::MeshObject(Eigen::MatrixXf V, Eigen::MatrixXf TC, Eigen::MatrixXf N, Eigen::MatrixXf F, Eigen::MatrixXf FTC, Eigen::MatrixXf FN) : V(V), TC(TC), N(N), F(F), FTC(FTC), FN(FN), textured(0), texIndex(-1), solidColor(0,0,0), T(4,4), currT(4,4)
{
    trianglify(F, V);
    trianglify(FTC, TC);
    trianglify(FN, N);
    
    VBO = new VertexBufferObject();
    VBO->init();
    TCBO = new VertexBufferObject();
    TCBO->init();
    NBO = new VertexBufferObject();
    NBO->init();
    
    Eigen::MatrixXf VFinal(3, F.cols()*3);
    Eigen::MatrixXf TCFinal(2, FTC.cols()*3);
    Eigen::MatrixXf NFinal(3, F.cols()*3);
    
    if(F.cols() != FTC.cols() || F.cols() != FN.cols()){
        std::cout << "NUMBER OF FACES DOES NOT MATCH FOR F, FTC, AND FN: \n";
        std::cout << "F.cols(): " << F.cols() << ", FTC.cols(): " << FTC.cols() << ", FN.cols(): " << FN.cols() << "\n";
    }
    int computedI = 0;
    for(int i = 0; i < F.cols(); i++) {
        computedI = i*3;
        for(int j = 0; j < 3; j++){
            if(F(j,i) > V.cols() || F(j,i) < 0){
                std::cout << F(j,i) << " > or < " << V.cols() << "\n";
            }
            VFinal.col(computedI+j) << V.col(F(j,i));
            TCFinal.col(computedI+j) << TC.col(FTC(j,i));
            NFinal.col(computedI+j) << N.col(FN(j,i));
        }
    }
    VFull = VFinal;
    TCFull = TCFinal;
    NFull = NFinal;
    
    VBO->update(VFinal);
    TCBO->update(TCFinal);
    NBO->update(NFinal);
    
    T << Eigen::MatrixXf::Identity(4,4);
    T_pointer = new float[16];
    update_pointer(T_pointer, T);
    currT << Eigen::MatrixXf::Identity(4,4);
    
    center = getObjCenter(V);
}

void MeshObject::transform(Eigen::MatrixXf newT) {
    currT = newT * T;
    update_pointer(T_pointer, currT);
}

void MeshObject::translate(Eigen::Vector3f from, Eigen::Vector3f to) {
    Eigen::MatrixXf TToApply = Eigen::MatrixXf::Identity(4,4);
    Eigen::Vector3f newBaryCenter = (to - from);
    TToApply.col(3) << newBaryCenter.x(), newBaryCenter.y(), 0, 1;
    transform(TToApply);
}

Eigen::Vector3f MeshObject::getTransformed(Eigen::Vector3f attrib) {
    Eigen::Vector4f extendedAttrib(attrib.x(), attrib.y(), attrib.z(), 1);
    extendedAttrib = currT * extendedAttrib;
    return *(new Eigen::Vector3f(extendedAttrib.x(), extendedAttrib.y(), extendedAttrib.z()));
}

Eigen::MatrixXf MeshObject::trianglify(Eigen::MatrixXf& M, Eigen::MatrixXf& Verts)
{
    if(M.rows() != 4){
        std::cout << "ERROR: Can only trianglify face matrices made up of four vertices per face.\n";
        return M;
    }
    
    Eigen::MatrixXf newM(3, M.cols()*2);
    int a;
    int b;
    int c;
    int d;
    std::vector<double> angles;
    std::vector<Eigen::Vector3f> rays;
    int computedI = 0;
    Eigen::Vector3f curVect;
    Eigen::Vector3f start;
    Eigen::Vector3f end;
    for(int i = 0; i < M.cols(); i++){
        a = M(0,i);
        for(int j = 1; j < 4; j++){
            if(Verts.rows() == 2){
                end << Verts.col(M(j,i)), 0;
                start << Verts.col(a), 0;
            }
            else{
                end = Verts.col(M(j,i));
                start = Verts.col(a);
            }
            rays.push_back((end-start).normalized());
        }
        
        angles.push_back(acos(rays[0].dot(rays[1])));
        angles.push_back(acos(rays[0].dot(rays[2])));
        angles.push_back(acos(rays[1].dot(rays[2])));
        
        //if the vector from M(0,i) to M(2,i) is in the middle
        if(angles[1] > angles[0] && angles[1] > angles[2]){
            b = M(2,i);
            c = M(1,i);
            d = M(3,i);
        }
        //if the vector from M(0,i) to M(1,i) is in the middle
        else if(angles[2] > angles[1] && angles[2] > angles[0]){
            b = M(1,i);
            c = M(2,i);
            d = M(3,i);
        }
        //if the vector from M(0,i) to M(3,i) is in the middle
        else if(angles[0] > angles[1] && angles[0] > angles[2]){
            b = M(3,i);
            c = M(2,i);
            d = M(1,i);
        }
        else{
            std::cout << "A CASE YOU DIDN'T ACCOUNT FOR OCCURRED\n";
        }
        
        computedI = i*2;
        newM.col(computedI) << a, c, b;
        newM.col(computedI+1) << b, d, a;
    }
    
    M = newM;
    return newM;
}

void MeshObject::reset() {
    T = Eigen::MatrixXf::Identity(4,4);
    currT = Eigen::MatrixXf::Identity(4,4);
    update_pointer(T_pointer, currT);
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//BLOCK IMPLEMENTED METHODS
//
////////////////////////////////////////////////////////////////////////////////////////////////

Block::Block(Eigen::MatrixXf V, Eigen::MatrixXf TC, Eigen::MatrixXf N, Eigen::MatrixXf F, Eigen::MatrixXf FTC, Eigen::MatrixXf FN) : MeshObject(V,TC,N,F,FTC,FN), xMinBound(std::numeric_limits<double>::max()), xMaxBound(std::numeric_limits<double>::min()), yMaxBound(std::numeric_limits<double>::min()), yMinBound(std::numeric_limits<double>::max()), velocity(0), state("static") {
    //determine xLeftBound and xRightBound
    double xToCheck = 0;
    double yToCheck = 0;
    for(int i = 0; i < V.cols(); i++) {
        xToCheck = V(0,i);
        yToCheck = V(1,i);
        if(xToCheck > xMaxBound)
            xMaxBound = xToCheck;
        if(xToCheck < xMinBound)
            xMinBound = xToCheck;
        
        if(yToCheck > yMaxBound)
            yMaxBound = yToCheck;
        if(yToCheck < yMinBound)
            yMinBound = yToCheck;
    }
    
    origyMinBound = yMinBound;
    origyMaxBound = yMaxBound;
    
    t_last_update = std::chrono::high_resolution_clock::now();
}

void Block::hit(std::deque<double> cursorXVelocities, double cursorX, Eigen::Vector3f hammerFace, double currAccel, int cheatMode) {
    std::cout << "hit acceleration: " << std::abs(currAccel) << "\n";
    if(cheatMode)
        currAccel = minTargetAccel;
    if(state == "base" || (state == "slide" && velocity == 0)){
        double secondToLastVelocity = getPreviousFromDeque(cursorXVelocities,cursorXVelocities.back());
        if(std::abs(currAccel) < minTargetAccel || std::abs(secondToLastVelocity) < std::abs(cursorXVelocities.back())) {
            if(std::abs(cursorX-xMaxBound) < std::abs(cursorX-xMinBound)){
                currT(0,3) = hammerFace.x() - xMaxBound;
            }
            else{
                currT(0,3) = hammerFace.x() - xMinBound;
            }
            state = "push_base";
            std::cout << "\tLESS THAN TARGET ACCEL ZONE\n";
        }
        else if(std::abs(currAccel) >= minTargetAccel && std::abs(currAccel) <= maxTargetAccel){
            if(std::abs(cursorXVelocities.back()) > std::abs(velocity))
                velocity = secondToLastVelocity;
            state = "slide";
            std::cout << "\tIN TARGET ACCEL ZONE\n";
        }
        else {
            std::cout << "\n\nYOU LOST :(\n\n";
            state = "boo";
        }
    }
    
}

void Block::updatePos() {
    auto t_now = std::chrono::high_resolution_clock::now();
    //don't need to do anything if static
    if(state == "static"){
        //std::cout << "Hi, I'm static\n";
    }
    
    //moves current block and all blocks above it
    else if(state == "push" || state == "push_base"){
        transform(currT);
        if(above != nullptr){
            above->state = "push";
            above->currT = currT;
            above->updatePos();
        }
        if(state == "push_base")
            state = "base";
    }
    
    //slides the current block and makes all blocks above it fall
    else if(state == "slide"){
        float interval = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_last_update).count();
        
        currT(0,3) = currT(0,3) + velocity*interval;
        transform(currT);
        
        double width = xMaxBound - xMinBound;
        double newXMin = getTransformed(*(new Eigen::Vector3f(0,xMinBound,0))).x();
        if(std::abs(newXMin - xMinBound) > width/2 && above != nullptr && above->yMinBound != 0.0015)
            above->state = "fall";
    }
    
    else if(state == "restack"){
        state = "static";
        velocity = 0;
        
        if(above != nullptr)
            above->state = "restack";
    }
    
    else if(state == "fall"){
        float interval = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_last_update).count();
        //divided gravity by half for visibility purposes
        velocity = (interval * GRAVITATIONALACCEL * -0.2)/METERSPERWORLDUNITS + velocity;
        
        currT(1,3) = currT(1,3) + velocity*interval;
        if(getTransformed(*(new Eigen::Vector3f(0,yMinBound,0))).y() <= 0.0015){
            state = "base";
            velocity = 0;
            currT(1,3) = 0.0015 - yMinBound;
            //update the ybounds and currT so that this is now the block is now the bottom block
            double height = yMaxBound - yMinBound;
            yMinBound = 0.0015;
            yMaxBound = yMinBound + height;
            
            if(above != nullptr)
                above->state = "restack";
        }
        else{
            transform(currT);
            
            if(above != nullptr){
                above->state = "fall";
            }
        }
    }
    t_last_update = t_now;
}

double Block::getTransformedBound(double bound) {
    return getTransformed(*(new Eigen::Vector3f(0,bound,0))).x();
}

void Block::reset() {
    MeshObject::reset();
    
    state = "static";
    velocity = 0;
    yMaxBound = origyMaxBound;
    yMinBound = origyMinBound;
}




////////////////////////////////////////////////////////////////////////////////////////////////
//
//HAMMER IMPLEMENTED METHODS
//
////////////////////////////////////////////////////////////////////////////////////////////////

Hammer::Hammer(Eigen::MatrixXf V, Eigen::MatrixXf TC, Eigen::MatrixXf N, Eigen::MatrixXf F, Eigen::MatrixXf FTC, Eigen::MatrixXf FN) : MeshObject(V,TC,N,F,FTC,FN), zMaxBound(std::numeric_limits<double>::min()), zMinBound(std::numeric_limits<double>::max()), yMaxBound(std::numeric_limits<double>::min()), yMinBound(std::numeric_limits<double>::max()), xMinBound(std::numeric_limits<double>::max()), xMaxBound(std::numeric_limits<double>::min()) {
    
    double zToCheck = 0;
    double yToCheck = 0;
    double xToCheck = 0;
    for(int i = 0; i < V.cols(); i++) {
        zToCheck = V(2,i);
        yToCheck = V(1,i);
        xToCheck = V(0,i);
        if(zToCheck > zMaxBound)
            zMaxBound = zToCheck;
        if(zToCheck < zMinBound)
            zMinBound = zToCheck;
        
        if(yToCheck > yMaxBound)
            yMaxBound = yToCheck;
        if(yToCheck < yMinBound)
            yMinBound = yToCheck;
        
        if(xToCheck > xMaxBound)
            xMaxBound = xToCheck;
        if(xToCheck < xMinBound)
            xMinBound = xToCheck;
    }
}

//permanently effects T
void Hammer::initialState(int degrees) {
    Eigen::MatrixXf TToApply = Eigen::MatrixXf::Identity(4,4);
    Eigen::MatrixXf transformation(2,2);
    int degreesToRotate = -90;
    double alpha = degreesToRotate*3.14159265/180;
    transformation << cos(alpha), sin(alpha)*(-1.0), sin(alpha), cos(alpha);
    
    TToApply.col(1) << 0, transformation.col(0), 0;
    TToApply.col(2) << 0, transformation.col(1), 0;
    
    //translate so that the object's barycenter doesn't change
    Eigen::MatrixXf origV(4,V.cols());
    origV << V, Eigen::MatrixXf::Ones(1,origV.cols());
    origV = T * origV;
    Eigen::MatrixXf newV = TToApply * origV;
    
    Eigen::Vector4f origBaryCenter = getObjCenter(origV);
    Eigen::Vector4f newBaryCenter = getObjCenter(newV);
    newBaryCenter = (newBaryCenter-origBaryCenter)*(-1.0);
    
    TToApply.col(3) << newBaryCenter.x(), newBaryCenter.y(), newBaryCenter.z(), 1;
    
    //translate again to align with blocks
    double shift = (yMaxBound - yMinBound)/4 - (zMaxBound - zMinBound)/2;
    TToApply(2,3) = TToApply(2,3) + shift;
    
    T = TToApply * T;
    update_pointer(T_pointer, currT);
    
    Eigen::Vector4f fullCenter(center.x(), center.y(), center.z(), 1);
    fullCenter = T * fullCenter;
    center << fullCenter.x(), fullCenter.y(), fullCenter.z();
    
    //init left and right face points
    Eigen::Vector3f hammerHead(center.x(), center.y(), center.z()-shift);
    leftFace << hammerHead.x()+xMinBound, hammerHead.y(), hammerHead.z();
    rightFace <<hammerHead.x()+xMaxBound, hammerHead.y(), hammerHead.z();
}



////////////////////////////////////////////////////////////////////////////////////////////////
//
//ALL CODE BENEATH THIS LINE IS ADAPTED FROM libigl
//
////////////////////////////////////////////////////////////////////////////////////////////////

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
    int vCount = 0;
    std::vector<int> vOffset;
    vOffset.push_back(0);
    int tcCount = 0;
    std::vector<int> tcOffset;
    tcOffset.push_back(0);
    int nCount = 0;
    std::vector<int> nOffset;
    nOffset.push_back(0);
    
    char line[IGL_LINE_MAX];
    int line_no = 1;
    while (fgets(line, IGL_LINE_MAX, obj_file) != NULL)
    {
        char type[IGL_LINE_MAX];

        // Read first word containing type
        if(sscanf(line, "%s",type) == 1)
        {
            if(type == v)
            {
                if(lastType == f){
                    currPass++;
                    lastType = v;
                }
                vCount++;
            }
            else if(type == vn){
                if(lastType == v){
                    vOffset.push_back(vCount);
                    lastType = vn;
                }
                nCount++;
            }
            else if(type == vt){
                if(lastType == vn){
                    nOffset.push_back(nCount);
                    lastType = vt;
                }
                tcCount++;
            }
            if(type == f && lastType == vt)
            {
                tcOffset.push_back(tcCount);
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
                    std::vector<Scalar > tex(2);    //forcefully discard the third coordinate
                    for(int i = 0;i<2;i++)
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
                            f.push_back(shift(i)-vOffset[pass]);
                            ftc.push_back(shift_t(it)-tcOffset[pass]);
                            fn.push_back(shift_n(in)-nOffset[pass]);
                        }else if(sscanf(word,"%ld/%ld",&i,&it) == 2)
                        {
                            f.push_back(shift(i)-vOffset[pass]);
                            ftc.push_back(shift_t(it)-tcOffset[pass]);
                        }else if(sscanf(word,"%ld//%ld",&i,&in) == 2)
                        {
                            f.push_back(shift(i)-vOffset[pass]);
                            fn.push_back(shift_n(in)-nOffset[pass]);
                        }else if(sscanf(word,"%ld",&i) == 1)
                        {
                            f.push_back(shift(i)-vOffset[pass]);
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

template bool igl::readOBJ<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(std::basic_string<char, std::char_traits<char>, std::allocator<char> >, int, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&);






