#pragma once
#define _USE_MATH_DEFINES
#include <vector>
#include "Entity.h"
#include <math.h>
#include <vector>
#include "Triangle.h"



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
        verticies.clear();
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
        set_texture("stb/pav_food.jpg");
        Construct();
    }
};