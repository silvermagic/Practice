### 学习目标

- 如何使用闭包
- 如何使用glib的信号机制

### 学习总结

上面就是一个带信号量的类的实现，描述了一个麻雀飞呀飞，被老鹰抓住的故事，核心点如下：

- 在class结构中声明信号处理函数，使用glib-genmarshal生成通用的信号函数实现，marshl.txt中保存的信号函数参数列表是g_signal_emit时候使用的参数列表。信号函数最后的gpointer参数不需要写入marshl.txt中，这个参数在g_signal_connect的时候传入，信号函数最后的gpointer参数可以去掉，对应的g_signal_connect最后的参数设置为NULL即可
- CMakeLists.txt使用glib-genmarshal命令生成通用信号函数的.h和.c文件
- 在.c文件中定义一个guint信号数组
- 在class_init函数创建一个信号
- 在成员函数或者public api使用g_signal_emit触发信号

