#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#define _USE_MATH_DEFINES
#include <iostream>
#include <cmath>
#include <math.h>
#include "shader.h"
#include "shaderprogram.h"
#include <vector>
#include "Vector.h"
#include "Entity.h"

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
/*=================================================================================================
	SHADERS & TRANSFORMATIONS
=================================================================================================*/

ShaderProgram PassthroughShader;
ShaderProgram PerspectiveShader;

glm::mat4 PerspProjectionMatrix( 1.0f );
glm::mat4 PerspViewMatrix( 1.0f );
glm::mat4 PerspModelMatrix( 1.0f );

float perspZoom = 1.0f, perspSensitivity = 0.35f;
float perspRotationX = 0.0f, perspRotationY = 0.0f;

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

GLuint draw_VAO;
GLuint draw_VBO[2];

std::vector<float> draw_verticies{};

std::vector<float> draw_colors{};

Entity* J = new Entity({
	//1
	-9.0f,  3.0f,  0.0f, 1.0f,
	-9.0f,  1.0f,  0.0f, 1.0f,
	-8.5f,  0.0f,  0.0f, 1.0f,
	//2
	-8.5f,  0.0f,  0.0f, 1.0f,
	-6.0f,  0.0f,  0.0f, 1.0f,
	-6.0f,  1.0f,  0.0f, 1.0f,
	//3
	-6.5f,  3.0f,  0.0f, 1.0f,
	-6.0f,  1.0f,  0.0f, 1.0f,
	-6.0f,  3.0f,  0.0f, 1.0f,
	//4
	-6.0f,  6.0f,  0.0f, 1.0f,
	-6.5f,  3.0f,  0.0f, 1.0f,
	-6.0f,  3.0f,  0.0f, 1.0f,
	//5
	-8.0f,  7.0f,  0.0f, 1.0f,
	-8.0f,  6.0f,  0.0f, 1.0f,
	-4.0f,  6.0f,  0.0f, 1.0f,
	//6
	-4.0f,  7.0f,  0.0f, 1.0f,
	-8.0f,  7.0f,  0.0f, 1.0f,
	-4.0f,  6.0f,  0.0f, 1.0f,
	},
	{
	1.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,

	0.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,

	1.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,

	0.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,

	1.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,

	0.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
	});

Entity* U = new Entity({
	//7
	-3.0f,  3.0f,  0.0f, 1.0f,
	-4.0f,  3.0f,  0.0f, 1.0f,
	-2.0f,  1.0f,  0.0f, 1.0f,
	//8
	0.0f,  1.0f,  0.0f, 1.0f,
	-2.0f,  1.0f,  0.0f, 1.0f,
	-1.0f,  0.0f,  0.0f, 1.0f,
	//9
	1.5f,  3.0f,  0.0f, 1.0f,
	0.5f,  3.0f,  0.0f, 1.0f,
	0.0f,  1.0f,  0.0f, 1.0f,
	}, {
	1.0f,  0.0f,  0.0f, 1.0f,
	0.0f,  1.0f,  0.0f, 1.0f,
	0.0f,  0.0f,  1.0f, 1.0f,

	1.0f,  0.0f,  0.0f, 1.0f,
	0.0f,  1.0f,  0.0f, 1.0f,
	0.0f,  0.0f,  1.0f, 1.0f,

	1.0f,  0.0f,  0.0f, 1.0f,
	0.0f,  1.0f,  0.0f, 1.0f,
	0.0f,  0.0f,  1.0f, 1.0f,
	});


