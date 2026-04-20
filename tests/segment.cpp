#include <iostream>
#include <iomanip>
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

		std::cout << "test " << std::fixed << (float)seg.point0.a << "," << (float)seg.point0.b << " " << (float)seg.point1.a << "," << (float)seg.point1.b << std::endl;

        if (cmd == "mid") {
            vec2 mid = seg.midpoint();
            std::cout << "RESULT " << std::fixed << (float)mid.a << " " << (float)mid.b << std::endl;
        } else if (cmd == "len") {
            std::cout << "RESULT " << std::fixed << (float)seg.len() << std::endl;
        } else if (cmd == "sqlen") {
            std::cout << "RESULT " << std::fixed << (float)seg.sqr_len() << std::endl;
        }
    }
    return 0;
}
