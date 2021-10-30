#include "obj.h"
#include "tga.h"

int main(int argc, char** argv) {
    let obj = obj_load("head.obj");
    let tga = tga_init(1000, 1000);
    obj_free(&obj);
    tga_free(&tga);
}
