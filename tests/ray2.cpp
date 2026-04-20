#include <iostream>
#include <iomanip>
#include <string>
#include "math/ray2.h"

using namespace math;

int main() {
    std::cout << "READY" << std::endl;
    std::string cmd;

    while (std::cin >> cmd) {
        if (cmd == "exit") break;

        if (cmd == "rot") {
            float ex, ey, dx, dy, angle;
            std::cin >> ex >> ey >> dx >> dy >> angle;
            ray2 r(vec2(ex, ey), vec2(dx, dy));
            ray2 res = ray2::rotate(r, fxpt(angle));
            std::cout << "RESULT " << std::fixed << (float)res.direction.a << " " << (float)res.direction.b << std::endl;
        }
    }
    return 0;
}
