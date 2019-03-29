# 切线空间法线贴图（tangent space normal mapping）

> 前言：我又来翻译了。。。

今天的主题是法线贴图。法线贴图和 Phong 着色之间的主要区别是什么？关键是我们拥有的信息密度。对于 Phong 着色，我们使用三角形网格的每个顶点给出的法向量（并在三角形内插入），而法线贴图纹理提供密集信息，大大改善了渲染细节。



好吧，我们已经在上一课中应用了法线贴图，但是我们使用全局坐标系来存储纹理。今天我们谈论的是切线空间法线贴图。



所以，我们有两个纹理，左边的一个是在全局框架中给出的（从RGB 到 XYZ 法线的直接转换），而在 Darboux 框架中是右边的：



![img](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/06b-tangent-space/nm_textures.jpg)



为了使用正确的纹理，我们绘制每个像素，我们计算切线空间。在此基础上，一个矢量（通常为z）与我们的表面正交，另外两个坐标轴给出了与当前点相切的平面。然后我们从纹理中读取（扰动的）法向量，将其坐标从 Darboux 框架转换为全局系统坐标，我们已经完成了。通常，法线贴图提供法向量的小扰动，因此纹理呈主导蓝色。



好吧，为什么这么乱？为什么不像以前那样使用全球系统？想象一下，我们想要为我们的模型制作动画。例如，我采取了黑人模型并张开嘴。很明显，要修改法向量。

![img](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/06b-tangent-space/global_vs_tangent.jpg)



左图给头部开口，但是没有改变的（全局框架）正常纹理。仔细检查下唇的内部。光线直接照在他的脸上;当嘴巴闭合时，下唇的背面自然不会被点亮的。现在嘴巴张开，但嘴唇没有亮起......正确的图像是用切线空间法线贴图计算的。

因此，如果我们有一个动画模型，那么为了在全局帧中进行正确的法线贴图，我们需要每帧动画有一个纹理，而切线空间相应地变形为模型，而且我们只需要一个纹理！

（这两段话完全没有理解）

这是另一个例子：

![img](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/06b-tangent-space/global_vs_tangent_diablo.jpg)

这些是暗黑破坏神模型的纹理。请注意，纹理中只绘制了一只手，而尾部只有一侧。画家对双臂和两侧使用相同的纹理，这意味着在全局坐标系中我可以为尾部的左侧提供法向矢量。要么是正确的，要么不是两者兼而有之！武器也是如此。我需要左侧和右侧的不同信息，例如，检查左侧图像中的左右颧骨，自然法线向量指向相反的方向！

让我们完成动机部分并直接进行计算。

（看不懂）





## 开始，Phong 着色



好的，这是起点。着色器非常简单，它是 Phong 着色。



```c++
struct Shader : public IShader {
    mat<2,3,float> varying_uv;  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
    mat<3,3,float> varying_nrm; // normal per vertex to be interpolated by FS

    virtual Vec4f vertex(int iface, int nthvert) {
        varying_uv.set_col(nthvert, model->uv(iface, nthvert));
        varying_nrm.set_col(nthvert, proj<3>((Projection*ModelView).invert_transpose()*embed<4>(model->normal(iface, nthvert), 0.f)));
        Vec4f gl_Vertex = Projection*ModelView*embed<4>(model->vert(iface, nthvert));
        varying_tri.set_col(nthvert, gl_Vertex);
        return gl_Vertex;
    }

    virtual bool fragment(Vec3f bar, TGAColor &color) {
        Vec3f bn = (varying_nrm*bar).normalize();
        Vec2f uv = varying_uv*bar;

        float diff = std::max(0.f, bn*light_dir);
        color = model->diffuse(uv)*diff;
        return false;
    }
};
```

这是渲染图像：

![img](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/06b-tangent-space/starting_point_a.jpg)

出于教育和调试目的，我将去除皮肤纹理并应用具有水平红色和垂直蓝线的常规网格

![img](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/06b-tangent-space/starting_point_b.jpg)

让我们记住 Phong 着色的工作原理：

![img](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/06b-tangent-space/grid_texture.png)



对于三角形的每个顶点，我们有它的坐标 p，纹理坐标 uv 和法向量。对于着色当前片段，我们的软件光栅化器为我们提供了片段（alpha，beta，gamma）的重心坐标。这意味着片段的坐标可以获得为p = alpha p0 + beta p1 + gamma p2。然后我们以相同的方式插入纹理坐标和法线向量：

![img](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/06b-tangent-space/f00.png)



