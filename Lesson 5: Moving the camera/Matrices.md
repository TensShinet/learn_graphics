# Matrices 矩阵

> 前言：又看到大神写的文章了，为了静下心来读文章，我把大神的文章翻译了一遍。翻三分之一就找到了比较好的***[中文翻译](http://www.opengl-tutorial.org/cn/beginners-tutorials/tutorial-3-matrices/)***。



## Homogeneous coordinates 齐次坐标

在那之前，我们只将 3D 顶点视为（x，y，z）三元组。我们来介绍一下 w。我们现在将有（x，y，z，w）向量。



只需要记住

+ 如果 w = 1， (x, y, z, 1) 是空间里面的点
+ 如果 w = 0，(x, y, z, 0) 是一个方向



这有什么不同？好吧，对于旋转，它不会改变任何东西。旋转点或方向时，会得到相同的结果。但是，对于变换（当你在某个方向上移动点）时，情况就不同了。



齐次坐标允许我们使用单个数学公式来处理这两种情况



## Transformation matrices 矩阵变换



### 介绍一下矩阵



简而言之，矩阵是一个数字数组，具有预定义的行数和列数。列如，2×3 矩阵可能如下所示



![img](http://www.opengl-tutorial.org/assets/images/tuto-3-matrix/2X3.png)



在 3D 图形学中我们最常用的是 4×4 矩阵。4×4 矩阵方便我们转换 (x, y, z, w) 。这是通过定点与矩阵相乘得来的。



一定要是这个顺序 **Matrix × Vertex **

![img](http://www.opengl-tutorial.org/assets/images/tuto-3-matrix/MatrixXVect.gif)



### 平移矩阵（Translation matrices）

平移矩阵是最简单的矩阵变换，一个平移矩阵像这样

![img](http://www.opengl-tutorial.org/assets/images/tuto-3-matrix/translationMatrix.png)

在里面 X, Y, Z 是点的位移增量。

例如，若想把向量(10, 10, 10, 1)沿X轴方向平移10个单位，可得：



![img](http://www.opengl-tutorial.org/assets/images/tuto-3-matrix/translationExamplePosition1.png)



（算算看，一定得亲手算算看）



这样就得到了齐次向量 (20,10,10,1)！记住，末尾的 1 表示这是一个点，而不是方向。经过变换计算后，点仍然是点，这倒是挺合情合理的。

下面来看对一个方向做上述平移会发生什么



![img](http://www.opengl-tutorial.org/assets/images/tuto-3-matrix/translationExampleDirection1.png)



得到与原来一模一样的方向，移动方向并没有意义



### 单位矩阵（The Identity matrix）



单位矩阵很特殊，它什么也不做。单位矩阵的身份和自然数”1”一样基础而重要，因此在这里要特别提及一下。



![img](http://www.opengl-tutorial.org/assets/images/tuto-3-matrix/identityExample.png)



### 缩放矩阵（Scaling matrices）

例如把一个向量（点或方向皆可）沿各方向放大2倍：



![img](http://www.opengl-tutorial.org/assets/images/tuto-3-matrix/scalingExample.png)



w 还是没变。你也许会问：”缩放一个向量”有什么用？嗯，大多数情况下是没什么用，所以一般不会去缩放向量；但在某些特殊情况下它就派上用场了。（顺便说一下，单位矩阵只是缩放矩阵的一个特例，其 (X, Y, Z) = (1, 1, 1)。单位矩阵同时也是旋转矩阵的一个特例，其 (X, Y, Z)=(0, 0, 0)）。



### 旋转矩阵（Rotation matrices）



暂时跳过



### 累积变换（Cumulating transformations）

前面已经学习了如何旋转、平移和缩放向量。把这些矩阵相乘就能将它们组合起来，例如：

`TransformedVector = TranslationMatrix * RotationMatrix * ScaleMatrix * OriginalVector;`

！！！注意！！！这行代码**首先**执行缩放，**接着**旋转，**最后**才是平移。这就是矩阵乘法的工作方式。



变换的顺序不同，得出的结果也不同。



## 模型（Model）、观察（View）和投影（Projection）矩阵

*在接下来的课程中，我们假定您已知如何绘制Blender经典模型小猴Suzanne*



利用模型、观察和投影矩阵，可以将变换过程清晰地分解为三个阶段。虽然此法并非必需（前两课我们就没用这个方法嘛），但采用此法较为稳妥。我们将看到，这种公认的方法对变换流程作了清晰的划分。



### 模型矩阵（The Model matrix）



这个模型正如我们深爱的红色三角形，由一系列顶点定义。这些顶点的 X，Y，Z 坐标与模型的中心相关。也就是说，如果这个顶点在 (0, 0, 0) ，那么这个点就是模型的中心



 ![img](http://www.opengl-tutorial.org/assets/images/tuto-3-matrix/model.png)



我们希望能够移动它，玩家也需要用键鼠控制这个模型。这很简单，只需记住：**缩放、旋转、平移**这个模型就够了。。在每一帧中，用算出的这个矩阵去乘所有的顶点，物体就会移动。唯一不动的是世界空间（World Space）的中心。



![img](http://www.opengl-tutorial.org/assets/images/tuto-3-matrix/world.png)                                                                                        



我们所有的顶点都是位于*世界空间*。下面黑色箭头的意思是：*从模型空间（Model Space）（顶点都相对于模型的中心定义）变换到世界空间（顶点都相对于世界空间中心定义）。*

![img](http://www.opengl-tutorial.org/assets/images/tuto-3-matrix/model_to_world.png)



下图概括了这一过程：

![img](http://www.opengl-tutorial.org/assets/images/tuto-3-matrix/M.png)

### 观察矩阵（The View matrix）

这里再引用一下《飞出个未来》：

> 引擎推动的不是飞船而是宇宙。飞船压根就没动过。

![img](http://www.opengl-tutorial.org/assets/images/tuto-3-matrix/camera.png)



仔细想想，摄像机的原理也是相通的。如果想换个角度观察一座山，您可以移动摄像机也可以……移动山。后者在实际中不可行，在计算机图形学中却十分方便。



下图展示了：*从世界空间（顶点都相对于世界空间中心定义）到摄像机空间（Camera Space，顶点都相对于摄像机定义）的变换。*

![img](http://www.opengl-tutorial.org/assets/images/tuto-3-matrix/model_to_world_to_camera.png)



下图解释了上述变换过程：

![img](http://www.opengl-tutorial.org/assets/images/tuto-3-matrix/MV.png)



好戏还在后头呢！

### 投影矩阵（The Projection matrix）

现在，我们处于摄像机空间中。这意味着，经历了这么多变换后，现在一个坐标X==0且Y==0的顶点，应该被画在屏幕的中心。但仅有x、y坐标还不足以确定物体是否应该画在屏幕上：它到摄像机的距离（z）也很重要！两个x、y坐标相同的顶点，z值较大的一个将会最终显示在屏幕上。

这就是所谓的透视投影（perspective projection）：

![img](http://www.opengl-tutorial.org/assets/images/tuto-3-matrix/model_to_world_to_camera_to_homogeneous.png)

’

最后一个变换：

*从摄像机空间（顶点都相对于摄像机定义）到齐次坐空间（Homogeneous Space）（顶点都在一个小立方体中定义。立方体内的物体都会在屏幕上显示）的变换。*



最后一幅图示：



![img](http://www.opengl-tutorial.org/assets/images/tuto-3-matrix/MVP.png)



再添几张图，以便大家更好地理解投影变换。投影前，蓝色物体都位于摄像机空间中，红色的东西是摄像机的平截头体（frustum）：这是摄像机实际能看见的区域。



![img](http://www.opengl-tutorial.org/assets/images/tuto-3-matrix/nondeforme.png)



用投影矩阵去乘前面的结果，得到如下效果：



![img](http://www.opengl-tutorial.org/assets/images/tuto-3-matrix/homogeneous.png)



## 原文

+ ***[Tutorial 3 : Matrices](http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/)***
+ ***[中文翻译原文](http://www.opengl-tutorial.org/cn/beginners-tutorials/tutorial-3-matrices/)***