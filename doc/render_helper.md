# 简介

Luna Engine目前整理升级了渲染流程，主要变化如下：

- 现在Game Module不能再直接访问任何Render Module的数据。如果是对Render Module有影响的组件(如MeshRenderer，Light，Camera等),需要先继承RenderComponent，
  后面会有教程来讲解如何将逻辑帧的更新推送给Render Module

- 现在主渲染流程会交给SceneRenderer，其功能是把输入的RenderScene，使用给定的SceneRenderPipeline渲染出来。其中RenderScene代表了被渲染的场景。而SceneRenderPipeline
  则是由很多RenderPassGenerator组成的渲染管线，负责决定场景数据会被如何渲染。

- RenderScene用于存储被渲染的场景数据。被设计为Entity-Component结构。RenderScene本身只保存两种数据的列表，即RenderView列表(存储场景里所有的相机)和RenderObject列表(
  存储场景里所有的被渲染对象)。RenderScene，RenderView和RenderObject都被设计为Entity，即本身不存储任何渲染会用到的数据，但是可以创建任意类型的组件数据(即RenderData)挂
  在这些Entity上。例如对于场景中的一个模型，它会在RenderScene里对应创建一个RenderObject，并会在这个RenderObject上面挂一个RenderMeshBase类型的RenderData。

- RenderScene内部任何数据的创建和使用，都只能在RenderModule的RenderTick阶段完成。不允许在其他的模块的Tick阶段直接修改RenderScene内部的数据。RenderModule的RenderTick阶段
  虽然可以访问到RenderScene的数据，但是只允许访问CPU可见的数据。如果需要修改或更新GPU资源，则只能通过创建Compute Shader Command或者Copy Command去更新这些数据。后面会有如何
  更新这些数的教程。

- SceneRenderPipeline实际上就是一个由RenderPassGenerator列表组成的Pass生成器。每个RenderPassGenerator负责将自己模块需要用到的Pass生成并提交给FrameGraph。这里需要注意的是
  制作Pass的代码是不需要考虑执行顺序和资源利用率的，FrameGraph会通过Compile解决这些问题。举例，如果你需要一个需要3次高斯模糊的后处理操作。
  正确的做法是创建四张FGVirtualTexture(A,B,C,D),然后制作A->B->C->D的所有pass。
  而不是只创建两张FGVirtualTexture(A,B),然后制作A->B->A->B的所有pass。这样并不能真正的节省存储空间(因为是虚拟贴图)，反而会造成循环依赖影响FrameGraph的编译工作
# Game Module的组件如何在Tick的流程里为RenderScene创建和更新自己对应的数据
- 待补充
# RenderScene里面的数据在RenderTick的流程中是如何更新的
- 待补充
# Render Module在RenderTick的流程中如何更新RenderScene里GPU的数据
- 待补充
# 如何创建一个RenderPassGenerator用于拓展渲染管线
- 待补充
# 其他
- 待补充