Entity* L = new Entity({
	//10
	3.0f,  7.0f,  0.0f, 1.0f,
	3.0f,  0.0f,  0.0f, 1.0f,
	4.0f,  0.0f,  0.0f, 1.0f,
	//11
	4.0f,  7.0f,  0.0f, 1.0f,
	3.0f,  7.0f,  0.0f, 1.0f,
	4.0f,  0.0f,  0.0f, 1.0f,
	//12
	4.0f,  1.0f,  0.0f, 1.0f,
	4.0f,  0.0f,  0.0f, 1.0f,
	6.0f,  0.0f,  0.0f, 1.0f,
	//13
	6.0f,  1.0f,  0.0f, 1.0f,
	4.0f,  1.0f,  0.0f, 1.0f,
	6.0f,  0.0f,  0.0f, 1.0f,
	}, {
	1.0f,  0.0f,  0.0f, 1.0f,
	0.0f,  1.0f,  0.0f, 1.0f,
	0.0f,  0.0f,  1.0f, 1.0f,

	1.0f,  0.0f,  0.0f, 1.0f,
	0.0f,  1.0f,  0.0f, 1.0f,
	0.0f,  0.0f,  1.0f, 1.0f,

	1.0f,  0.0f,  0.0f, 1.0f,
	0.0f,  1.0f,  0.0f, 1.0f,
	0.0f,  0.0f,  1.0f, 1.0f,

	1.0f,  0.0f,  0.0f, 1.0f,
	0.0f,  1.0f,  0.0f, 1.0f,
	0.0f,  0.0f,  1.0f, 1.0f,
	}
	);

Entity* I = new Entity({
	//14
	8.0f,  4.0f,  0.0f, 1.0f,
	8.0f,  0.0f,  0.0f, 1.0f,
	9.0f,  0.0f,  0.0f, 1.0f,
	//15
	9.0f,  4.0f,  0.0f, 1.0f,
	8.0f,  4.0f,  0.0f, 1.0f,
	9.0f,  0.0f,  0.0f, 1.0f,
	//16
	8.5f,  6.0f,  0.0f, 1.0f,
	8.0f,  5.0f,  0.0f, 1.0f,
	9.0f,  5.0f,  0.0f, 1.0f,
	}, {
	1.0f,  0.0f,  0.0f, 1.0f,
	0.0f,  1.0f,  0.0f, 1.0f,
	0.0f,  0.0f,  1.0f, 1.0f,

	1.0f,  0.0f,  0.0f, 1.0f,
	0.0f,  1.0f,  0.0f, 1.0f,
	0.0f,  0.0f,  1.0f, 1.0f,

	1.0f,  0.0f,  0.0f, 1.0f,
	0.0f,  1.0f,  0.0f, 1.0f,
	0.0f,  0.0f,  1.0f, 1.0f,
	});

std::vector<Entity*> Entities{ J, U, L, I };





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

void CreateDrawBuffers(void)
{
	glGenVertexArrays(1, &draw_VAO); //generate 1 new VAO, its ID is returned in axis_VAO
	glBindVertexArray(draw_VAO); //bind the VAO so the subsequent commands modify it

	glGenBuffers(2, &draw_VBO[0]); //generate 2 buffers for data, their IDs are returned to the axis_VBO array

	// first buffer: vertex coordinates
	glBindBuffer(GL_ARRAY_BUFFER, draw_VBO[0]); //bind the first buffer using its ID
	glBufferData(GL_ARRAY_BUFFER, sizeof(draw_verticies[0]) * draw_verticies.size(), &draw_verticies[0], GL_STATIC_DRAW); //send coordinate array to the GPU
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); //let GPU know this is attribute 0, made up of 4 floats
	glEnableVertexAttribArray(0);

	// second buffer: colors
	glBindBuffer(GL_ARRAY_BUFFER, draw_VBO[1]); //bind the second buffer using its ID
	glBufferData(GL_ARRAY_BUFFER, sizeof(draw_colors[0]) * draw_colors.size(), &draw_colors[0], GL_STATIC_DRAW); //send color array to the GPU
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); //let GPU know this is attribute 1, made up of 4 floats
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); //unbind when done

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

void keyboard_func( unsigned char key, int x, int y )
{
	key_states[ key ] = true;

	switch( key )
	{
		case 'w':
		{
			draw_wireframe = !draw_wireframe;
			if( draw_wireframe == true )
				std::cout << "Wireframes on.\n";
			else
				std::cout << "Wireframes off.\n";
			break;
		}

		// Exit on escape key press
		case '\x1B':
		{
			exit( EXIT_SUCCESS );
			break;
		}
	}
}

void key_released( unsigned char key, int x, int y )
{
	key_states[ key ] = false;
}

void key_special_pressed( int key, int x, int y )
{
	key_special_states[ key ] = true;
}

void key_special_released( int key, int x, int y )
{
	key_special_states[ key ] = false;
}

