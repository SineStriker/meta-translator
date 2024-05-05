#include <iostream>

#include <mtccore/format.h>

int main(int argc, char *argv[]) {
    int a = 2;
    std::cout << MTC::formatTextN("%1, %2, %3", "12", a, 2.0) << std::endl;
    // std::cout << MTC::Private::to_string_impl("32") << std::endl;
    return 0;
}