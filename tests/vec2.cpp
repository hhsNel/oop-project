#include <iostream>
#include <iomanip>
#include <string>
#include "math/vec2.h"

using namespace math;

int main() {
    std::cout << "READY" << std::endl;
    std::string cmd;

    while (std::cin >> cmd) {
        if (cmd == "exit") break;

        float a1, b1, a2, b2, s;
        vec2 res;

        if (cmd == "add" || cmd == "sub") {
            std::cin >> a1 >> b1 >> a2 >> b2;
            vec2 v1(a1, b1), v2(a2, b2);
            res = (cmd == "add") ? v1 + v2 : v1 - v2;
        } 
        else if (cmd == "mul" || cmd == "div") {
            std::cin >> a1 >> b1 >> s;
            vec2 v1(a1, b1);
            res = (cmd == "mul") ? v1 * fxpt(s) : v1 / fxpt(s);
        }
        else if (cmd == "dot") {
            std::cin >> a1 >> b1 >> a2 >> b2;
            fxpt dot = vec2::dot_product(vec2(a1, b1), vec2(a2, b2));
            std::cout << "RESULT " << std::fixed << static_cast<float>(dot) << std::endl;
            continue;
        }
        else if (cmd == "len" || cmd == "sqlen") {
            std::cin >> a1 >> b1;
            vec2 v(a1, b1);
            float val = (cmd == "len") ? static_cast<float>(v.len()) : static_cast<float>(v.sqr_len());
            std::cout << "RESULT " << std::fixed << val << std::endl;
            continue;
        }

        std::cout << "RESULT " << std::fixed << static_cast<float>(res.a) << " " << static_cast<float>(res.b) << std::endl;
    }
    return 0;
}
