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

    Vector calculateNormal(Vector p1, Vector p2, Vector p3) {
        Vector U = p2 - p1;
        Vector V = p3 - p1;
        Vector result = Vector((U.y * V.z) - (U.z * V.y), (U.z * V.x) - (U.x * V.z), (U.x * V.y) - (U.y * V.x));
        return result;
    };
public:
    float radius = 2;
    float inner_radius = 0.5;
    float vertexCount = 500;
    std::vector<Triangle> triangles;
    std::vector<Vector> verticies;
    std::vector<float> normals;
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
       
        std::vector<std::vector<Vector>> rings;
        for (float theta = 0; theta < 2*M_PI; theta+= 2 * M_PI/sqrt(vertexCount)) {
            std::vector<Vector> ring;
            for (float phi = 0; phi < 2 * M_PI; phi+= 2 * M_PI / sqrt(vertexCount)) {
                ring.push_back(Vector((radius + inner_radius * cos(theta)) * cos(phi), (radius + inner_radius * cos(theta)) * sin(phi), inner_radius * sin(theta)));
            }
            rings.push_back(ring);
        }
        for (int i = 0; i < rings.size()-1; i++) {
            for (int j = 0; j < rings[i].size()-1; j++) {
                auto t = Triangle();
                t.x = rings[i][j];
                t.y = rings[i][j + 1];
                t.z = rings[i + 1][j];

                triangles.push_back(t);

                auto t2 = Triangle();
                t2.x = rings[i+1][j+1];
                t2.y = rings[i+1][j];
                t2.z = rings[i][j+1];

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
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);
            normals.push_back(1.0f);
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);
            normals.push_back(1.0f);
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);
            normals.push_back(1.0f);
        }
        
    }
    
    Torus() {
        Construct();
    }
};