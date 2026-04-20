#include <iostream>
#include <iomanip>
#include <string>
#include "math/vec3.h"

using namespace math;

int main() {
    std::cout << "READY" << std::endl;
    std::string cmd;

    while (std::cin >> cmd) {
        if (cmd == "exit") break;

        if (cmd == "add" || cmd == "sub" || cmd == "cross") {
            float x1, y1, z1, x2, y2, z2;
            std::cin >> x1 >> y1 >> z1 >> x2 >> y2 >> z2;
            vec3 v1(x1, y1, z1), v2(x2, y2, z2);
            vec3 res = (cmd == "add") ? v1 + v2 : (cmd == "sub") ? v1 - v2 : vec3::cross_product(v1, v2);
            std::cout << "RESULT " << std::fixed << (float)res.a << " " << (float)res.b << " " << (float)res.c << std::endl;
        }
        else if (cmd == "mul" || cmd == "div") {
            float x, y, z, s;
            std::cin >> x >> y >> z >> s;
            vec3 v(x, y, z);
            vec3 res = (cmd == "mul") ? v * fxpt(s) : v / fxpt(s);
            std::cout << "RESULT " << std::fixed << (float)res.a << " " << (float)res.b << " " << (float)res.c << std::endl;
        }
        else if (cmd == "dot") {
            float x1, y1, z1, x2, y2, z2;
            std::cin >> x1 >> y1 >> z1 >> x2 >> y2 >> z2;
            std::cout << "RESULT " << std::fixed << (float)vec3::dot_product(vec3(x1, y1, z1), vec3(x2, y2, z2)) << std::endl;
        }
    }
    return 0;
}
