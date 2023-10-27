# 简介

Luna Engine目前fork了公版的vcpkg并对一部分功能进行了改进，主要目的是为了：

- 快速修复公版vcpkg的bug，以及快速修复一些因为某些公版第三方库的cmake写的不全导致的问题

- 方便添加私包

# vcpkg扩展一个包需要加哪些东西

- vcpkg所有管理的包的配置信息，都是放在ports文件夹下，例如库ms-gltf的配置文件，就是放在ports/ms-gltf里
- 每个包的配置文件主要有四类：
  - .patch文件，如果你希望对原本的库做一些修改，则需要将修改差异导出为patch文件放在这里，如果原始库不需要改则这里可以忽略。

  - .cmakelist文件，如果原始库里没有cmakelist，你可以在这里补上一个，如果原始库已经有了，则不需要在这里再加一个。

  - vcpkg.json文件，这里主要填写你的库是否依赖其他的库(例如ms-gltf编译依赖rapidjson)，以及有哪些可选的fearture(例如imgui可以添加一个feature选择用master分支还是使用docking分支)

  - portfile.cmake，这个是每个包最重要的文件，里面用cmake描述了每个库具体是怎么编译的，大部分的库其实是差不多的，修改bug的时候参考下其他库就可以了
    
    - vcpkg_from_github会需要填写一下你的包的github地址，以及分支提交的md5，和SHA512，前面两个是比较容易填的（分支提交的md5在github上面查看提交记录就可以找到），最后一个每次可以不填，然后生成包的时候报错会告诉你正确的   SHA512是多少，再改一下就行。

# vcpkg对第三方库的代码有哪些要求

- vcpkg是依赖cmake的install的，所以任何第三方库如果希望借助vcpkg进行管理，一定要制作了完整的cmake的install信息

- 目前大部分公版的库，install信息都是比较全的，有些缺少cmake或者install信息的库官方也通过patch进行了补充。不过有些库因为缺失的功能太多，或者写了一些不完全的install，导致仍然有vcpkg打完的包不能用的情况发生。

# 完整的install信息一般包括哪些

- 一般来说，如果一个库包含了完整的install信息，应该会在cmake生成的时候，生成包括：
    - xxxConfig.cmake
    - xxxConfigVersion.cmake
    - xxxTargetscmake(一般会被xxxConfig.cmake引用)
    - xxxTargets-Debug.cmake/xxxTargets-Release.cmake
- 当然，在测试过程中，我们发现有些文件缺失也是可以正常使用的，不过大部分情况下还是需要完整的cmake文件才能被vcpkg正确管理

# vcpkg常见的错误有哪些

- 包拉下来之后，find_package提示缺少xxxconfig.cmake(例如gltf)，这种就是没有导出任何install信息，分两种情况，一种是完全没有写install相关的东西，一种是写了但是没有保存相应的文件，这种情况的修复可以参考lunaEngine对ms-gltf的修复方案。

- 包拉下来之后，find_package是正常的，但是在其他的cmakelist里引用这个包的时候，会发现没有这个包。这种情况比较常见，因为install信息里面填的输出的包的名称是可以和vcpkg的文件夹不一致的，而且同一个第三方库甚至可以install多个包，这个在vcpkg install安装包的时候会提示真正的可引用的包的名字的。

- 包拉下来之后，find_package是正常的，也不会报找不到这个包，但是在工程里面引用时，发现引用的.lib文件的地址不对(例如directx12-memory-allocator)。这种情况是因为虽然第三方库有install信息，但是实际上只有一部分，例如只有target或者干脆用target代替config，还有就是install里的路径没有用vcpkg指定的一些宏。这种情况的修复可以参考unaEngine对directx12-memory-allocator的修复方案。

# 其他
- 目前看起来，vcpkg的bug大部分都可以靠修复portfile.cmake和为第三方库补全install信息来解决。之后有时间可以考虑把引擎里面的功能也独立出来通过魔改的vcpkg管理