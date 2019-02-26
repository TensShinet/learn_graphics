#include <vector>
#include <cmath>
#include <iostream>
#include "../tgaimage.h"
#include "../geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);
const int width = 200;
const int height = 200;


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
            image.set(x, y, white);
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
            image.set(x, y, white);
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



int main(int argc, char **argv) {
    TGAImage image(width, height, TGAImage::RGB);
    std::cout<<"test"<<std::endl;
    Vec2i t0[3] = {Vec2i(10, 70), Vec2i(50, 160), Vec2i(70, 80)};
    Vec2i t1[3] = {Vec2i(180, 50), Vec2i(150, 1), Vec2i(70, 180)};
    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};

    triangle2(t0[0], t0[1], t0[2], image, red);
    triangle2(t1[0], t1[1], t1[2], image, white);
    triangle2(t2[0], t2[1], t2[2], image, green);
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}