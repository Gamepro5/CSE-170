#pragma once
#include <vector>
#include "Vector.h"

class Entity {
public:             
    std::vector<float> mesh;
    std::vector<float> color;
    Vector rotation;
    Vector position;
    Vector scale;

    Entity(std::vector<float> _mesh, std::vector<float> _color) {
        for (int i = 0; i < _mesh.size(); i++) {
            mesh.push_back(_mesh[i]);
            color.push_back(_color[i]);
        }  
    };
};