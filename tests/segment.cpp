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

		std::cout << "test " << std::fixed << (float)seg.point0.x << "," << (float)seg.point0.y << " " << (float)seg.point1.x << "," << (float)seg.point1.y << std::endl;

        if (cmd == "mid") {
            vec2 mid = seg.midpoint();
            std::cout << "RESULT " << std::fixed << (float)mid.x << " " << (float)mid.y << std::endl;
        } else if (cmd == "len") {
            std::cout << "RESULT " << std::fixed << (float)seg.len() << std::endl;
        } else if (cmd == "sqlen") {
            std::cout << "RESULT " << std::fixed << (float)seg.sqr_len() << std::endl;
        }
    }
    return 0;
}
