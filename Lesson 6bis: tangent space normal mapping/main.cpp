#include <vector>
#include <iostream>
#include <cmath>
#include <limits>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const int width = 800;
const int height = 800;
const int depth = 255;

Model *model = NULL;
int *zbuffer = NULL;
Vec3f light_dir = Vec3f(1, -1, 1).normalize();
Vec3f eye(1, 1, 3);
Vec3f center(0, 0, 0);


Matrix lookat(Vec3f eye, Vec3f center, Vec3f up) {
    // 计算 z 轴单位向量
    Vec3f z = (eye-center).normalize();
    // 计算 x 轴单位向量 只能是 up ^ z
    Vec3f x = (up ^ z).normalize();
    // 计算 y 轴单位向量 只能是 z ^ y
    Vec3f y = (z ^ x).normalize();
    Matrix ModelView = Matrix::identity(4);
    // 得到 ModelView matrix
    for(int i = 0; i < 3; i++) {
        // 旋转
        ModelView[0][i] = x[i];
        ModelView[1][i] = y[i];
        ModelView[2][i] = z[i];
        // 偏移
        ModelView[i][3] = -center[i];
    }

    return ModelView;
}

Matrix viewport(int x, int y, int w, int h) {
    Matrix m = Matrix::identity(4);
    m[0][3] = x + w/2.f;
    m[1][3] = y + h/2.f;
    m[2][3] = depth/2.f;

    m[0][0] = w / 2.f;
    m[1][1] = h / 2.f;
    m[2][2] = depth / 2.f;
    return m;
}
void triangle(Vec3i t0, Vec3i t1, Vec3i t2, float ity0, float ity1, float ity2, TGAImage &image, int *zbuffer) {
    if(t0.y == t1.y && t0.y == t2.y) return;
    // 按 y 排序
    if (t0.y>t1.y) { std::swap(t0, t1); std::swap(ity0, ity1); }
    if (t0.y>t2.y) { std::swap(t0, t2); std::swap(ity0, ity2); }
    if (t1.y>t2.y) { std::swap(t1, t2); std::swap(ity1, ity2); }

    int total_height = t2.y - t0.y;
    // 画不画最后一个顶点都可以
    // 所有的三角形都是连起来的
    // 只画第一个和中间那个顶点就能保证
    // 所有顶点都画上
    for(int i = 0; i < total_height; i++) {
        // 把三角形分成两部分
        // 中间点以下和中间点以上
        // 递增 y，在两个点中画一条直线
        // 如果 t1.y == t0.y 跳到第二部分
        // 由于 i == 0 所以这条直线不会被跳过
        bool second_half = i > t1.y - t0.y || t1.y == t0.y;
        
        int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;

        float alpha = (float)i / total_height;
        float beta = (float)(i-(second_half ? t1.y-t0.y : 0))/segment_height;

        Vec3i A = t0 + Vec3f(t2-t0)*alpha;
        Vec3i B = second_half ? t1 + Vec3f(t2-t1  )*beta : t0  + Vec3f(t1-t0  )*beta;

        // 线性变化
        float ityA =               ity0 +   (ity2-ity0)*alpha;
        float ityB = second_half ? ity1 +   (ity2-ity1)*beta : ity0 +   (ity1-ity0)*beta;

        if (A.x>B.x) { std::swap(A, B); std::swap(ityA, ityB); }
        for (int j=A.x; j<=B.x; j++) {
            float phi = B.x==A.x ? 1. : (float)(j-A.x)/(B.x-A.x);
            Vec3i    P = Vec3f(A) +  Vec3f(B-A)*phi;
            float ityP =    ityA  + (ityB-ityA)*phi;
            int idx = P.x+P.y*width;
            if (P.x>=width||P.y>=height||P.x<0||P.y<0) continue;
            if (zbuffer[idx]<P.z) {
                zbuffer[idx] = P.z;
                image.set(P.x, P.y, TGAColor(255, 255, 255)*ityP);
            }
        }
    }
}

int main(int argc, char** argv) {
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("../obj/african_head/african_head.obj");
    }

    zbuffer = new int[width * height];
    for(int i = 0; i < width*height; i++) {
        zbuffer[i] = std::numeric_limits<int>::min();
    }

    Matrix ModelView = lookat(eye, center, Vec3f(0, 1, 0));
    Matrix Projection = Matrix::identity(4);
    // 透视
    Projection[3][2] = -1.f/(eye - center).norm();
    // 转换到试图窗口
    Matrix ViewPort = viewport(width/8, height/8, width*3/4, height*3/4);

    TGAImage image(width, height, TGAImage::RGB);

    for(int i = 0; i < model->nfaces(); i++) {
        // 返回三个点的位置索引
        std::vector<int> face = model->face(i);

        Vec3i screen_coords[3];
        float intensity[3];

        for(int j = 0; j < 3; j++) {
            // 得到顶点
            Vec3f v = model->vert(face[j]);
            screen_coords[j] = Vec3f(ViewPort * Projection * ModelView * Matrix(v));
            // 由法线计算顶点的光照强度
            intensity[j] = model->norm(i, j)*light_dir;
        }
        triangle(screen_coords[0], screen_coords[1], screen_coords[2], intensity[0], intensity[1], intensity[2], image, zbuffer);
    }
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");

    // 不用线性变化
    TGAImage zbimage(width, height, TGAImage::GRAYSCALE);
    for (int i=0; i<width; i++) {
        for (int j=0; j<height; j++) {
            zbimage.set(i, j, TGAColor(zbuffer[i+j*width]));
        }
    }
    zbimage.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    zbimage.write_tga_file("zbuffer.tga");

    delete model;
    delete[] zbuffer;
    return 0;
}