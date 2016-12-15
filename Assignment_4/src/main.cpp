// This example is heavily based on the tutorial at https://open.gl

// OpenGL Helpers to reduce the clutter
#include "Helpers.h"

// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>

// Linear Algebra Library
#include <Eigen/Core>

// Timer
#include <chrono>

#include <iostream>
#include <vector>

// VertexBufferObject wrapper
Program program;

std::vector<MeshObject*> meshObjects;

float* view_A_pointer = new float[16];

class ViewTransformations
{
public:
    ViewTransformations(GLFWwindow* window) : window(window), view_A(4,4), cam_A(4,4), window_A(4,4){
        view_A.setIdentity(4,4);
        cam_A.setIdentity(4,4);
        window_A.setIdentity(4,4);
        //initial view
        cam_A.col(3) << 0, -0.7, 0, 1;
        cam_A *= 1.0/2;
    }
    void updateView(int code = -1) {
        //zoom out
        if(code == 0){
            cam_A *= 0.8;
        }
        
        //zoom in
        else if(code == 1){
            cam_A *= 1.2;
        }
        
        //update scale to size of window (assuming the correct ratio is 600x600)
        window_A.setIdentity(4,4);
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        double xScale = 1.0/((double)width/600);
        double yScale = 1.0/((double)height/600);
        window_A.col(0) << xScale * window_A.col(0);
        window_A.col(1) << yScale * window_A.col(1);
        
        setView();
    }
    
private:
    void setView() {
        view_A = window_A * cam_A;
        update_pointer(view_A_pointer, view_A);
        glUniformMatrix4fv(program.uniform("view"), 1, true, view_A_pointer);
    }
    
    GLFWwindow* window;
    Eigen::MatrixXf view_A;
    Eigen::MatrixXf cam_A;
    Eigen::MatrixXf window_A;
};

ViewTransformations* viewTrans;

void drawMeshObjects(){
    for(MeshObject* object : meshObjects){
        // The vertex shader wants the position of the vertices as an input.
        // The following line connects the VBO we defined above with the position "slot"
        // in the vertex shader
        program.bindVertexAttribArray("position",*(object->VBO));
        //glUniformMatrix4fv(program.uniform("MModel"), 1, true, object->T_pointer);
        for(int i = 0; i < object->F.cols(); i++)
            //if you're having depth conflicts, try GL_TRIANGLE_STRIP
            glDrawArrays(GL_LINE_LOOP, i*4, 4);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // Get the position of the mouse in the window
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Get the size of the window
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Convert screen position to world coordinates
    double xworld = ((xpos/double(width))*2)-1;
    double yworld = (((height-1-ypos)/double(height))*2)-1; // NOTE: y axis is flipped in glfw

    // Update the position of the first vertex if the left button is pressed
    /*if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        V.col(0) << xworld, yworld;*/

    // Upload the change to the GPU
    //VBO.update(V);
}

int shift_on = 0;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Update the position of the first vertex if the keys 1,2, or 3 are pressed
    switch (key)
    {
        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_RIGHT_SHIFT:
            if(action == GLFW_PRESS)
                shift_on = 1;
            else if(action == GLFW_RELEASE)
                shift_on = 0;
            break;
        case GLFW_KEY_EQUAL:
            if(shift_on && action == GLFW_PRESS)
                viewTrans->updateView(1);
            break;
        case GLFW_KEY_MINUS:
            if(action == GLFW_PRESS)
                viewTrans->updateView(0);
            break;
        case  GLFW_KEY_1:
            //V.col(0) << -0.5,  0.5;
            break;
        case GLFW_KEY_2:
            //V.col(0) << 0,  0.5;
            break;
        case  GLFW_KEY_3:
            //V.col(0) << 0.5,  0.5;
            break;
        default:
            break;
    }

    // Upload the change to the GPU
    //VBO.update(V);
}

