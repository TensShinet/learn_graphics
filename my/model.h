#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class Model
{
  private:
    // 保存所有顶点，每个元素是 Vec3f
    std::vector<Vec3f> verts_;
    // 保存所有的三角形，每个元素是一个三角形
    // 三角形有三个顶点 所以每个元素的元素 是一个顶点
    // 每个顶点有三个描述 位置 v 纹理 vt 法线 vn 的索引
    std::vector<std::vector<Vec3i>> faces_; // attention, this Vec3i means vertex/uv/normal
    // 保存所有法线，每个元素是 Vec3f
    std::vector<Vec3f> norms_;
    // 保存所有贴图位置信息， 每个元素是 Vec2f
    std::vector<Vec2f> uv_;
    // 贴图图片
    TGAImage diffusemap_;
    // 加载贴图
    void load_texture(std::string filename, const char *suffix, TGAImage &img);

  public:
    // 构造函数
    Model(const char *filename);
    ~Model();
    // 顶点数目
    int nverts();
    // 三角形数目
    int nfaces();
    // 顶点法向量正则化
    Vec3f norm(int iface, int nvert);
    // 返回顶点
    Vec3f vert(int i);
    // 贴图信息
    Vec2i uv(int iface, int nvert);
    // 
    TGAColor diffuse(Vec2i uv);
    // 返回位置索引
    std::vector<int> face(int idx);
};

#endif //__MODEL_H__
