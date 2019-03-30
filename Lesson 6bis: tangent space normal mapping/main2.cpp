
#include <vector>
#include <iostream>

#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "our_gl.h"

Model *model     = NULL;
const int width  = 800;
const int height = 800;

Vec3f light_dir(1, 1, 3);
Vec3f       eye(1, 1, 3);
Vec3f    center(0, 0, 0);
Vec3f        up(0, 1, 0);
struct Shader : public IShader {
    // template<size_t DimRows,size_t DimCols,typename T> class mat
    // vec<DimCols,T> rows[DimRows];
    // 定义了 vec3f rows[2] varying_uv
    mat<2,3,float> varying_uv;
    mat<4,4,float> uniform_M;   //  Projection*ModelView
    mat<4,4,float> uniform_MIT; // (Projection*ModelView).invert_transpose()

    virtual Vec4f vertex(int iface, int nthvert) {
        // void set_col(size_t idx, vec<DimRows,T> v) {
        //     assert(idx<DimCols);
        //     for (size_t i=DimRows; i--; rows[i][idx]=v[i]);
        // }
        // rows[1][nthvert] = v[1]
        // rows[0][nthvert] = v[0]
        // std::cout << varying_uv << '\n';
        varying_uv.set_col(nthvert, model->uv(iface, nthvert));
        Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert)); // read the vertex from .obj file
        return Viewport*Projection*ModelView*gl_Vertex; // transform it to screen coordinates
    }
    
    virtual bool fragment(Vec3f bar, TGAColor &color) {
        /*
        varying_uv  2, 3, float
        两行三列
        */
        // std::cout << "fuck" << '\n';
        Vec2f uv = varying_uv*bar;
        // std::cout << uv << '\n';
        Vec3f n = proj<3>(uniform_MIT*embed<4>(model->normal(uv))).normalize();
        Vec3f l = proj<3>(uniform_M  *embed<4>(light_dir        )).normalize();
        Vec3f r = (n*(n*l*2.f) - l).normalize();   // reflected light
        float spec = pow(std::max(r.z, 0.0f), model->specular(uv));
        float diff = std::max(0.f, n*l);
        TGAColor c = model->diffuse(uv);
        color = c;
        for (int i=0; i<3; i++) color[i] = std::min<float>(5 + c[i]*(diff + .6*spec), 255);
        return false;
    }
};

int main(int argc, char** argv) {


    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("../obj//african_head.obj");
    }

    lookat(eye, center, up);
    viewport(width/8, height/8, width*3/4, height*3/4);
    projection(-1.f/(eye-center).norm());
    light_dir.normalize();

    TGAImage image  (width, height, TGAImage::RGB);
    TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);

    Shader shader;
    // 点的转换
    shader.uniform_M = Projection * ModelView;
    // 法线的转换
    shader.uniform_MIT = (Projection * ModelView).invert_transpose();
    
    for (int i = 0; i < model->nfaces(); i++)
    {
        Vec4f screen_coords[3];
        for (int j = 0; j < 3; j++) {
            // i 是第几个脸
            // j 是第几个顶点
            // 顶点着色器
            screen_coords[j] = shader.vertex(i, j);
        }
        std::cout << "fuck1 " << screen_coords[0]  << '\n';
        std::cout << "fuck2 " << screen_coords[1] << '\n';
        std::cout << "fuck3 " << screen_coords[2] << '\n';

        triangle(screen_coords, shader, image, zbuffer);
    }

    image.  flip_vertically(); // to place the origin in the bottom left corner of the image
    zbuffer.flip_vertically();
    image.  write_tga_file("output_0100.tga");
    zbuffer.write_tga_file("zbuffer.tga");

    delete model;
    return 0;
}