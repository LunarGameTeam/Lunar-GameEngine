# Lunar-GameEngine
Lunar GameEngie是几个渣渣业余写的基于C++的游戏引擎。

# 特点

相比于比较成熟的引擎，该引擎的特点如下

- 结构,标准混乱
- bug众多
- 根本不能用！

最后的最后

**To The Moon and Beyond!**

# 项目结构

## 更新日志
- 2022.7 更新启动，Install流程，现在是一个标准Python。支持Pycharm进行编写脚本。
- 2022.8 更新子仓库，更新build脚本，移动工具到third-party文件夹

## 目录结构
- assets (引擎内置资源/资源)
  - built-in/...  
- sdk (第三方库)
  - boost
  - jsoncpp
  - DirectXTex
  - DirectXTK
  - ...
- editor (编辑器代码)
- log (日志 ignored)
- build (构建 ignored)
- src (源码)
- lib/bin (build后的二进制和库 ignored)
- release (发布目录，包含完整引擎 ignored)


## 编译须知

建议使用 vs2022 进行编译

编译需要先下载fbxsdk2020-2-1

https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2020-2-1

cmake配置项中FBX_ROOT_DIR XXX/Autodesk/FBX/FBX SDK/2020.2.1


C++标准：C++ 20
Python版本：3.10
CMake版本：3.24
PyCharm：自选

C++ VS启动项目：editor_exe

Pycharm启动脚本：editor/main.py

## 工作目录

工作目录：Lunar-GameEngine/

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



