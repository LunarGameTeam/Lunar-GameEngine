# Lunar-GameEngine Asset System

lunar引擎的资源系统设计指标

## Overview

资源系统设计参考

Unity： AssetBundle 以及 Meta 文件，

UE4：UPackage UAseet，SoftReference HardReference，Streamable.SyncLoad

### 功能

- 加载与卸载（同步与异步）
- 缓存Cache
- Asset依赖分析（按照依赖顺序依次加载）
- Meta元数据文件

### 扩展功能
- LazyLoad（非异步）延迟资源加载（画大饼）
- 类似于Streamable的资源网络流加载（画大饼）
- Package，Bundle（画大饼）


一个引擎资源如下

assets/xxxxx.meta
assets/xxxxx.png

其中meta是资源元数据描述文件，包含GUID，资源依赖等属性（非资源本身数据，应当存放资源名字，GUID，如压缩选项，贴图大小，脚本类型，材质对应贴图等）

区分Raw数据和资源描述文件，Raw Data是比如xxx.png xxx.jpg，xxx.fbx这样的数据资源

其对应存在xxx.meta的元数据描述文件

复合资源可以没有Raw Data，比如一个复合的prefab，cubemap可能没有Raw Data，他表示了资源的复合


资源系统应可以为原生RawData创建默认Meta文件（在没有编辑器的情况下可以保证每个RawData对应有元数据资源描述文件）


## 接口

- LoadAssetSync
- LoadAssetAsync
- UnloadAsset
- GetAssetDependency

## 
