#include <vector>
#include <iostream>
#include <cmath>
#include <limits>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const int width  = 800;
const int height = 800;
const int depth  = 255;

Model *model = NULL;
int *zbuffer = NULL;
Vec3f light_dir = Vec3f(1,-1,1).normalize();
Vec3f eye(1,1,3);
Vec3f center(0,0,0);

Matrix viewport(int x, int y, int w, int h) {
    Matrix m = Matrix::identity(4);
    m[0][3] = x+w/2.f;
    m[1][3] = y+h/2.f;
    m[2][3] = depth/2.f;

    m[0][0] = w/3.f;
    m[1][1] = h/3.f;
    m[2][2] = depth/2.f;
    return m;
}
/*

解释一下 lookat 函数的作用

center 是视线的中心，所以向量 (eye-center) 就是 z 轴方向
up 是世界坐标的向上向量通常定义为 (0, 1, 0)

有了 z 和 up 以后通过法向量的计算方式就可以算出 x 轴的单位向量
有了 x 和 z 以后通过法向量的计算方式就可以算出 y 轴的单位向量

接着构造一个将世界坐标系投影到相机坐标系的矩阵

*/
Matrix lookat(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f z = (eye-center).normalize();
    Vec3f x = (up^z).normalize();
    Vec3f y = (z^x).normalize();
    Matrix res = Matrix::identity(4);
    for (int i=0; i<3; i++) {
        res[0][i] = x[i];
        res[1][i] = y[i];
        res[2][i] = z[i];
        res[i][3] = -center[i];
    }
    return res;
}

void triangle(Vec3i t0, Vec3i t1, Vec3i t2, float ity0, float ity1, float ity2, TGAImage &image, int *zbuffer) {
    if (t0.y==t1.y && t0.y==t2.y) return; // i dont care about degenerate triangles
    if (t0.y>t1.y) { std::swap(t0, t1); std::swap(ity0, ity1); }
    if (t0.y>t2.y) { std::swap(t0, t2); std::swap(ity0, ity2); }
    if (t1.y>t2.y) { std::swap(t1, t2); std::swap(ity1, ity2); }

    int total_height = t2.y-t0.y;
    for (int i=0; i<total_height; i++) {
        bool second_half = i>t1.y-t0.y || t1.y==t0.y;
        int segment_height = second_half ? t2.y-t1.y : t1.y-t0.y;
        float alpha = (float)i/total_height;
        float beta  = (float)(i-(second_half ? t1.y-t0.y : 0))/segment_height; // be careful: with above conditions no division by zero here
        Vec3i A    =               t0  + Vec3f(t2-t0  )*alpha;
        Vec3i B    = second_half ? t1  + Vec3f(t2-t1  )*beta : t0  + Vec3f(t1-t0  )*beta;
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

/*
叙述整个过程

1. 模型矩阵。用来计算每个点的变换，但是我们没有什么变换，所以不去计算什么模型矩阵。把点从模型中读取出来就是模型矩阵了
2. 视图矩阵，我们更关心的是，模型相对于眼睛的位置

把原点平移到相机处，然后通过旋转调整位置
*/

int main(int argc, char** argv) {
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("../obj/african_head/african_head.obj");
    }

    zbuffer = new int[width*height];
    for (int i=0; i<width*height; i++) {
        zbuffer[i] = std::numeric_limits<int>::min();
    }

    { // draw the model
        Matrix ModelView  = lookat(eye, center, Vec3f(0,1,0));
        Matrix Projection = Matrix::identity(4);
        Matrix ViewPort   = viewport(width/8, height/8, width*3/4, height*3/4);
        // 只改变
        Projection[3][2] = -1.f/(eye-center).norm();

        std::cerr << ModelView << std::endl;
        std::cerr << Projection << std::endl;
        std::cerr << ViewPort << std::endl; 
        Matrix z = (ViewPort*Projection*ModelView);
        std::cerr << z << std::endl;

        TGAImage image(width, height, TGAImage::RGB);
        for (int i=0; i<model->nfaces(); i++) {
            // i 是三角形的索引
            // face 是三角形位置的索引 所以长度是 3
            std::vector<int> face = model->face(i);  // 返回位置索引
            Vec3i screen_coords[3];
            Vec3f world_coords[3];
            float intensity[3];
            for (int j=0; j<3; j++) {
                Vec3f v = model->vert(face[j]);
                // v 是顶点信息
                screen_coords[j] = Vec3f(ViewPort*Projection*ModelView*Matrix(v));
                world_coords[j]  = v;
                // 保留所有顶点的 强度值，然后在画三角形的时候，使中间的值线性变化
                intensity[j] = model->norm(i, j)*light_dir;
            }
            triangle(screen_coords[0], screen_coords[1], screen_coords[2], intensity[0], intensity[1], intensity[2], image, zbuffer);
        }
        image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
        image.write_tga_file("output.tga");
    }

    { // dump z-buffer (debugging purposes only)
        TGAImage zbimage(width, height, TGAImage::GRAYSCALE);
        for (int i=0; i<width; i++) {
            for (int j=0; j<height; j++) {
                zbimage.set(i, j, TGAColor(zbuffer[i+j*width]));
            }
        }
        zbimage.flip_vertically(); // i want to have the origin at the left bottom corner of the image
        zbimage.write_tga_file("zbuffer.tga");
    }
    delete model;
    delete [] zbuffer;
    return 0;
}
