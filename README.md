# Lunar-GameEngine

Lunar GameEngie是几个渣渣业余写的基于C++的轻量游戏引擎。

# 特点

相比于比较成熟的 Unity 以及 UE4 引擎，该引擎的特点如下

- 结构混乱
- 标准混乱
- bug众多
- 根本不能用

最后的最后

**To The Moon and Beyond!**

# 项目结构

## 目录结构
- assets (引擎内置资源/资源)
  - internal-assets/...
  - external-assets/...
- sdk (第三方库)
  - boost
  - jsoncpp
  - DirectXTex
  - DirectXTK
  - ...
- log (日志)
- build (构建)
- src (源码)
- cmake-build 2017/2019.bat 生成vs sln文件
- full-build 2017/2019.bat 生成vs sln，全量构建boost(需要拉取所有boost子模块)


(第一次使用full-build构建
后续更改项目结构（新增，删除文件等）使用cmake-build.bat)



## 源码结构

一级VS文件夹结构(归类引擎和SDK)

二级项目结构(归类项目)


- LunarGameEngine - 引擎，独立应用，编辑器源码
  - engine (内含各个模块)
    - assets 资源 [Pancy](https://github.com/pancystar)
    - core 核心 [IsakWong](https://github.com/IsakWong) | [Pancy](https://github.com/pancystar)
      - 文件子系统 [IsakWong](https://github.com/IsakWong)
      - 线程 [Pancy](https://github.com/pancystar)
      - 反射 [Pancy](https://github.com/pancystar) 
      - GC [IsakWong](https://github.com/IsakWong)
      - 对象系统 [IsakWong](https://github.com/IsakWong)
      - 序列化 [Pancy](https://github.com/pancystar)
    - physics 物理 [无]
    - render 渲染 [Pancy](https://github.com/pancystar)
    - window 窗口模块 [IsakWong](https://github.com/IsakWong)
    - world 世界 [无]
  - engine_test (引擎单元测试) [IsakWong](https://github.com/IsakWong)
  - editor
  - editor_test (编辑器单元测试)
  - application
  - application_test (独立应用单元测试)
- SDK - 第三方库代码，使用CMake include

## 运行及工作目录映射

工作目录：Lunar-GameEngine/ 

**可通过asstes/ ./assets log/ ./log访问子文件夹**

后续考虑映射到项目子文件夹，防止误修改源文件

## 编码规范

遵循Goole C++编码规范

[Google C++ 编码规范](https://zh-google-styleguide.readthedocs.io/en/latest/google-cpp-styleguide/naming/)

- 文件名小写 
  - xxx_xxx.cpp 
  - xxx_xxx.h 
  - xxx_xxx.hpp
- 类名驼峰命名
  - class AbcDef
- 方法驼峰命名
  - GetAAA()
  - DoBBB()
- 变量名
  - xx_xx 参数
  - m_xx_xx 成员
  - s_xx_xx 静态成员
  - g_xx_xx 全局
  
## Message规范

- feat(scope): 特性
- fix(scope): 修复bug
- refactor(scope): 重构
- doc(scope): 文档
- style(scope): 格式化，重命名
- revert(scope)：还原
- devops(scope): build脚本

如下:

**feat(core,render):增加反射，渲染管道等**



