### 学习目标

- 格式化输出

### 学习总结

- 按进制显示，十六进制设置`std::hex`，八进制设置`std::oct`
- 宽度控制`setw`
- 字符填充`setfill`
- 位置控制，指定宽度后，输出位于宽度中部设置`std::internal`，位于宽度右边设置`std::right`，位于宽度左边设置`std::left`
- 浮点数精度控制`std::setprecision`
- 进制前缀显示，显示`0x`设置`std::showbase`，不显示`0x`设置`std::noshowbase`
- 布尔值显示，显示字符串`true`设置`std::boolalpha`，显示值`1`设置`std::boolalpha`
- 正符号'+'显示，显示设置`std::showpos`，不显示设置`std::noshowpos`
- 标志设置会对后续所有std::cout生效，如果希望标志不在生效，需要清除标志