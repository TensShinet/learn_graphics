# Moving Camera 移动相机



## 3D 空间的基本变化

在欧几里得坐标系中，坐标可以被「原点」和「基坐标」表示。$(x, y, z)$ 在坐标系 $(O, i, j, k)$ 意味着什么呢？意味着，向量 OP 可以按以下的方法表示



$$\overrightarrow{OP}=\overrightarrow{i}x+\overrightarrow{j}y+\overrightarrow{k}z=\begin{bmatrix}
\overrightarrow{i} &\overrightarrow{j}  & \overrightarrow{k} 
\end{bmatrix}\begin{bmatrix}
x\\ 
y\\ 
z
\end{bmatrix}​$$

现在有另一个坐标系了$(O', i', j', k')​$ 我们如何转换坐标呢？新的坐标系可以由旧坐标系表示

$\begin{bmatrix}
\overrightarrow{i}' &\overrightarrow{j}'  & \overrightarrow{k}' 
\end{bmatrix} = \begin{bmatrix}
\overrightarrow{i} &\overrightarrow{j}  & \overrightarrow{k} 
\end{bmatrix} × M$

一图解决所有问题

![img](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/05-camera/f66a0139058ab1d1025dbfd8cd401389.png)

所以我们重新表示一下 OP

![img](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/05-camera/f02.png)

用之前的公式代替

![img](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/05-camera/f03.png)



同样给我们了新的公式去转换坐标

![img](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/05-camera/f04.png)



## 让我们创建我们自己的 gluLookAt

我们的玩具渲染器只能让相机在 Z 轴画图像，如果我们移动相机没有问题，我们可以移动整个场景，让我们的相机不能移动



画个图表示一下



![img](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/05-camera/b94dd4a591514fd66a91a6e4cc065644.png)



这个图表示，我们可以在新的坐标系中画图，$(c, x', y', z')$ 但是我们给的模型是坐标系$(O, x, y, z)$。没有问题！我们所需要的就是去计算坐标的转换，这里是 C++ 代码计算必要的 4×4 模型场景矩阵

```c++
void lookat(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f z = (eye-center).normalize();
    Vec3f x = cross(up,z).normalize();
    Vec3f y = cross(z,x).normalize();
    Matrix Minv = Matrix::identity();
    Matrix Tr   = Matrix::identity();
    for (int i=0; i<3; i++) {
        Minv[0][i] = x[i];
        Minv[1][i] = y[i];
        Minv[2][i] = z[i];
        Tr[i][3] = -center[i];
    }
    ModelView = Minv*Tr;
}
```

注意向量 ce 给出了 $z'$ （不要忘记去正则化它， 这在后面会有用）我们如何计算 $x'$ ? 简单通过 $u$ 和 $z'$ 叉乘得到，然后我们计算 $y'$ 使它与计算的 $x'$, $z'$ 正交。（让我提醒你一下，我们问题的设定 **ce** 和 **u** 不是必要正交）。最后一步是将原点转换为中心c，我们的转换矩阵就绪。现在只需在模型框架中获得坐标（x，y，z，1）的任意点，将其乘以矩阵ModelView，我们就可以获得相机框架中的坐标！顺便说一下，ModelView这个名字来自OpenGL术语。



## Viewport 

如果你从一开始就按照这个课程，你应该记住像这样的奇怪的行：



```c++
screen_coords[j] = Vec2i((v.x+1.)*width/2., (v.y+1.)*height/2.);
```



这是什么意思？这意味着我有一个点Vec2f v，它属于  [-1,1] * [ - 1,1]。我想在图像（宽度，高度）尺寸的图像中绘制它。值（v.x 1）在0和2之间变化，（v.x 1）/ 2在0和1之间变化，并且（v.x 1）* width / 2扫描所有图像。因此，我们有效地将双单元正方形映射到图像上。



但是现在我们正在摆脱这些丑陋的构造，我想重写矩阵形式的所有计算。让我们考虑以下 C++ 代码：



```c++
Matrix viewport(int x, int y, int w, int h) {
    Matrix m = Matrix::identity(4);
    m[0][3] = x+w/2.f;
    m[1][3] = y+h/2.f;
    m[2][3] = depth/2.f;

    m[0][0] = w/2.f;
    m[1][1] = h/2.f;
    m[2][2] = depth/2.f;
    return m;
}
```



这个 code 创建了这样一个矩阵

![img](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/05-camera/f08.png)



这意味着原来 [-1,1] * [ - 1,1] * [ - 1,1] 被映射到屏幕立方体 [x，xw] * [y，yh] * [0，d ]。这是立方体，而不是矩形。这是因为z-buffer的深度计算。这里 d 是 z 缓冲区的分辨率。我喜欢它等于255，因为简单的倾斜 z-buffer 的黑白图像用于调试。 在 OpenGL 术语中，该矩阵称为 ViewPort 矩阵。



## Chain of coordinate transformations 坐标一系列的变换



所以，让我们总结一下。我们的模型是在他们自己的坐标系中创建的，它们要被插入以世界坐标表示的场景中。使用矩阵模型进行从一个坐标系到另一个坐标系的转换。然后我们想要在相机框架（眼睛坐标系中）表示它们，转换这个过程称为 View。得到新的坐标以后，使用透视投影。矩阵转换到场景中被叫做 Clip Coordinate。最后，我们画出这个场景，这个矩阵变化的过程 Clip Coordinate 到 Screen Coordinates 被叫做 ViewPort

再一次如果我们从 .obj 文件读取一个点 v，然后画它在屏幕上。经历了以下步骤

`Viewport * Projection * View * Model * v`

## Transformation of normal vectors 法向量的转换



这里有一个广泛知道的事实



+ 如果我们有一个平面和它的法向量，这个平面经过一系列仿射变换，那么法向量同样也会经过相同的仿射变换。等价于原始映射矩阵的逆矩阵的转换。

什么什么什么？！我认识了很多了解这个事实的程序员，但对他们来说仍然是一个黑魔法。事实上，它并不复杂。拿一支铅笔绘制一个 2D 三角形 $(0, 0) , (0, 1), (1, 0)$ 和一个法向量 n，n 等于 $(1, 1)$。然后让我们将所有 y 坐标拉伸 2 倍，保持 x 坐标不变。因此，我们的三角形变为$(0, 0) (0, 2)(1, 0)$。如果我们以相同的方式变换向量 n，它变为 $(1, 2)$ 并且它不再与三角形的变换边正交。

因此，为了消除所有的黑魔法雾，我们需要理解一件简单的事情：我们不需要简单地变换法向量（因为它们可能变得不正常），我们需要计算（新）法向量到变换后的模型。

回到 3D，我们有一个向量$n = (A，B，C)$。我们知道穿过原点并且 n为法线的平面具有方程 $Ax + By + Cz = 0​$。让我们以矩阵形式编写它（我从头开始在齐次坐标中执行）：



![img](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/05-camera/f05.png)



回想一下$(A，B，C)$ -是一个向量，所以我们在嵌入 4D 时用0增加它，而（x，y，z）用1增加，因为它是一个点。

让我们在其间插入一个单位矩阵

![img](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/05-camera/f06.png)

可以写成



![img](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/05-camera/f06.png)

![img](https://raw.githubusercontent.com/ssloy/tinyrenderer/gh-pages/img/05-camera/f06.png)