int main(void)
{
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    // Activate supersampling
    glfwWindowHint(GLFW_SAMPLES, 8);

    // Ensure that we get at least a 3.2 context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    // On apple we have to load a core profile with forward compatibility
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a windowed mode window and its OpenGL context
    //window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize the VAO
    // A Vertex Array Object (or VAO) is an object that describes how the vertex
    // attributes are stored in a Vertex Buffer Object (or VBO). This means that
    // the VAO is not the actual object storing the vertex data,
    // but the descriptor of the vertex data.
    VertexArrayObject VAO;
    VAO.init();
    VAO.bind();

    // Initialize the VBO with the vertices data
    // A VBO is a data container that lives in the GPU memory
    /*VBO.init();

    V.resize(3,4);*/
    /*V << 0,  0.5, -0.5, 0,
    0.5, -0.5, -0.5, 0.5,
    0, 0, 0, 0;*/
    /*V << 0.5, 0.5, -0.5, -0.5,
    0.5, -0.5, 0.5, -0.5,
    0, 0, 0, 0;
    VBO.update(V);*/
    
    igl::test("SUCCESS ");
    
    Eigen::Matrix<double, -1, -1, 0, -1, -1> VM;
    Eigen::Matrix<double, -1, -1, 0, -1, -1> TCM;
    Eigen::Matrix<double, -1, -1, 0, -1, -1> NM;
    Eigen::Matrix<int, -1, -1, 0, -1, -1> FM;
    Eigen::Matrix<int, -1, -1, 0, -1, -1> FTCM;
    Eigen::Matrix<int, -1, -1, 0, -1, -1> FNM;
    for(int i = 0; i < 7; i++){
        igl::readOBJ("../data/darumaotoshi_obj/darumaotoshi_obj.obj", i, VM, TCM, NM, FM, FTCM, FNM);
        meshObjects.push_back(new MeshObject(
                                             VM.transpose().cast<float>(),
                                             TCM.transpose().cast<float>(),
                                             NM.transpose().cast<float>(),
                                             FM.transpose().cast<float>(),
                                             FTCM.transpose().cast<float>(),
                                             FNM.transpose().cast<float>()));
    }

    // Initialize the OpenGL Program
    // A program controls the OpenGL pipeline and it must contains
    // at least a vertex shader and a fragment shader to be valid
    const GLchar* vertex_shader =
            "#version 150 core\n"
                    "in vec3 position;"
                    "uniform mat4 view;"
                    "void main()"
                    "{"
                    "    vec4 vec4pos = vec4(position[0],position[1],position[2],1.0);"
                    "    mat4 newM = view;"
                    "    vec4 newPos = newM * vec4pos;"
                    "    gl_Position = vec4(newPos.xyz, 1.0);"
                    "}";
    const GLchar* fragment_shader =
            "#version 150 core\n"
                    "out vec4 outColor;"
                    "uniform vec3 triangleColor;"
                    "void main()"
                    "{"
                    "    outColor = vec4(triangleColor, 1.0);"
                    "}";

    // Compile the two shaders and upload the binary to the GPU
    // Note that we have to explicitly specify that the output "slot" called outColor
    // is the one that we want in the fragment buffer (and thus on screen)
    program.init(vertex_shader,fragment_shader,"outColor");
    program.bind();

    // Save the current time --- it will be used to dynamically change the triangle color
    auto t_start = std::chrono::high_resolution_clock::now();

    // Register the keyboard callback
    glfwSetKeyCallback(window, key_callback);

    // Register the mouse callback
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    
    viewTrans = new ViewTransformations(window);
    viewTrans->updateView();

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        viewTrans->updateView();
        
        // Bind your VAO (not necessary if you have only one)
        VAO.bind();

        // Bind your program
        program.bind();

        // Set the uniform value depending on the time difference
        auto t_now = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
        glUniform3f(program.uniform("triangleColor"), (float)(sin(time * 4.0f) + 1.0f) / 2.0f, 0.0f, 0.0f);

        // Clear the framebuffer
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        drawMeshObjects();

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Deallocate opengl memory
    program.free();
    VAO.free();
    //VBO.free();
    
    for(MeshObject* mo : meshObjects)
        delete mo;
    meshObjects.clear();

    // Deallocate glfw internals
    glfwTerminate();
    return 0;
}
