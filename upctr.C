#include <iostream>

#include "rose.h"

using namespace std;

int main(int argc, char* argv[])
{
    SgProject* proj = frontend(argc, argv);


    return backend(proj);
}
