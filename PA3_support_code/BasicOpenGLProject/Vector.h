#pragma once

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
    void print() {
        std::cout << "Vector(" << x << ", " << y << ", " << z << ")" << std::endl;
    }
};