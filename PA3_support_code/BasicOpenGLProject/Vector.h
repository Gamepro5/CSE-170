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
    Vector operator - (Vector const& obj1) {
        Vector obj2;
        obj2.x = x - obj1.x;
        obj2.y = y - obj1.y;
        obj2.z = z - obj1.z;
        return obj2;
    };
    Vector operator * (Vector const& obj1) {
        Vector obj2;
        obj2.x = x * obj1.x;
        obj2.y = y * obj1.y;
        obj2.z = z * obj1.z;
        return obj2;
    };
    Vector operator + (Vector const& obj1) {
        Vector obj2;
        obj2.x = x + obj1.x;
        obj2.y = y + obj1.y;
        obj2.z = z + obj1.z;
        return obj2;
    };
    Vector operator / (Vector const& obj1) {
        Vector obj2;
        obj2.x = x / obj1.x;
        obj2.y = y / obj1.y;
        obj2.z = z / obj1.z;
        return obj2;
    };
    Vector operator = (Vector const& obj1) {
        x = obj1.x;
        y = obj1.y;
        z = obj1.z;
        return *this;
    };
};