#include <iostream>

#include "array.hpp"
#include "print.hpp"



int main() {

    Array<float, 3> arr = a_(1.6f,2,3.f);
    print("cool", arr);

    return 0;
}