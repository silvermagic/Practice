### 学习目标

- 解析命令行参数

### 学习总结

- 长参数: 使用`--`
- 短参数: 使用`-`
- `struct option`的最后一个参数表示匹配到长参数时`getopt_long`的返回值
- required_argument: 表示长参数带参数
- no_argument: 表示长参数不带参数
- optional_argument: 表示长参数可带可不带参数
- 短参数可使用getopt函数