#pragma once
#include <vector>
#include "Vector.h"

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
        texture_path = std::string(path);
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
		PerspectiveTextureLightShader.Use();
		PerspectiveTextureLightShader.SetUniform("projectionMatrix", glm::value_ptr(PerspProjectionMatrix), 4, GL_FALSE, 1);
		PerspectiveTextureLightShader.SetUniform("viewMatrix", glm::value_ptr(PerspViewMatrix), 4, GL_FALSE, 1);
		PerspectiveTextureLightShader.SetUniform("modelMatrix", glm::value_ptr(PerspModelMatrix * scaleMatrix * translationMatrix * rotationMatrixZ * rotationMatrixY * rotationMatrixX), 4, GL_FALSE, 1);



		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_img_width, texture_img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		std::cout << textureId << std::endl;
		PerspectiveTextureLightShader.SetUniform("texId", textureId);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId);

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