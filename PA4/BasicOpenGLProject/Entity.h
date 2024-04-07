#pragma once
#include <vector>
#include "Vector.h"

class Entity {
public:
    std::vector<float> mesh;
    std::vector<float> color;
    std::vector<float> debug_normals;
    std::vector<float> debug_normals_colors;
    std::vector<float> normals;
    std::vector<glm::vec2> texture_coordinates;
    Vector rotation;
    Vector position;
    Vector scale;
    unsigned char* texture_data;
    int texture_img_width;
    int texture_img_height;
    int texture_img_channels;
    GLuint textureId;

    void set_texture(const char* path) {
        texture_data = stbi_load(path, &texture_img_width, &texture_img_height, &texture_img_channels, 0);
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_img_width, texture_img_height, 0, GL_RGB,
            GL_UNSIGNED_BYTE, texture_data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    }

    Entity(std::vector<float> _mesh, std::vector<float> _color) {
        for (int i = 0; i < _mesh.size(); i++) {
            mesh.push_back(_mesh[i]);
            color.push_back(_color[i]);
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