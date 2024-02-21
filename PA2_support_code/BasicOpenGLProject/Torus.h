#pragma once
#define _USE_MATH_DEFINES
#include <vector>
#include "Vector.h"
#include "Entity.h"
#include <math.h>



class Torus : public Entity {
public:
    float radius = 1;
    float inner_radius = 0.5;
    float triangles = 1000;

    void addTriangles() {
        triangles += 50;
    };

    void removeTriangles() {
        triangles -= 50;
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
        mesh.clear();
        color.clear();
        for (float theta = 0; theta < 2*M_PI; theta+= 2 * M_PI/sqrt(triangles)) {
            for (float phi = 0; phi < 2 * M_PI; phi+= 2 * M_PI / sqrt(triangles)) {
                mesh.push_back((radius+inner_radius*cos(theta))*cos(phi)); //x
                mesh.push_back((radius + inner_radius * cos(theta)) * sin(phi)); //y
                mesh.push_back(inner_radius*sin(theta)); //z
                mesh.push_back(1.0);

                color.push_back(1.0);
                color.push_back(0.0);
                color.push_back(0.0);
                color.push_back(1.0);
            }
        }
    }
    
    Torus() {

    }
};