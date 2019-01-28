### 学习目标

- 如何使用gobject语法定义一个类
- 如何实例化定义的类

### 学习总结

上面就是一个最简单类的实现，将类名bird替换成其他名字即可生成一个新类，核心点如下：

- bird.h使用宏和typedef实现类的基本操作
- bird.c使用G_DEFINE_TYPE自动展开成类定义
- main.c使用g_type_init()使新类生效

对应C++类


```
class Bird
{
public:
    Bird();
    Bird(std::string name);
    
    std::string get_bird_name();
private:
    std::string name_;
};
```

