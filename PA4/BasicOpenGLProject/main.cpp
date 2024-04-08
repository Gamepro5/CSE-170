#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define _USE_MATH_DEFINES
#include <iostream>
#include <cmath>
#include <math.h>
#include "shader.h"
#include "shaderprogram.h"
#include <vector>




/*=================================================================================================
	DOMAIN
=================================================================================================*/

// Window dimensions
const int InitWindowWidth  = 800;
const int InitWindowHeight = 800;
int WindowWidth  = InitWindowWidth;
int WindowHeight = InitWindowHeight;

// Last mouse cursor position
int LastMousePosX = 0;
int LastMousePosY = 0;

// Arrays that track which keys are currently pressed
bool key_states[256];
bool key_special_states[256];
bool mouse_states[8];

// Other parameters
bool draw_wireframe = false;
float toRadians = M_PI / 180.0;
bool debug_draw_normals = false;
/*=================================================================================================
	SHADERS & TRANSFORMATIONS
=================================================================================================*/

ShaderProgram PassthroughShader;
ShaderProgram PerspectiveShader;
ShaderProgram LightShader;
ShaderProgram TextureShader;
ShaderProgram PerspectiveTextureLightShader;

glm::mat4 PerspProjectionMatrix( 1.0f );
glm::mat4 PerspViewMatrix( 1.0f );
glm::mat4 PerspModelMatrix( 1.0f );

float perspZoom = 1.0f, perspSensitivity = 0.35f;
float perspRotationX = 0.0f, perspRotationY = 0.0f;




/*=================================================================================================
    CLASSES
=================================================================================================*/


class Vector {
public:
    float x;
    float y;
    float z;

    Vector() {
        x = 0;
        y = 0;
        z = 0;
    }

    Vector(float _x, float _y, float _z) {
        x = _x;
        y = _y;
        z = _z;
    };
    float length() {
        return sqrt((pow(x, 2) + pow(y, 2) + pow(z, 2)));
    }
    Vector normalized() {
        Vector result;
        result.x = x / length();
        result.y = y / length();
        result.z = z / length();
        return result;
    }
    void print() {
        std::cout << "Vector(" << x << ", " << y << ", " << z << ")" << std::endl;
    }
    Vector operator - (Vector const& obj1) {
        Vector obj2;
        obj2.x = x - obj1.x;
        obj2.y = y - obj1.y;
        obj2.z = z - obj1.z;
        return obj2;
    };
    Vector operator * (Vector const& obj1) {
        Vector obj2;
        obj2.x = x * obj1.x;
        obj2.y = y * obj1.y;
        obj2.z = z * obj1.z;
        return obj2;
    };
    Vector operator + (Vector const& obj1) {
        Vector obj2;
        obj2.x = x + obj1.x;
        obj2.y = y + obj1.y;
        obj2.z = z + obj1.z;
        return obj2;
    };
    Vector operator / (Vector const& obj1) {
        Vector obj2;
        obj2.x = x / obj1.x;
        obj2.y = y / obj1.y;
        obj2.z = z / obj1.z;
        return obj2;
    };
    Vector operator = (Vector const& obj1) {
        x = obj1.x;
        y = obj1.y;
        z = obj1.z;
        return *this;
    };
};


class Triangle {
public:
    glm::vec4 x;
    glm::vec4 y;
    glm::vec4 z;

    Triangle() {
        x = glm::vec4();
        y = glm::vec4();
        z = glm::vec4();
    }

    Triangle(glm::vec4 _x, glm::vec4 _y, glm::vec4 _z) {
        x = _x;
        y = _y;
        z = _z;
    };
    void print() {
        std::cout << "Triangle Consists of: Vector(" << x.x << ", " << x.y << ", " << x.z << "), " << "Vector(" << x.x << ", " << y.y << ", " << y.z << "), " << "Vector(" << z.x << ", " << z.y << ", " << z.z << "), " << std::endl;
    }
};