请注意，蓝线和红线相应地是 u 和 v 的等值线。因此，对于我们表面的每个点，我们定义了一个所谓的 Darboux 框架，其中 x 和 y 轴平行于蓝色和红色线，z 轴垂直于表面。这是切线空间法线贴图所在的框架。



## 如何从三个样本重建（3D）线性函数



好的，所以我们的目标是为我们绘制的每个像素计算三个向量（切线基础）。让我们把它搁置一段时间，想象一个线性函数 f，对于每个点（x，y，z）给出一个实数 f（x，y，z）= Ax + By + Cz + D。唯一的问题是我们不知道 A，B，C 和 D，但是我们知道在空间的三个不同点（p0，p1，p2）有三个函数值：



![img](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/06b-tangent-space/f01.png)

![img](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/06b-tangent-space/gradient_a.png)



将 f 想象为倾斜平面的高度图是很方便的。我们在平面上修复了三个不同的（非共线）点，我们知道这些点中的 f 的值。三角形内的红线表示等高 f0，f0 + 1 米，f0 + 2 米等。对于线性函数，我们的等值线是平行（直线）线。



事实上，我对方向更感兴趣，正交于等值线。如果我们沿着 iso 移动，高度不会改变（嗯，这是一个iso！）。如果我们偏离 iso 一点点，高度开始变化一点点。当我们正交于等值线时，我们获得最陡的上升。



让我们回想一下，函数最陡峭的上升方向就是它的梯度。对于线性函数f(x，y，z) = Ax+By+Cz+D，其梯度是常数向量（A，B，C）。回想一下，我们不知道（A，B，C）的值。我们只知道该函数的三个样本。我们可以重建 A，B 和 C 吗？当然可以。



所以，我们有三个点 p0，p1，p2 和三个值 f0，f1，f2。我们需要找到最陡上升的矢量（A，B，C）。让我们考虑另一个定义为g（p）= f（p）-f（p0）的函数：

![img](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/06b-tangent-space/gradient_b.png)

显然，我们只是简单地平移了我们的倾斜平面，而没有改变它的倾斜度，因此 f 和 g 的最陡上升方向是相同的。



让我们重写 g 的定义

![img](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/06b-tangent-space/f02.png)

请注意，p ^ x中的上标 x 表示点 p 的 x 坐标而不是幂。因此，函数g 只是向量 (p - p0) 和 (A B C) 之间的点积。我们仍然不知道 (A，B，C)！



好的，让我们回想一下我们所知道的。我们知道如果我们从点 p0 到点 p2，那么函数 g 将从零到 f2-f0 。换句话说，矢量 (p2 - p0) 和(ABC) 之间的点积等于 f2 - f0。 (p1 - p0) 也是如此。因此，我们正在寻找向量 ABC ，与法向量 n 正交并且遵守点积的两个约束。



![img](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/06b-tangent-space/f03.png)



让我们以矩阵形式重写：



![img](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/06b-tangent-space/f04.png)



因此，我们得到了一个易于求解的线性矩阵方程 Ax = b：



![img](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/06b-tangent-space/f05.png)



请注意，我使用字母 A 表示两种不同的东西，其含义应从上下文中清楚。因此，我们的 3x3 矩阵 A 乘以未知向量 x = (A, B, C)，给出向量 b = (f1 - f0, f2 - f0, 0)。当我们将 A 的逆乘以 b 时，未知向量 x 变为已知。

还要注意，矩阵 A 与函数 f 没有任何关系。它只包含有关我们三角形的一些信息。





## 让我们计算 Darboux 基础并应用法线的扰动



因此，Darboux 是向量三元组 (i，j，n)，其中 n - 是原始法向量，i，j 可以如下计算：



![img](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/06b-tangent-space/f06.png)



这是***[提交](https://github.com/ssloy/tinyrenderer/tree/907bb561c38e7bd86db8d99678c0108f2e53d54d)***，使用切线空间中的法线贴图，在这里您可以检查相对于起点（Phong 着色）的差异。



直截了当的来吧，我计算矩阵 A。

```C++
mat<3,3,float> A;
A[0] = ndc_tri.col(1) - ndc_tri.col(0);
A[1] = ndc_tri.col(2) - ndc_tri.col(0);
A[2] = bn;
```



然后计算 Darboux 的两个未知向量 (i, j):



一旦我们得到所有切线基础，我从纹理中读取扰动法线并应用从切线基础到全局坐标的基础变化。回想一下，我已经描述了如何改变基础。



这是最后的渲染图像，与 ***[Phong shading](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/06b-tangent-space/starting_point_a.jpg)*** 比较一下吧！



![img](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/06b-tangent-space/normalmapping.jpg)

