#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

// model c++ 构造函数
// 在头文件中声明过一些变量
Model::Model(const char *filename) : verts_(), faces_(), norms_(), uv_(), diffusemap_() {
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if(in.fail()) {
        std::cout << "打开文件失败" << std::endl;
    }

    std::string line;

    while(!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());

        char trash;

        if(!line.compare(0, 2, "v ")) {
            // 把 v 给 trash
            iss >> trash;
            Vec3f v;

            // v 格式是
            // 0.112553 0.137432 0.569616
            // >> 操作符会略过空格
            Vec3f v;

            for(int i = 0; i < 3; i++) {
                // 根据 v[i] 读取数据
                iss >> v[i];
            }
            // 保存所有的顶点
            verts_.push_back(v);
        } else if (!line.compare(0, 3, "vn ")) {
            // vn 顶点法线
            iss >> trash >> trash;
            Vec3f n;
            // vn  0.001 0.482 -0.876
            for (int i=0;i<3;i++) iss >> n[i];
            norms_.push_back(n);
        } else if (!line.compare(0, 3, "vt ")) {
            // 把 vt 丢掉
            iss >> trash >> trash;
            Vec2f uv;
            // ut 的格式
            // 0.535 0.917 0.000
            for (int i=0;i<2;i++) iss >> uv[i];
            uv_.push_back(uv);
        } else if (!line.compare(0, 2, "f ")) {
            std::vector<Vec3i> f;
            Vec3i tmp;
            // 去掉 f    
            iss >> trash;
            // f 格式 位置 纹理 法线
            // 24/1/24 25/2/25 26/3/26
            while(iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) {
                // f 是从 1 开始不是从 0 开始
                for (int i=0; i<3; i++) tmp[i]--;
                f.push_back(tmp);
            }
            faces_.push_back(f);
        }
        // 加载贴图
        std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << " vt# " << uv_.size() << " vn# " << norms_.size() << std::endl;
        load_texture(filename, "_diffuse.tga", diffusemap_);
    }
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

std::vector<int> Model::face(int idx) {
    std::vector<int> face;
    for (int i=0; i<(int)faces_[idx].size(); i++) face.push_back(faces_[idx][i][0]);
    // 返回位置索引
    return face;
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

void Model::load_texture(std::string filename, const char *suffix, TGAImage &img) {
    std::string texfile(filename);
    size_t dot = texfile.find_last_of(".");
    if (dot!=std::string::npos) {
        texfile = texfile.substr(0,dot) + std::string(suffix);
        std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
        img.flip_vertically();
    }
}


TGAColor Model::diffuse(Vec2i uv) {
    return diffusemap_.get(uv.x, uv.y);
}

Vec2i Model::uv(int iface, int nvert) {
    int idx = faces_[iface][nvert][1];
    // idx 是顶点的索引
    // 
    return Vec2i(uv_[idx].x*diffusemap_.get_width(), uv_[idx].y*diffusemap_.get_height());
}

Vec3f Model::norm(int iface, int nvert) {
    int idx = faces_[iface][nvert][2];
    return norms_[idx].normalize();
}