class Entity {
private:
    GLuint draw_VAO;
    GLuint draw_VBO[4];

    GLuint debug_VAO;
    GLuint debug_VBO[2];

    void CreateDrawBuffers() {
        glGenVertexArrays(1, &draw_VAO); //generate 1 new VAO, its ID is returned in axis_VAO
        glBindVertexArray(draw_VAO); //bind the VAO so the subsequent commands modify it

        glGenBuffers(4, &draw_VBO[0]); //generate 2 buffers for data, their IDs are returned to the axis_VBO array

        // first buffer: vertex coordinates
        glBindBuffer(GL_ARRAY_BUFFER, draw_VBO[0]); //bind the first buffer using its ID
        glBufferData(GL_ARRAY_BUFFER, sizeof(verticies[0]) * verticies.size(), &verticies[0], GL_STATIC_DRAW); //send coordinate array to the GPU
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); //let GPU know this is attribute 0, made up of 4 floats
        glEnableVertexAttribArray(0);

        // second buffer: colors
        glBindBuffer(GL_ARRAY_BUFFER, draw_VBO[1]); //bind the second buffer using its ID
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors[0]) * colors.size(), &colors[0], GL_STATIC_DRAW); //send color array to the GPU
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); //let GPU know this is attribute 1, made up of 4 floats
        glEnableVertexAttribArray(1);


        glBindBuffer(GL_ARRAY_BUFFER, draw_VBO[2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), &normals[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, draw_VBO[3]); //bind the second buffer using its ID
        glBufferData(GL_ARRAY_BUFFER, sizeof(texture_coordinates[0]) * texture_coordinates.size(), &texture_coordinates[0], GL_STATIC_DRAW); //send normal array to the GPU
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0); //let GPU know this is attribute 1, made up of 4 floats
        glEnableVertexAttribArray(3);

        glBindVertexArray(0); //unbind when done
    }


    void CreateDebugBuffers(void)
    {
        glGenVertexArrays(1, &debug_VAO); //generate 1 new VAO, its ID is returned in axis_VAO
        glBindVertexArray(debug_VAO); //bind the VAO so the subsequent commands modify it

        glGenBuffers(2, &debug_VBO[0]); //generate 2 buffers for data, their IDs are returned to the axis_VBO array

        // first buffer: vertex coordinates
        glBindBuffer(GL_ARRAY_BUFFER, debug_VBO[0]); //bind the first buffer using its ID
        glBufferData(GL_ARRAY_BUFFER, sizeof(debug_normals[0]) * debug_normals.size(), &debug_normals[0], GL_STATIC_DRAW); //send coordinate array to the GPU
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); //let GPU know this is attribute 0, made up of 4 floats
        glEnableVertexAttribArray(0);

        // second buffer: colors
        glBindBuffer(GL_ARRAY_BUFFER, debug_VBO[1]); //bind the second buffer using its ID
        glBufferData(GL_ARRAY_BUFFER, sizeof(debug_normals_colors[0]) * debug_normals_colors.size(), &debug_normals_colors[0], GL_STATIC_DRAW); //send color array to the GPU
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); //let GPU know this is attribute 1, made up of 4 floats
        glEnableVertexAttribArray(1);

        glBindVertexArray(0); //unbind when done
    }
