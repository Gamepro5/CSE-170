#pragma once

class Triangle {
public:
    glm::vec4 x;
    glm::vec4 y;
    glm::vec4 z;

    Triangle() {
        x = glm::vec4();
        y = glm::vec4();
        z = glm::vec4();
    }

    Triangle(glm::vec4 _x, glm::vec4 _y, glm::vec4 _z) {
        x = _x;
        y = _y;
        z = _z;
    };
    void print() {
        std::cout << "Triangle Consists of: Vector(" << x.x << ", " << x.y << ", " << x.z << "), " << "Vector(" << x.x << ", " << y.y << ", " << y.z << "), " << "Vector(" << z.x << ", " << z.y << ", " << z.z << "), " << std::endl;
    }
};