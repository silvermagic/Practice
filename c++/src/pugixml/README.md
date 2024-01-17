### 环境搭建

软件安装

```
# yum install -y pugixml-devel
```

### 学习目标

- 从字符串中加载
- 读取/设置节点的属性值
- 查询节点中的特定属性
- 添加/删除节点的属性
- 格式化输出XML字符串

### 学习总结

- `CMakeLists.txt`中如果直接使用`PUGIXML_LIBRARIES`会导致链接失败，查询`build/CMakeFiles/app.dir/link.txt`文件，发现并没有链接`-lpugixml`