void mouse_func( int button, int state, int x, int y )
{
	// Key 0: left button
	// Key 1: middle button
	// Key 2: right button
	// Key 3: scroll up
	// Key 4: scroll down

	if( x < 0 || x > WindowWidth || y < 0 || y > WindowHeight )
		return;

	float px, py;
	window_to_scene( x, y, px, py );

	if( button == 3 )
	{
		perspZoom += 0.03f;
	}
	else if( button == 4 )
	{
		if( perspZoom - 0.03f > 0.0f )
			perspZoom -= 0.03f;
	}

	mouse_states[ button ] = ( state == GLUT_DOWN );

	LastMousePosX = x;
	LastMousePosY = y;
}

void passive_motion_func( int x, int y )
{
	if( x < 0 || x > WindowWidth || y < 0 || y > WindowHeight )
		return;

	float px, py;
	window_to_scene( x, y, px, py );

	LastMousePosX = x;
	LastMousePosY = y;
}

void active_motion_func( int x, int y )
{
	if( x < 0 || x > WindowWidth || y < 0 || y > WindowHeight )
		return;

	float px, py;
	window_to_scene( x, y, px, py );

	if( mouse_states[0] == true )
	{
		perspRotationY += ( x - LastMousePosX ) * perspSensitivity;
		perspRotationX += ( y - LastMousePosY ) * perspSensitivity;
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

	
	for (int i = 0; i < Entities.size(); i++) {
		draw_verticies.clear();
		draw_colors.clear();
		for (int j = 0; j < Entities[i]->mesh.size(); j++) {
			draw_verticies.push_back(Entities[i]->mesh[j]);
			draw_colors.push_back(Entities[i]->color[j]);
		};
		CreateDrawBuffers();
		glm::mat4 rotationMatrixX (
			{ 1.0f, 0.0f, 0.0f, 0.0f },
			{ 0.0f, std::cos(Entities[i]->rotation.x), -std::sin(Entities[i]->rotation.x), 0.0f },
			{ 0.0f, std::sin(Entities[i]->rotation.x), std::cos(Entities[i]->rotation.x), 0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f }
		);
		glm::mat4 rotationMatrixY (
			{ std::cos(Entities[i]->rotation.y), 0.0f, std::sin(Entities[i]->rotation.y), 0.0f },
			{ 0.0f, 1.0f, 0.0f, 0.0f },
			{ -std::sin(Entities[i]->rotation.y), 0.0f, std::cos(Entities[i]->rotation.y), 0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f }
		);
		glm::mat4 rotationMatrixZ (
			{ std::cos(Entities[i]->rotation.z), -std::sin(Entities[i]->rotation.z), 0.0f, 0.0f},
			{ std::sin(Entities[i]->rotation.z), std::cos(Entities[i]->rotation.z), 0.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f }
		);
		glm::mat4 scaleMatrix (
			{ Entities[i]->scale.x, 0.0f, 0.0f, 0.0f },
			{ 0.0f, Entities[i]->scale.y, 0.0f, 0.0f },
			{ 0.0f, 0.0f, Entities[i]->scale.z, 0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f }
		);
		glm::mat4 translationMatrix (
			{ 1.0f, 0.0f, 0.0f, Entities[i]->position.x },
			{ 0.0f, 1.0f, 0.0f, Entities[i]->position.y },
			{ 0.0f, 0.0f, 1.0f, Entities[i]->position.z },
			{ 0.0f, 0.0f, 0.0f, 1.0f }
		);
		
		PerspectiveShader.SetUniform("modelMatrix", glm::value_ptr(rotationMatrixZ * rotationMatrixY * rotationMatrixX * scaleMatrix * translationMatrix * PerspModelMatrix), 4, GL_FALSE, 1);
		//PerspectiveShader.SetUniform("modelMatrix", glm::value_ptr(scaleMatrix * PerspModelMatrix), 4, GL_FALSE, 1);
		glBindVertexArray(draw_VAO);
		glDrawArrays(GL_TRIANGLES, 0, draw_verticies.size());
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
	//U->rotation.y = 45 * toRadians;
	//U->position.y = 2;
	U->scale.y = 2;
	U->scale.x = 1;
	U->scale.z = 1;
	// PUTTING DATA FROM ENTITY OBJECTS INTO THE DRAW AND COLOR VECTORS TO BE PASSED INTO THE GPU.
	CreateDrawBuffers();

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
