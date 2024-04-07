#pragma once
#include "Vector.h"

class Triangle {
public:
    Vector x;
    Vector y;
    Vector z;

    Triangle() {
        x = Vector();
        y = Vector();
        z = Vector();
    }

    Triangle(Vector _x, Vector _y, Vector _z) {
        x = _x;
        y = _y;
        z = _z;
    };
    void print() {
        std::cout << "Triangle Consists of: Vector(" << x.x << ", " << x.y << ", " << x.z << "), " << "Vector(" << x.x << ", " << y.y << ", " << y.z << "), " << "Vector(" << z.x << ", " << z.y << ", " << z.z << "), " << std::endl;
    }
};