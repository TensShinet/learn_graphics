#include "tgaimage.h"
#include <iostream>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor fuck = TGAColor(0.1, 0.1, 0.1, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
int main(int argc, char **argv)
{
    TGAImage image(100, 100, TGAImage::RGB);
    std::cout<<"test"<<std::endl;
    for(int i = 0; i < 100; i++) {
        image.set(0, i, fuck);
        image.set(i, 0, red);
    }
    
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}