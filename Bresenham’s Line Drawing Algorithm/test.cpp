#include "../tgaimage.h"
#include "../model.h"
#include <iostream>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);
Model *model = NULL;
const int width = 800;
const int height = 800;

void point(int x, int y, TGAImage &image, TGAColor color) {
    image.set(x, y, color);
}


void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    // 检查是否是大斜率
    bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);
    if(steep) {
        // 变换
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    // 限制只能从左往右画
    if(x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;  
    int derr = 2 * std::abs(abs(dy));
    // std::cout << "derr" << derr << std::endl;
    int err = 0;

    int ystep = y0 < y1 ? 1 : -1;
    // 开始画图
    int y = y0;
    for(int x = x0; x <= x1; x++) {
        if(steep) {
            point(y, x, image, color);
        } else {
            point(x, y, image, color);
        }

        err += derr;
        // std::cout << err << std::endl;
        if(err > dx) {
            y += ystep;
            err -= 2 * dx;
        }
    }
}

void line1(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    int dy = y1 -y0;
    int dx = x1 - x0;
    float s = float(dy) / float(dx);
    for(int x = x0; x <= x1; x++) {
        int y = s * (x-x0) + y0;
        point(x, y, image, color);
    }
    
}


void line2(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    if(x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dy = y1 -y0;
    int dx = x1 - x0;
    float derr = std::abs(float(dy) / float(dx));
    float err = .0;

    int y = y0;
    int ystep = y0 < y1 ? 1 : -1;
    for(int x = x0; x <= x1; x++) {
        point(x, y, image, color);

        err += derr;
        if(err > 0.5) {
            y += ystep;
            err -= 1;
        }
    }
    
}

int main(int argc, char **argv) {
    TGAImage image(width, height, TGAImage::RGB);
    std::cout<<"执行"<<std::endl;
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("../obj/african_head/african_head.obj");
    }

    for (int i=0; i<model->nfaces(); i++) { 
        std::vector<int> face = model->face(i); 
        for (int j=0; j<3; j++) { 
            Vec3f v0 = model->vert(face[j]); 
            Vec3f v1 = model->vert(face[(j+1)%3]); 
            int x0 = (v0.x+1.)*width/2.; 
            int y0 = (v0.y+1.)*height/2.; 
            int x1 = (v1.x+1.)*width/2.; 
            int y1 = (v1.y+1.)*height/2.;
            line(x0, y0, x1, y1, image, white); 
        } 
    }
    image.flip_vertically();
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}