public:
    std::vector<glm::vec4> verticies;
    std::vector<glm::vec4> colors;
    std::vector<glm::vec4> debug_normals;
    std::vector<glm::vec4> debug_normals_colors;
    std::vector<glm::vec4> normals;
    std::vector<glm::vec2> texture_coordinates;
    Vector rotation;
    Vector position;
    Vector scale;
    unsigned char* texture_data;
    int texture_img_width;
    int texture_img_height;
    int texture_img_channels;
    GLuint textureId;
    std::string texture_path = "";

    void set_texture(const char* path) {
        std::cout << path << std::endl;
        texture_path = std::string(path);
        delete texture_data;
        texture_data = stbi_load(path, &texture_img_width, &texture_img_height, &texture_img_channels, 0);
    }




    void draw() {
        glm::mat4 rotationMatrixX(
            { 1.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, std::cos(rotation.x), -std::sin(rotation.x), 0.0f },
            { 0.0f, std::sin(rotation.x), std::cos(rotation.x), 0.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        );
        glm::mat4 rotationMatrixY(
            { std::cos(rotation.y), 0.0f, std::sin(rotation.y), 0.0f },
            { 0.0f, 1.0f, 0.0f, 0.0f },
            { -std::sin(rotation.y), 0.0f, std::cos(rotation.y), 0.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        );
        glm::mat4 rotationMatrixZ(
            { std::cos(rotation.z), -std::sin(rotation.z), 0.0f, 0.0f },
            { std::sin(rotation.z), std::cos(rotation.z), 0.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        );
        glm::mat4 scaleMatrix(
            { scale.x, 0.0f, 0.0f, 0.0f },
            { 0.0f, scale.y, 0.0f, 0.0f },
            { 0.0f, 0.0f, scale.z, 0.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        );
        glm::mat4 translationMatrix(
            { 1.0f, 0.0f, 0.0f, position.x },
            { 0.0f, 1.0f, 0.0f, position.y },
            { 0.0f, 0.0f, 1.0f, position.z },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        );
        //apply transformation shader
        CreateDrawBuffers();
        
        
        if (texture_data) {
            glGenTextures(1, &textureId);
            glBindTexture(GL_TEXTURE_2D, textureId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_img_width, texture_img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            PerspectiveTextureLightShader.Use();
            PerspectiveTextureLightShader.SetUniform("projectionMatrix", glm::value_ptr(PerspProjectionMatrix), 4, GL_FALSE, 1);
            PerspectiveTextureLightShader.SetUniform("viewMatrix", glm::value_ptr(PerspViewMatrix), 4, GL_FALSE, 1);
            PerspectiveTextureLightShader.SetUniform("modelMatrix", glm::value_ptr(PerspModelMatrix * scaleMatrix * translationMatrix * rotationMatrixZ * rotationMatrixY * rotationMatrixX), 4, GL_FALSE, 1);
            PerspectiveTextureLightShader.SetUniform("texId", textureId);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureId);
        }
        else {
            LightShader.Use();
            LightShader.SetUniform("projectionMatrix", glm::value_ptr(PerspProjectionMatrix), 4, GL_FALSE, 1);
            LightShader.SetUniform("viewMatrix", glm::value_ptr(PerspViewMatrix), 4, GL_FALSE, 1);
            LightShader.SetUniform("modelMatrix", glm::value_ptr(PerspModelMatrix * scaleMatrix * translationMatrix * rotationMatrixZ * rotationMatrixY * rotationMatrixX), 4, GL_FALSE, 1);
        }
        

        //draw it
        glBindVertexArray(draw_VAO);
        glDrawArrays(GL_TRIANGLES, 0, verticies.size());
        if (debug_draw_normals == true) {
            PerspectiveShader.Use();
            PerspectiveShader.SetUniform("projectionMatrix", glm::value_ptr(PerspProjectionMatrix), 4, GL_FALSE, 1);
            PerspectiveShader.SetUniform("viewMatrix", glm::value_ptr(PerspViewMatrix), 4, GL_FALSE, 1);
            CreateDebugBuffers();
            PerspectiveShader.SetUniform("modelMatrix", glm::value_ptr(PerspModelMatrix * scaleMatrix * translationMatrix * rotationMatrixZ * rotationMatrixY * rotationMatrixX), 4, GL_FALSE, 1);
            glBindVertexArray(debug_VAO);
            glDrawArrays(GL_LINES, 0, debug_normals.size());
        }
        glDeleteTextures(1, &textureId);
    }





    Entity(std::vector<glm::vec4> _verticies, std::vector<glm::vec4> _color) {
        for (int i = 0; i < _verticies.size(); i++) {
            verticies.push_back(_verticies[i]);
            colors.push_back(_color[i]);
        }
        scale.x = 1;  // default scale is 1, not zero.
        scale.y = 1;
        scale.z = 1;
    };
    Entity() {
        scale.x = 1;  // default scale is 1, not zero.
        scale.y = 1;
        scale.z = 1;
    };
};



class Torus : public Entity {
private:
    void addDebugNormal(glm::vec4 start, glm::vec4 end) {
        debug_normals.push_back(start);
        debug_normals.push_back(start + end);
        debug_normals_colors.push_back(glm::vec4(1.0, 0.64, 0.0, 1.0));
    }
    glm::vec4 calculateNormal(glm::vec4 p1, glm::vec4 p2, glm::vec4 p3) {
        glm::vec4 U = p2 - p1;
        glm::vec4 V = p3 - p1;
        glm::vec4 result = glm::vec4((U.y * V.z) - (U.z * V.y), (U.z * V.x) - (U.x * V.z), (U.x * V.y) - (U.y * V.x), 1.0);
        return result;
    };
public:
    bool smooth_shading = false;
    float radius = 2;
    float inner_radius = 0.5;
    float vertexCount = 500;
    std::vector<Triangle> triangles;
    std::vector<glm::vec4> flat_normals;
    std::vector<glm::vec4> smooth_normals;
    std::vector<glm::vec4> ringCenters;


    int slices = 8;
    int loops = 20;

    int _mainSegments = 50;
    int _tubeSegments = 20;
    float _mainRadius = 2;
    float _tubeRadius = 0.5;



    void addVerticies() {
        vertexCount += 50;
    };

    void removeVerticies() {
        vertexCount -= 50;
    };

    void increaseRadius() {
        radius += 0.01;
    };

    void decreaseRadius() {
        radius -= 0.01;
    };

    void increaseInnerRadius() {
        inner_radius += 0.01;
    };

    void decreaseInnerRadius() {
        inner_radius -= 0.01;
    };

    void Construct() {
        verticies.clear();
        colors.clear();
        triangles.clear();
        normals.clear();
        smooth_normals.clear();
        flat_normals.clear();
        ringCenters.clear();
        debug_normals.clear();
        debug_normals_colors.clear();
        texture_coordinates.clear();

        std::vector<std::vector<glm::vec4>> rings;
        for (float theta = 0; theta < 2 * M_PI; theta += 2 * M_PI / sqrt(vertexCount)) {
            std::vector<glm::vec4> ring;
            for (float phi = 0; phi < 2 * M_PI; phi += 2 * M_PI / sqrt(vertexCount)) {
                ring.push_back(glm::vec4((radius + inner_radius * cos(theta)) * cos(phi), (radius + inner_radius * cos(theta)) * sin(phi), inner_radius * sin(theta), 1.0));
            }
            rings.push_back(ring);
        }
        for (float phi = 0; phi < 2 * M_PI; phi += 2 * M_PI / sqrt(vertexCount)) {
            ringCenters.push_back(glm::vec4(radius * cos(phi), radius * sin(phi), 0, 1.0));
        }
        for (int i = 0; i < rings.size(); i++) {
            for (int j = 0; j < rings[i].size(); j++) {
                int modulo_i = (i + 1) % rings.size();
                int modulo_j = (j + 1) % rings[i].size();
                auto t = Triangle();
                t.x = rings[i][j];
                smooth_normals.push_back(rings[i][j] - ringCenters[j]);
                addDebugNormal(rings[i][j], glm::normalize(rings[i][j] - ringCenters[j]));
                t.y = rings[i][modulo_j];
                smooth_normals.push_back(rings[i][modulo_j] - ringCenters[modulo_j]);
                addDebugNormal(rings[i][modulo_j], glm::normalize(rings[i][modulo_j] - ringCenters[modulo_j]));
                t.z = rings[modulo_i][j];
                smooth_normals.push_back(rings[modulo_i][j] - ringCenters[j]);
                addDebugNormal(rings[modulo_i][j], glm::normalize(rings[modulo_i][j] - ringCenters[j]));
                triangles.push_back(t);

                auto t2 = Triangle();
                t2.x = rings[modulo_i][modulo_j];
                smooth_normals.push_back(rings[modulo_i][modulo_j] - ringCenters[modulo_j]);
                addDebugNormal(rings[modulo_i][modulo_j], glm::normalize(rings[modulo_i][modulo_j] - ringCenters[modulo_j]));
                t2.y = rings[modulo_i][j];
                smooth_normals.push_back(rings[modulo_i][j] - ringCenters[j]);
                addDebugNormal(rings[modulo_i][j], glm::normalize(rings[modulo_i][j] - ringCenters[j]));
                t2.z = rings[i][modulo_j];
                smooth_normals.push_back(rings[i][modulo_j] - ringCenters[modulo_j]);
                addDebugNormal(rings[i][modulo_j], glm::normalize(rings[i][modulo_j] - ringCenters[modulo_j]));

                triangles.push_back(t2);
            }
        }

        for (int i = 0; i < triangles.size(); i++) {

            verticies.push_back(triangles[i].x);

            colors.push_back(glm::vec4(1.0, 1.0, 1.0, 1.0));

            verticies.push_back(triangles[i].y);

            colors.push_back(glm::vec4(1.0, 1.0, 1.0, 1.0));

            verticies.push_back(triangles[i].z);

            colors.push_back(glm::vec4(1.0, 1.0, 1.0, 1.0));

            glm::vec4 normal = calculateNormal(triangles[i].x, triangles[i].y, triangles[i].z);
            flat_normals.push_back(normal);
            flat_normals.push_back(normal);
            flat_normals.push_back(normal);


        }
        if (smooth_shading == true) {
            for (int i = 0; i < smooth_normals.size(); i++) {
                normals.push_back(smooth_normals[i]);
            }

        }
        else {
            debug_normals.clear();
            debug_normals_colors.clear();
            for (int i = 0; i < flat_normals.size(); i++) {

                addDebugNormal(verticies[i], glm::normalize(flat_normals[i]));

                normals.push_back(flat_normals[i]);


            }
        }

        for (int i = 0; i < verticies.size(); i++) {
            texture_coordinates.push_back(glm::vec2(verticies[i].x / 2, verticies[i].y / 2));
        }
    }

    Torus() {
        Construct();
    }
};





/*=================================================================================================
	OBJECTS
=================================================================================================*/

//VAO -> the object "as a whole", the collection of buffers that make up its data
//VBOs -> the individual buffers/arrays with data, for ex: one for coordinates, one for color, etc.

GLuint axis_VAO;
GLuint axis_VBO[2];

float axis_vertices[] = {
	//x axis
	-1.0f,  0.0f,  0.0f, 1.0f,
	1.0f,  0.0f,  0.0f, 1.0f,
	//y axis
	0.0f, -1.0f,  0.0f, 1.0f,
	0.0f,  1.0f,  0.0f, 1.0f,
	//z axis
	0.0f,  0.0f, -1.0f, 1.0f,
	0.0f,  0.0f,  1.0f, 1.0f
};

float axis_colors[] = {
	//x axis
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	//y axis
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	//z axis
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f
};

Torus* MyTorus = new Torus();


std::vector<Torus*> Tori{ MyTorus };




/*=================================================================================================
	HELPER FUNCTIONS
=================================================================================================*/

void window_to_scene( int wx, int wy, float& sx, float& sy )
{
	sx = ( 2.0f * (float)wx / WindowWidth ) - 1.0f;
	sy = 1.0f - ( 2.0f * (float)wy / WindowHeight );
}

/*=================================================================================================
	SHADERS
=================================================================================================*/

void CreateTransformationMatrices( void )
{
	// PROJECTION MATRIX
	PerspProjectionMatrix = glm::perspective<float>( glm::radians( 60.0f ), (float)WindowWidth / (float)WindowHeight, 0.01f, 1000.0f );

	// VIEW MATRIX
	glm::vec3 eye   ( 0.0, 0.0, 2.0 );
	glm::vec3 center( 0.0, 0.0, 0.0 );
	glm::vec3 up    ( 0.0, 1.0, 0.0 );

	PerspViewMatrix = glm::lookAt( eye, center, up );

	// MODEL MATRIX
	PerspModelMatrix = glm::mat4( 1.0 );
	PerspModelMatrix = glm::rotate( PerspModelMatrix, glm::radians( perspRotationX ), glm::vec3( 1.0, 0.0, 0.0 ) );
	PerspModelMatrix = glm::rotate( PerspModelMatrix, glm::radians( perspRotationY ), glm::vec3( 0.0, 1.0, 0.0 ) );
	PerspModelMatrix = glm::scale( PerspModelMatrix, glm::vec3( perspZoom ) );
}

void CreateShaders( void )
{
	// Renders without any transformations
	PassthroughShader.Create( "./shaders/simple.vert", "./shaders/simple.frag" );

	// Renders using perspective projection
	PerspectiveShader.Create( "./shaders/persp.vert", "./shaders/persp.frag" );

	LightShader.Create("./shaders/persplight.vert", "./shaders/persplight.frag");

	TextureShader.Create("./shaders/texpersp.vert", "./shaders/texpersp.frag");

	PerspectiveTextureLightShader.Create("./shaders/texpersplight.vert", "./shaders/texpersplight.frag");
	//
	// Additional shaders would be defined here
	//
}

/*=================================================================================================
	BUFFERS
=================================================================================================*/

void CreateAxisBuffers( void )
{
	glGenVertexArrays( 1, &axis_VAO ); //generate 1 new VAO, its ID is returned in axis_VAO
	glBindVertexArray( axis_VAO ); //bind the VAO so the subsequent commands modify it

	glGenBuffers( 2, &axis_VBO[0] ); //generate 2 buffers for data, their IDs are returned to the axis_VBO array

	// first buffer: vertex coordinates
	glBindBuffer( GL_ARRAY_BUFFER, axis_VBO[0] ); //bind the first buffer using its ID
	glBufferData( GL_ARRAY_BUFFER, sizeof( axis_vertices ), axis_vertices, GL_STATIC_DRAW ); //send coordinate array to the GPU
	glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), (void*)0 ); //let GPU know this is attribute 0, made up of 4 floats
	glEnableVertexAttribArray( 0 );

	// second buffer: colors
	glBindBuffer( GL_ARRAY_BUFFER, axis_VBO[1] ); //bind the second buffer using its ID
	glBufferData( GL_ARRAY_BUFFER, sizeof( axis_colors ), axis_colors, GL_STATIC_DRAW ); //send color array to the GPU
	glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), (void*)0 ); //let GPU know this is attribute 1, made up of 4 floats
	glEnableVertexAttribArray( 1 );

	glBindVertexArray( 0 ); //unbind when done

	//NOTE: You will probably not use an array for your own objects, as you will need to be
	//      able to dynamically resize the number of vertices. Remember that the sizeof()
	//      operator will not give an accurate answer on an entire vector. Instead, you will
	//      have to do a calculation such as sizeof(v[0]) * v.size().
}


