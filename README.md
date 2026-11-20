# OOP-BIGHW-SHYqwq

面向对象程序设计课程大作业（BigHW）源代码仓库。

- 课程：面向对象程序设计
- 环境：Visual Studio 2026 / PlatformToolset v145 / C++20
- 平台：x64、x86；Debug、Release

## 目录结构

```text
BigHW/                  VS2026 解决方案（提交内容）
    BigHW.slnx
    90-01-b1/           上学期作业：汉诺塔
    90-01-b2/           上学期作业：彩球游戏
    90-02-b1/           本学期作业项目
    test-cct/           cmd_console_tools 测试
    common/             公共源文件
    include/            公共头文件
Version-info/           每次作业老师下发的题目 PDF
README.md
.gitignore
```

## 项目一览

| 项目 | 内容 |
|---|---|
| `90-01-b1` | 汉诺塔 |
| `90-01-b2` | 彩球游戏 |
| `90-02-b1` | 本学期作业项目 |
| `test-cct` | `cmd_console_tools` 系列函数测试 |
| `common` / `include` | 公共源文件/头文件，仅查看，不单独生成 |

## 构建

1. 用 VS2026 打开 `BigHW\BigHW.slnx`。
2. 在“解决方案 → 属性”中取消 `common`、`include` 在 Debug/Release、x64/x86 下的“生成”。
3. 重新生成解决方案；不要单独生成 `common` / `include`（它们没有 `main`）。

也可以逐个项目构建：

```powershell
msbuild BigHW\BigHW.slnx /t:90-01-b1 /p:Configuration=Debug /p:Platform=x64
```

## 版本

| 标签 | 内容 | 作业 PDF |
|---|---|---|
| `hw0` | 汉诺塔、彩球游戏、cmd 工具集 | [hw0.pdf](Version-info/hw0.pdf) |

## Version-info

`Version-info/` 保存每次作业老师下发的 PDF，文件名与作业编号一致：

- `hw0.pdf`
- 后续作业按 `hwN.pdf` 追加

该目录只用于存档题目，不参与编译。

## 编码与规范

- 源文件 `*.h` / `*.cpp` 使用 GBK（代码页 936）编码，换行为 CRLF。
- 公共文件只保留一份：头文件在 `include/`，源文件在 `common/`。
- 不使用全局变量、STL 容器、`scanf`/`printf`、`goto`。
- 提交前删除 `.vs`、`x64`、`Debug`、`Release` 等本地产物。
