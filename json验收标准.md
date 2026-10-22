# JSON 解析器项目验收标准

项目名暂定为 **jsonlite**。可以先完成一种语言版本，再用另一种语言按同一标准重写。

## 1. 项目目标

实现一个无第三方依赖的 JSON 解析器和序列化器，支持 RFC 8259 标准 JSON。

交付物包括：

- 可复用的解析器库
- 命令行工具 `jsonlite`
- 单元测试、集成测试和模糊测试
- README、设计说明和基准测试报告

核心目标不是支持 JSON5、注释或尾逗号，而是正确处理标准 JSON、错误输入和大数据量。

## 2. 功能要求

必须支持以下 JSON 值：

- `null`
- `true` / `false`
- 数字
- 字符串
- 数组
- 对象

具体要求：

1. 顶层可以是任意 JSON 值，不必限定为对象。
2. 正确识别空格、制表符、换行和回车的空白字符。
3. 对象键必须是字符串，值可以是任意 JSON 类型。
4. 解析完成后只允许出现空白字符，后面还有内容必须报错。
5. 默认拒绝乱序、缺失和多余的括号、逗号、引号。
6. 默认拒绝尾逗号和没有引号的对象键。
7. 输入开头最多允许一个 UTF-8 BOM，并正确忽略。
8. 检测不是合法 UTF-8 的字节序列。
9. 支持可配置的最大嵌套深度，避免恶意输入导致栈溢出。
10. 支持最大输入大小限制，防止意外读取超大文件。

## 3. 数字处理要求

数字必须严格符合 JSON 语法：

- 可选负号
- 整数部分不能有前导零，`01` 必须报错
- 小数部分必须至少有一位数字
- 科学计数法的指数必须至少有一位数字
- 不支持 `NaN`、`Infinity`、`+1` 和十六进制数字

建议内部保留数字的原始文本，同时提供整数和浮点解析结果。不能出现：

- 超出 `int64` 却静默变成错误整数
- 大整数变成不准确浮点数却没有提示
- 序列化后改变数字含义

例如：

```json
123456789012345678901234567890
```

可以保留原始字符串，而不是强行转成 `double`。

## 4. 字符串和 Unicode 要求

必须支持：

- 普通 UTF-8 字符
- `\"`
- `\\`
- `\/`
- `\b`
- `\f`
- `\n`
- `\r`
- `\t`
- `\uXXXX`

同时要求：

1. 正确解析 `\uD834\uDD1E` 这样的 UTF-16 代理对。
2. 单独出现的高位或低位代理必须报错。
3. `0x00` 至 `0x1F` 的控制字符不能直接出现在字符串中。
4. 序列化时正确转义引号、反斜杠和控制字符。
5. Unicode 字符可以选择输出原字符或 `\uXXXX`，但行为必须固定并有测试。

## 5. 对象和重复键

RFC 对重复键的处理没有唯一要求，因此项目必须明确策略。

建议默认策略：

```text
遇到重复键时返回 DuplicateKey 错误。
```

也可以提供兼容模式，设置为“后出现的值覆盖前出现的值”。但两种行为都必须有测试，不能随实现悄悄变化。

对象键的输出顺序也必须确定，可以使用排序后的键顺序或插入顺序，但不能依赖哈希表随机顺序。

## 6. API 要求

### C++

建议提供近似以下接口：

```cpp
ParseResult parse(std::string_view json, const ParseOptions& options = {});
std::string serialize(const Value& value);
std::string serialize_pretty(const Value& value, int indent = 2);
```

要求：

- 核心库不使用 `unsafe` 内存操作，不泄漏内存
- `Value` 拥有自己的字符串数据，不保存悬空的 `string_view`
- 不在解析正常错误时直接 `abort`
- 解析错误通过结果类型或异常统一返回
- 不使用裸 `new` / `delete` 管理所有权

### Rust

建议提供：