/*=================================================================================================
	CALLBACKS
=================================================================================================*/

//-----------------------------------------------------------------------------
// CALLBACK DOCUMENTATION
// https://www.opengl.org/resources/libraries/glut/spec3/node45.html
// http://freeglut.sourceforge.net/docs/api.php#WindowCallback
//-----------------------------------------------------------------------------

void idle_func()
{
	//uncomment below to repeatedly draw new frames
	glutPostRedisplay();
}

void reshape_func( int width, int height )
{
	WindowWidth  = width;
	WindowHeight = height;

	glViewport( 0, 0, width, height );
	glutPostRedisplay();
}
void keyboard_func(unsigned char key, int x, int y)
{
	key_states[key] = true;

	switch (key)
	{
	case 'f':
	{
		draw_wireframe = !draw_wireframe;
		if (draw_wireframe == true)
			std::cout << "Wireframes on.\n";
		else
			std::cout << "Wireframes off.\n";
		break;
	}

	case '?':
	{
		std::cout << "- 'q' : increment the number of triangles(n)\n- 'a' : decrement the number of triangles(n)\n- 'w' : increment the r radius(by a small value)\n- 's' : decrement the r radius(by a small value)\n- 'e' : increment the R radius(by a small value)\n- 'd' : decrement the R radius(by a small value)\n- 'c' : Constructs the object (done automatically when using another key)\n- 'f' : toggle wireframes.\n- 'i' : toggle draw normals.\n- 'p' : toggle shading mode.\n- 'space bar' : Cycle between 3 different texture images." << std::endl;
		break;
	}
	case 'c':
	{
		MyTorus->Construct();
		break;
	}
	case 'q':
	{
		MyTorus->addVerticies();
		MyTorus->Construct();
		break;
	}
	case 'a':
	{
		MyTorus->removeVerticies();
		MyTorus->Construct();
		break;
	}
	case 'w':
	{
		MyTorus->increaseInnerRadius();
		MyTorus->Construct();
		break;
	}
	case 's':
	{
		MyTorus->decreaseInnerRadius();
		MyTorus->Construct();
		break;
	}
	case 'e':
	{
		MyTorus->increaseRadius();
		MyTorus->Construct();
		break;
	}
	case 'd':
	{
		MyTorus->decreaseRadius();
		MyTorus->Construct();
		break;
	}
	case 'i':
	{
		debug_draw_normals = !debug_draw_normals;
		break;
	}
	case 'p':
	{
		MyTorus->smooth_shading = !MyTorus->smooth_shading;
		MyTorus->Construct();
		break;
	}
	case ' ':
	{
		std::string new_path = "";
		
		if (MyTorus->texture_path == "stb/pav_food.jpg") {
			new_path = "stb/sam.jpg";
		}
		else if (MyTorus->texture_path == "stb/sam.jpg") {
			new_path = "stb/Texture1.jpg";
		}
		else if (MyTorus->texture_path == "stb/Texture1.jpg") {
			new_path = "stb/pav_food.jpg";
        }
        else {
            new_path = "stb/pav_food.jpg";
        }
        std::cout << new_path << std::endl;
        const char* c = new_path.c_str();
		MyTorus->set_texture(c);
		MyTorus->Construct();
        break;
	}


	// Exit on escape key press
	case '\x1B':
	{
		exit(EXIT_SUCCESS);
		break;
	}
	}
}

