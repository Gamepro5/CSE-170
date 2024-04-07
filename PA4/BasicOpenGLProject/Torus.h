#pragma once
#define _USE_MATH_DEFINES
#include <vector>
#include "Vector.h"
#include "Entity.h"
#include <math.h>
#include <vector>
#include "Triangle.h"



class Torus : public Entity {
private:
    void addDebugNormal(Vector start, Vector end) {
        debug_normals.push_back(start.x);
        debug_normals.push_back(start.y);
        debug_normals.push_back(start.z);
        debug_normals.push_back(1.0);
        debug_normals.push_back(start.x + end.x);
        debug_normals.push_back(start.y + end.y);
        debug_normals.push_back(start.z + end.z);
        debug_normals.push_back(1.0);

        debug_normals_colors.push_back(1.0);
        debug_normals_colors.push_back(0.64);
        debug_normals_colors.push_back(0.0);
        debug_normals_colors.push_back(1.0);
        debug_normals_colors.push_back(1.0);
        debug_normals_colors.push_back(0.64);
        debug_normals_colors.push_back(0.0);
        debug_normals_colors.push_back(1.0);
    }
    Vector calculateNormal(Vector p1, Vector p2, Vector p3) {
        Vector U = p2 - p1;
        Vector V = p3 - p1;
        Vector result = Vector((U.y * V.z) - (U.z * V.y), (U.z * V.x) - (U.x * V.z), (U.x * V.y) - (U.y * V.x));
        return result;
    };
public:
    bool smooth_shading = false;
    float radius = 2;
    float inner_radius = 0.5;
    float vertexCount = 500;
    std::vector<Triangle> triangles;
    std::vector<Vector> verticies;
    std::vector<Vector> flat_normals;
    std::vector<Vector> smooth_normals;
    std::vector<Vector> ringCenters;
    
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
        mesh.clear();
        color.clear();
        triangles.clear();
        verticies.clear();
        normals.clear();
        smooth_normals.clear();
        flat_normals.clear();
        ringCenters.clear();
        debug_normals.clear();
        debug_normals_colors.clear();
        texture_coordinates.clear();

        std::vector<std::vector<Vector>> rings;
        for (float theta = 0; theta < 2*M_PI; theta+= 2 * M_PI/sqrt(vertexCount)) {
            std::vector<Vector> ring;
            for (float phi = 0; phi < 2 * M_PI; phi+= 2 * M_PI / sqrt(vertexCount)) {
                ring.push_back(Vector((radius + inner_radius * cos(theta)) * cos(phi), (radius + inner_radius * cos(theta)) * sin(phi), inner_radius * sin(theta)));
            }
            rings.push_back(ring);
        }
        for (float phi = 0; phi < 2 * M_PI; phi += 2 * M_PI / sqrt(vertexCount)) {
            ringCenters.push_back(Vector(radius * cos(phi), radius * sin(phi), 0));
        }
        for (int i = 0; i < rings.size(); i++) {
            for (int j = 0; j < rings[i].size(); j++) {
                int modulo_i = (i + 1) % rings.size();
                int modulo_j = (j + 1) % rings[i].size();
                auto t = Triangle();
                t.x = rings[i][j];
                smooth_normals.push_back(rings[i][j] - ringCenters[j]);
                addDebugNormal(rings[i][j], (rings[i][j] - ringCenters[j]).normalized());
                t.y = rings[i][modulo_j];
                smooth_normals.push_back(rings[i][modulo_j] - ringCenters[modulo_j]);
                addDebugNormal(rings[i][modulo_j], (rings[i][modulo_j] - ringCenters[modulo_j]).normalized());
                t.z = rings[modulo_i][j];
                smooth_normals.push_back(rings[modulo_i][j] - ringCenters[j]);
                addDebugNormal(rings[modulo_i][j], (rings[modulo_i][j] - ringCenters[j]).normalized());
                triangles.push_back(t);

                auto t2 = Triangle();
                t2.x = rings[modulo_i][modulo_j];
                smooth_normals.push_back(rings[modulo_i][modulo_j] - ringCenters[modulo_j]);
                addDebugNormal(rings[modulo_i][modulo_j], (rings[modulo_i][modulo_j] - ringCenters[modulo_j]).normalized());
                t2.y = rings[modulo_i][j];
                smooth_normals.push_back(rings[modulo_i][j] - ringCenters[j]);
                addDebugNormal(rings[modulo_i][j], (rings[modulo_i][j] - ringCenters[j]).normalized());
                t2.z = rings[i][modulo_j];
                smooth_normals.push_back(rings[i][modulo_j] - ringCenters[modulo_j]);
                addDebugNormal(rings[i][modulo_j], (rings[i][modulo_j] - ringCenters[modulo_j]).normalized());

                triangles.push_back(t2);
            }
        }

        

        for (int i = 0; i < triangles.size(); i++) {
           
            mesh.push_back(triangles[i].x.x);
            mesh.push_back(triangles[i].x.y);
            mesh.push_back(triangles[i].x.z);
            mesh.push_back(1.0);

            color.push_back(1.0);
            color.push_back(0.0);
            color.push_back(0.0);
            color.push_back(1.0);
            
            mesh.push_back(triangles[i].y.x);
            mesh.push_back(triangles[i].y.y);
            mesh.push_back(triangles[i].y.z);
            mesh.push_back(1.0);

            color.push_back(1.0);
            color.push_back(0.0);
            color.push_back(0.0);
            color.push_back(1.0);

            mesh.push_back(triangles[i].z.x);
            mesh.push_back(triangles[i].z.y);
            mesh.push_back(triangles[i].z.z);
            mesh.push_back(1.0);

            color.push_back(1.0);
            color.push_back(0.0);
            color.push_back(0.0);
            color.push_back(1.0);

            Vector normal = calculateNormal(triangles[i].x, triangles[i].y, triangles[i].z);
            verticies.push_back(triangles[i].x);
            verticies.push_back(triangles[i].y);
            verticies.push_back(triangles[i].z);
            flat_normals.push_back(normal);
            flat_normals.push_back(normal);
            flat_normals.push_back(normal);
           
        }
        if (smooth_shading == true) {
            for (int i = 0; i < smooth_normals.size(); i++) {
                
                    normals.push_back(smooth_normals[i].x);
                    normals.push_back(smooth_normals[i].y);
                    normals.push_back(smooth_normals[i].z);
                    normals.push_back(1.0);
                
            }
            
        }
        else {
            debug_normals.clear();
            debug_normals_colors.clear();
            for (int i = 0; i < flat_normals.size(); i++) {
                    
                    addDebugNormal(verticies[i], flat_normals[i].normalized());

                    normals.push_back(flat_normals[i].x);
                    normals.push_back(flat_normals[i].y);
                    normals.push_back(flat_normals[i].z);
                    normals.push_back(1.0);
                
            }
        }
        for (int i = 0; i < mesh.size(); i++) {
            glm::vec2 temp;
            temp.x = 0;
            temp.y = 1;
            texture_coordinates.push_back(temp);
        }
    }
    
    Torus() {
        set_texture("textures/sam.jpg");
        Construct();
    }
};