#include <vector>
#include <cmath>
#include <iostream>
#include "../tgaimage.h"
#include "../geometry.h"
#include "../model.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);
Model *model = NULL;
const int width = 800;
const int height = 800;

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) { 
    // 先按 y 排序
    if(t0.y > t1.y) { std::swap(t0, t1); }
    if(t0.y > t2.y) { std::swap(t0, t2); }
    if(t1.y > t2.y) { std::swap(t1, t2); }

    // std::cout<< t0.y << t1.y << t2.y << std::endl;
    // 递增 y 画点
    int total_height = t2.y - t0.y;

    for(int y = t0.y; y <= t1.y; y++) {
        // 两条直线一起画
        // 由于 segment_height 可能为 0 所以我们加 1
        int segment_height = t1.y - t0.y + 1;
        float alpha = (float)(y - t0.y) / segment_height;
        float beta = (float)(y - t0.y) / total_height;
        Vec2i A = t0 + (t1 - t0) * alpha;
        Vec2i B = t0 + (t2 - t0) * beta;

        if(A.x > B.x) { std::swap(A, B); }
        // 连接两个点
        for(int x = A.x; x <= B.x; x++) {
            image.set(x, y, color);
        }

    }
    // 画第二部分
    for(int y = t1.y; y <= t2.y; y++) {
        // 两条直线一起画
        int segment_height = t2.y - t1.y + 1;
        float alpha = (float)(y - t1.y) / segment_height;
        float beta = (float)(y - t0.y) / total_height;
        Vec2i A = t1 + (t2 - t1) * alpha;
        Vec2i B = t0 + (t2 - t0) * beta;

        if(A.x > B.x) { std::swap(A, B); }
        // 连接两个点
        for(int x = A.x; x <= B.x; x++) {
            image.set(x, y, color);
        }

    }
    
}

void triangle1(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) { 
    // 先按 y 排序
    if(t0.y > t1.y) { std::swap(t0, t1); }
    if(t0.y > t2.y) { std::swap(t0, t2); }
    if(t1.y > t2.y) { std::swap(t1, t2); }

    // 递增 y 画点
    int total_height = t2.y - t0.y;

    for(int y = t0.y; y <= t1.y; y++) {
        // 两条直线一起画
        // 由于 segment_height 可能为 0 所以我们加 1
        int segment_height = t1.y - t0.y + 1;
        float alpha = (float)(y - t0.y) / segment_height;
        float beta = (float)(y - t0.y) / total_height;
        Vec2i A = t0 + (t1 - t0) * alpha;
        Vec2i B = t0 + (t2 - t0) * beta;
        image.set(A.x, y, white);
        image.set(B.x, y, green);
    }
}

void triangle2(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) { 
    // 先按 y 排序
    if(t0.y > t1.y) { std::swap(t0, t1); }
    if(t0.y > t2.y) { std::swap(t0, t2); }
    if(t1.y > t2.y) { std::swap(t1, t2); }

    // std::cout<< t0.y << t1.y << t2.y << std::endl;
    // 递增 y 画点
    int total_height = t2.y - t0.y;

    for(int y = t0.y; y <= t1.y; y++) {
        // 两条直线一起画
        // 由于 segment_height 可能为 0 所以我们加 1
        int segment_height = t1.y - t0.y + 1;
        float alpha = (float)(y - t0.y) / segment_height;
        float beta = (float)(y - t0.y) / total_height;
        Vec2i A = t0 + (t1 - t0) * alpha;
        Vec2i B = t0 + (t2 - t0) * beta;

        if(A.x > B.x) { std::swap(A, B); }
        // 连接两个点
        for(int x = A.x; x <= B.x; x++) {
            image.set(x, y, white);
        }

    }
}

Vec3f cross_product(Vec3f vect_A, Vec3f vect_B) {

    float x = vect_A[1] * vect_B[2] - vect_A[2] * vect_B[1];
    float y = vect_A[0] * vect_B[2] - vect_A[2] * vect_B[0];
    float z = vect_A[0] * vect_B[1] - vect_A[1] * vect_B[0];
    return Vec3f(x, y, z);
}

int main(int argc, char **argv) {
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("../obj/african_head/african_head.obj");
    }

    TGAImage image(width, height, TGAImage::RGB);
    Vec3f light_dir(0,0,-1);
    for (int i=0; i<model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        Vec2i screen_coords[3];
        Vec3f world_coords[3];
        for (int j=0; j<3; j++) {
            Vec3f v = model->vert(face[j]);
            std::cout<< "v.x" << v.x <<std::endl;
            screen_coords[j] = Vec2i((v.x + 1)*width/2., (v.y + 1)*height/2.);
            world_coords[j]  = v;
        }
        Vec3f n = cross_product(world_coords[2]-world_coords[0], world_coords[1]-world_coords[0]);
        n.normalize();
        std::cout<< 'n' << n[0] <<std::endl;
        float intensity = n*light_dir;
        std::cout<< "intensity " << intensity << std::endl;
        if (intensity>0) {
            std::cout << "intensity * 255 " << int(intensity * 255) << std::endl;
            triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(int(intensity*255), int(intensity*255), int(intensity*255), 255));
        }
    }
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}