void key_released(unsigned char key, int x, int y)
{
	key_states[key] = false;



}

void key_special_pressed(int key, int x, int y)
{
	key_special_states[key] = true;
}

void key_special_released(int key, int x, int y)
{
	key_special_states[key] = false;
}

void mouse_func(int button, int state, int x, int y)
{
	// Key 0: left button
	// Key 1: middle button
	// Key 2: right button
	// Key 3: scroll up
	// Key 4: scroll down

	if (x < 0 || x > WindowWidth || y < 0 || y > WindowHeight)
		return;

	float px, py;
	window_to_scene(x, y, px, py);

	if (button == 3)
	{
		perspZoom += 0.03f;
	}
	else if (button == 4)
	{
		if (perspZoom - 0.03f > 0.0f)
			perspZoom -= 0.03f;
	}

	mouse_states[button] = (state == GLUT_DOWN);

	LastMousePosX = x;
	LastMousePosY = y;
}

void passive_motion_func(int x, int y)
{
	if (x < 0 || x > WindowWidth || y < 0 || y > WindowHeight)
		return;

	float px, py;
	window_to_scene(x, y, px, py);

	LastMousePosX = x;
	LastMousePosY = y;
}

void active_motion_func(int x, int y)
{
	if (x < 0 || x > WindowWidth || y < 0 || y > WindowHeight)
		return;

	float px, py;
	window_to_scene(x, y, px, py);

	if (mouse_states[0] == true)
	{
		perspRotationY += (x - LastMousePosX) * perspSensitivity;
		perspRotationX += (y - LastMousePosY) * perspSensitivity;
	}

	LastMousePosX = x;
	LastMousePosY = y;
}