```rust
pub fn parse(input: &str) -> Result<Value, ParseError>;
pub fn parse_bytes(input: &[u8]) -> Result<Value, ParseError>;
pub fn to_string(value: &Value) -> Result<String, SerializeError>;
pub fn to_string_pretty(value: &Value, indent: usize) -> Result<String, SerializeError>;
```

要求：

- 库代码遇到非法输入时不能 `panic!`
- 核心实现尽量不使用 `unsafe`
- 使用 `Result` 表达解析错误
- 递归解析必须有深度限制

## 7. 错误信息要求

错误至少包含：

- 字节偏移量
- 错误类别
- 可读的错误说明

建议同时提供：

- 行号
- 列号
- 输入附近的一小段上下文

至少区分这些错误：

- `UnexpectedEndOfInput`
- `UnexpectedToken`
- `InvalidEscape`
- `InvalidUnicode`
- `InvalidNumber`
- `InvalidUtf8`
- `DuplicateKey`
- `DepthLimitExceeded`
- `InputSizeExceeded`

例如：

```text
解析失败：第 3 行第 18 列，对象键必须使用双引号
```

## 8. 序列化要求

必须支持：

- 紧凑输出
- 2 或 4 空格缩进的美化输出
- 正确的字符串转义
- 数组和对象递归序列化
- 拒绝 `NaN`、无穷大等非 JSON 数字

需要满足：

```text
parse(serialize(value)) 与 value 语义相等
```

即解析、序列化、再次解析后，数据内容和类型不能发生变化。

## 9. CLI 要求

建议提供两个基本命令：

```text
jsonlite check input.json
jsonlite format input.json --pretty
jsonlite format input.json --compact
```

行为要求：

- 合法 JSON：退出码 `0`
- JSON 语法错误：非零退出码，并向 `stderr` 输出位置和原因
- 文件不存在、无读取权限等系统错误应与 JSON 语法错误区分
- 支持从标准输入读取内容

## 10. 测试要求

必须覆盖：

- 所有合法基础类型
- 空数组和空对象
- 嵌套数组和对象
- 转义字符
- Unicode 代理对
- 合法与非法数字
- 非法 UTF-8
- 重复键策略
- 超深嵌套
- 截断的输入
- 尾部多余字符
- 随机空白字符组合

推荐把 [JSONTestSuite](https://github.com/nst/JSONTestSuite) 作为兼容性测试集。需要注意，其中部分测试属于“行为可自行选择”，项目应在 README 中说明处理方式。

此外至少完成：

- 单元测试
- CLI 集成测试
- 解析与序列化往返测试
- 模糊测试
- 对 10 MB 左右 JSON 文件的性能基准

## 11. 非功能要求

- 解析时间复杂度应保持线性，即 `O(n)`
- 不允许因非法输入崩溃、越界访问或死循环
- C++ 版本应通过 ASan、UBSan
- Rust 版本应通过 `cargo clippy` 和 `cargo test`
- 建立性能基线，记录吞吐量、耗时和峰值内存
- 深度限制和输入大小限制必须有测试
- README 说明数字精度、重复键、BOM 和对象键顺序策略

## 12. 推荐开发顺序

1. 定义 `Value`、`ParseError`、`ParseOptions`
2. 实现字符扫描器和位置跟踪
3. 解析 `null`、布尔值和数字
4. 实现字符串转义和 Unicode
5. 实现数组、对象和递归深度检查
6. 实现紧凑及美化序列化
7. 加入 CLI、测试和错误信息
8. 使用 JSONTestSuite、模糊测试和性能测试收尾

## 13. 最终验收标准

> 能正确处理标准 JSON 的全部主要语法；错误输入能给出准确定位；解析和序列化可以稳定往返；面对超大、超深、非法 UTF-8 和模糊测试输入时不崩溃；C++ 与 Rust 两个版本都附有测试、基准和清晰的设计说明。
