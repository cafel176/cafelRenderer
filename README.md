# cafelRenderer
一款仿Mitsuba的渲染器

![图](pic/image0.jpg)

本项目功能仿照开源渲染器Mitsuba，场景文件采用xml格式组织，程序读取后，会首先生成opengl场景以供预览，用户可以在场景中移动旋转相机以调整视角，找到合适位置后在菜单中点击开始渲染，主窗口会从中间向上下两边动态的显示渲染结果，最后可以将渲染结果保存为jpg图片格式。

<span id="jump"><b>主要功能：</b></span><br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href="#jump1">1.xml规划场景</a><br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href="#jump2">2.opengl预览</a><br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href="#jump3">3.内置几何体</a><br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href="#jump4">4.内置bsdf</a><br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href="#jump5">5.内置采样器</a><br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href="#jump6">6.蒙特卡洛光路追踪</a><br>

<br/>
<span id="jump1"><b>1.xml规划场景： </b><a href="#jump">返回目录</a></span><br>
与Mitsuba相同，场景组织采用xml文件。<br/>
在此文件中可以定义相机参数，如生成图像的宽高，追踪器采样数，光线最大深度等等。<br/>
此外即是场景中的物体，可以通过此文件定义他们的几何信息，表面材质信息等等。<br/>

![图](pic/image0.jpg)