/*=================================================================================================
	RENDERING
=================================================================================================*/

void display_func( void )
{	
	// Clear the contents of the back buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	CreateAxisBuffers();
	// Update transformation matrices
	CreateTransformationMatrices();

	// Choose which shader to user, and send the transformation matrix information to it
	PerspectiveShader.Use();
	PerspectiveShader.SetUniform( "projectionMatrix", glm::value_ptr( PerspProjectionMatrix ), 4, GL_FALSE, 1 );
	PerspectiveShader.SetUniform( "viewMatrix", glm::value_ptr( PerspViewMatrix ), 4, GL_FALSE, 1 );
	PerspectiveShader.SetUniform( "modelMatrix", glm::value_ptr( PerspModelMatrix ), 4, GL_FALSE, 1 );

	// Drawing in wireframe?
	if( draw_wireframe == true )
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	else
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	// Bind the axis Vertex Array Object created earlier, and draw it
	glBindVertexArray( axis_VAO );
	glDrawArrays( GL_LINES, 0, 6 ); // 6 = number of vertices in the object

	
	for (int i = 0; i < Tori.size(); i++) { //render each entity individually, so a seperate transformation can be applied to it.
		Tori[i]->draw();
	};


	// Unbind when done
	glBindVertexArray( 0 );

	// Swap the front and back buffers
	glutSwapBuffers();
}

