# File Subsystem设计

lunar引擎的文件系统设计指标

## Overview

主要目的：跨平台的文件操作


### 功能

- 同步异步I/O（同步与异步）
- FileLoadAsyncHandle
- 跨平台文件操作（删除，移动）

### 扩展功能
- Streamable
- 虚拟文件系统（Package）

## 接口

对外提供IPlatformManager

针对不同系统拥有不同实现

WindowsPlatformManager已经实现


### IPlatformManager接口

- ReadFileSync
- ReadFileAsync
- WriteFileSync
- WriteFileAsync
- IsFileExists
- CreateDirectory
- CreateFile
- DeleteFile
