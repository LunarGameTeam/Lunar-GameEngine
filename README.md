# Lunar-GameEngine
Lunar GameEngie是几个渣渣业余写的基于C++的游戏引擎。

# 特点

相比于比较成熟的引擎，该引擎的特点如下

- 结构,标准混乱
- bug众多
- 根本不能用！

最后的最后

**To The Moon and Beyond!**


# 更新日志

## 目前提供的功能

- Vulkan/DirectX12双device
- Render FrameGraph和基础的PBR Pass，ShadowMap
- 多View，多RenderScene
- Python binding 和 doc生成
- ImGui 及 ImGui的Python binding
- LunaCore 以及Library，Module流程框架，支持Python侧扩展模块（EditorModule）
- ECS系统
- 简单的引擎+项目文件系统
- 简单的反射和基于反射的自动序列化，反序列化
- 简单的Python Editor框架（基于IMGUI和Editor）

## 2023.1 TODO
- 新版内存生命周期管理
  
## 2022.12 Updates
- 编辑器功能
- 标准Python + luna.pyd库。支持Pycharm进行编写脚本，使用vcpkg管理三方库
- 支持 Vulkan DirectX12 两个Device


# 项目结构

对于资源文件，可以从引擎目录加载，也可以从项目目录加载，优先读写到项目目录

## 引擎结构
- assets (引擎内置资源/资源)
  - built-in/...  
- sdk (第三方库)
  - ...
- editor (编辑器代码)
- log.txt (日志 ignored)
- build (构建 ignored)
- src (源码)
- bin (build后的二进制和库 ignored)

## 项目结构
- assets
  - ...

## 编译须知

建议使用 vs2022 进行编译

编译需要先下载fbxsdk2020-2-1

https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2020-2-1

cmake配置项中FBX_ROOT_DIR XXX/Autodesk/FBX/FBX SDK/2020.2.1

C++标准：C++ 20
Python版本：3.11
CMake版本：3.24

PyCharm：自选

C++ VS启动项目：app_editor
Pycharm 启动脚本：editor/main.py

## 工作目录

工作目录：Lunar-GameEngine/

## 编码规范

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
  - aaBb 参数
  - mAaBb 成员
  - sAaBb 静态成员  

## Tips
- Luna的Editor和Game执行程序的入口是一个标准的python执行脚本文件流程，所以可以用系统的python + editor/main.py脚本启动，工作目录要设置引擎根目录
- 不要用标准的assert()，使用LUNA_ASSERT，标准assert在Release版本下不执行，LUNA_ASSERT会log报错信息。
- 使用luna的log
- core模块提供了STATIC_INIT静态结构体初始化函数段功能，但是尽量不要用，函数段初始化顺序不一致。
- include头文件请写全相对src的路径，方便重构
- core提供了thread safe map，但是目前还是单线程
- 反射模块主要是为了序列化和binding而写的，别的地方尽量不要用


