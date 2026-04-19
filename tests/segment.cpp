#include <iostream>
#include <string>
#include "math/segment.h"

using namespace math;

int main() {
    std::cout << "READY" << std::endl;
    std::string cmd;

    while (std::cin >> cmd) {
        if (cmd == "exit") break;

        float x1, y1, x2, y2;
        std::cin >> x1 >> y1 >> x2 >> y2;
        segment seg(vec2(x1, y1), vec2(x2, y2));

        if (cmd == "mid") {
            vec2 mid = seg.midpoint();
            std::cout << "RESULT " << (float)mid.a << " " << (float)mid.b << std::endl;
        } else if (cmd == "len") {
            std::cout << "RESULT " << (float)seg.len() << std::endl;
        } else if (cmd == "sqlen") {
            std::cout << "RESULT " << (float)seg.sqr_len() << std::endl;
        }
    }
    return 0;
}