/*=================================================================================================
	INIT
=================================================================================================*/

void init( void )
{
	// Print some info
	std::cout << "Vendor:         " << glGetString( GL_VENDOR   ) << "\n";
	std::cout << "Renderer:       " << glGetString( GL_RENDERER ) << "\n";
	std::cout << "OpenGL Version: " << glGetString( GL_VERSION  ) << "\n";
	std::cout << "GLSL Version:   " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << "\n\n";
	// Set OpenGL settings
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f ); // background color
	glEnable( GL_DEPTH_TEST ); // enable depth test
	glEnable( GL_CULL_FACE ); // enable back-face culling

	// Create shaders
	CreateShaders();

	// Create axis buffers
	CreateAxisBuffers();

	//
	// Consider calling a function to create your object here
	
	// PUTTING DATA FROM ENTITY OBJECTS INTO THE DRAW AND COLOR VECTORS TO BE PASSED INTO THE GPU.

	std::cout << "Finished initializing...\n\n";
}

/*=================================================================================================
	MAIN
=================================================================================================*/

int main( int argc, char** argv )
{
	
	// Create and initialize the OpenGL context
	glutInit( &argc, argv );

	glutInitWindowPosition( 100, 100 );
	glutInitWindowSize( InitWindowWidth, InitWindowHeight );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );

	glutCreateWindow( "CSE-170 Computer Graphics" );

	// Initialize GLEW
	GLenum ret = glewInit();
	if( ret != GLEW_OK ) {
		std::cerr << "GLEW initialization error." << std::endl;
		glewGetErrorString( ret );
		return -1;
	}

	// Register callback functions
	glutDisplayFunc( display_func );
	glutIdleFunc( idle_func );
	glutReshapeFunc( reshape_func );
	glutKeyboardFunc( keyboard_func );
	glutKeyboardUpFunc( key_released );
	glutSpecialFunc( key_special_pressed );
	glutSpecialUpFunc( key_special_released );
	glutMouseFunc( mouse_func );
	glutMotionFunc( active_motion_func );
	glutPassiveMotionFunc( passive_motion_func );

	// Do program initialization
	init();

	// Enter the main loop
	glutMainLoop();

	return EXIT_SUCCESS;
}
