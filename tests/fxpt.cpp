#include <iostream>
#include <string>
#include "math/fxpt.h"

using namespace math;

int main() {
    std::cout << "READY" << std::endl;
    std::string cmd;

    while (std::cin >> cmd) {
		fxpt res;

        if (cmd == "exit") {
            break;
        } else if (cmd == "add" || cmd == "sub" || cmd == "mul" || cmd == "div") {
            float a, b;
            std::cin >> a >> b;

            fxpt fa(a), fb(b);

            if (cmd == "add") res = fa + fb;
            else if (cmd == "sub") res = fa - fb;
            else if (cmd == "mul") res = fa * fb;
            else if (cmd == "div") res = fa / fb;
        } else if (cmd == "sqr" || cmd == "sqrt") {
            float a;
            std::cin >> a;

            fxpt fa(a);

            if (cmd == "sqr") res = fa.sqr();
            else if (cmd == "sqrt") res = fa.sqrt();
        } else if (cmd == "sin" || cmd == "cos" || cmd == "acos") {
            float a;
            std::cin >> a;
            
            fxpt fa(a);
            
            if (cmd == "sin") res = fxpt::sin(fa);
            else if (cmd == "cos") res = fxpt::cos(fa);
            else if (cmd == "acos") res = fxpt::acos(fa);
        } else if (cmd == "atan2") {
            float y, x;
            std::cin >> y >> x;
            
            fxpt fy(y), fx(x);
            res = fxpt::atan2(fy, fx);
        }

		std::cout << "RESULT " << static_cast<float>(res) << std::endl;
    }
    return 0;
}
