#include <vector>
#include <cmath>
#include <iostream>
#include <limits>
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

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color)
{
    // 先按 y 排序
    if (t0.y > t1.y)
    {
        std::swap(t0, t1);
    }
    if (t0.y > t2.y)
    {
        std::swap(t0, t2);
    }
    if (t1.y > t2.y)
    {
        std::swap(t1, t2);
    }

    // std::cout<< t0.y << t1.y << t2.y << std::endl;
    // 递增 y 画点
    int total_height = t2.y - t0.y;

    for (int y = t0.y; y <= t1.y; y++)
    {
        // 两条直线一起画
        // 由于 segment_height 可能为 0 所以我们加 1
        int segment_height = t1.y - t0.y + 1;
        float alpha = (float)(y - t0.y) / segment_height;
        float beta = (float)(y - t0.y) / total_height;
        Vec2i A = t0 + (t1 - t0) * alpha;
        Vec2i B = t0 + (t2 - t0) * beta;

        if (A.x > B.x)
        {
            std::swap(A, B);
        }
        // 连接两个点
        for (int x = A.x; x <= B.x; x++)
        {
            image.set(x, y, color);
        }
    }
    // 画第二部分
    for (int y = t1.y; y <= t2.y; y++)
    {
        // 两条直线一起画
        int segment_height = t2.y - t1.y + 1;
        float alpha = (float)(y - t1.y) / segment_height;
        float beta = (float)(y - t0.y) / total_height;
        Vec2i A = t1 + (t2 - t1) * alpha;
        Vec2i B = t0 + (t2 - t0) * beta;

        if (A.x > B.x)
        {
            std::swap(A, B);
        }
        // 连接两个点
        for (int x = A.x; x <= B.x; x++)
        {
            image.set(x, y, color);
        }
    }
}

void line2(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    // 检查是否是大斜率
    bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);
    if (steep)
    {
        // 变换
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    // 限制只能从左往右画
    if (x0 > x1)
    {
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
    for (int x = x0; x <= x1; x++)
    {
        if (steep)
        {
            image.set(y, x, color);
        }
        else
        {
            image.set(x, y, color);
        }

        err += derr;
        // std::cout << err << std::endl;
        if (err > dx)
        {
            y += ystep;
            err -= 2 * dx;
        }
    }
}

Vec3f cross_product(Vec3f vect_A, Vec3f vect_B)
{

    float x = vect_A[1] * vect_B[2] - vect_A[2] * vect_B[1];
    float y = vect_A[0] * vect_B[2] - vect_A[2] * vect_B[0];
    float z = vect_A[0] * vect_B[1] - vect_A[1] * vect_B[0];
    return Vec3f(x, y, z);
}

void line(Vec2i p, Vec2i p1, TGAImage &image, TGAColor color)
{
    line2(p[0], p[1], p1[0], p1[1], image, color);
}

void rasterize(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color, int ybuffer[])
{
    if (p0.x > p1.x)
    {
        std::swap(p0, p1);
    }
    for (int x = p0.x; x <= p1.x; x++)
    {
        float t = (x - p0.x) / (float)(p1.x - p0.x);
        int y = p0.y * (1. - t) + p1.y * t;
        if (ybuffer[x] < y)
        {
            ybuffer[x] = y;
            image.set(x, 0, color);
        }
    }
}

int main(int argc, char **argv) {

    TGAImage render(width, 100, TGAImage::RGB);
    int ybuffer[width];
    for (int i = 0; i < width; i++) {
        ybuffer[i] = std::numeric_limits<int>::min();
    }
    std::cout<< "ybuffer[0] " << ybuffer[1] << std::endl;

    rasterize(Vec2i(20, 34), Vec2i(744, 400), render, red, ybuffer);
    rasterize(Vec2i(120, 434), Vec2i(444, 400), render, green, ybuffer);
    rasterize(Vec2i(330, 463), Vec2i(594, 200), render, blue, ybuffer);
    render.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    render.write_tga_file("render2.tga");
    return 0;
}