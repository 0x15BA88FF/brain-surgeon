# 🧠 brain-surgeon

**brain-surgeon** is a linter and formatter for the [Brainfuck](https://esolangs.org/wiki/Brainfuck) programming language, written in modern C++.

It includes:

* 🛠 A **command-line linter/formatter** for Brainfuck code.
* 🧩 A **VS Code extension** for Brainfuck syntax highlighting, linting, and formatting.
* 🧪 A simple **Brainfuck interpreter** written in C.

---

## 🧠 Why Brainfuck?

Brainfuck’s simplicity makes it ideal for tooling like linters and formatters — and brain-surgeon brings modern code quality tools to this minimal esolang.
Also there doesn't exist any brainfuck linter and formatter (as far as i could find).

---

## ✨ Features

* **Linting**: Detects unnecessary commands, unmatched brackets, and style inconsistencies.
* **Formatting**: Cleans up and indents Brainfuck code for better readability.
* **VS Code Integration**:
  * Syntax highlighting
  * Real-time linting
  * On-save formatting
* **Simple Brainfuck Interpreter**: Minimal implementation in C for running `.bf` programs.

---

## 🗂 Project Structure

```bash
brain-surgeon/
├── brain-surgeon/        # C++ linter/formatter (CLI tool)
├── vscode-extension/     # VS Code extension (syntax + tooling support)
└── bf-interpreter/       # C interpreter for Brainfuck
```

---

## 🚀 Getting Started

### 🔧 Build and Install CLI Linter/Formatter

```bash
cd brain-surgeon
make clean install
```

This will build the `brain-surgeon` binary and install it to your system (typically `/usr/local/bin/brain-surgeon`).

### 🧪 Build and Install the Interpreter

```bash
cd bf-interpreter
make clean install
```

This builds and installs a lightweight Brainfuck interpreter called `bf-interpreter`.

---

## 🧩 VS Code Extension

To build and install the VS Code extension:

```bash
cd vscode-extension
npm install
npm run package
```

Then in VS Code:

1. Open Command Palette (`Ctrl+Shift+P`)
2. Run: `Extensions: Install from VSIX`
3. Select the generated `.vsix` file

---

## 📦 CLI Usage

### Format or Lint Brainfuck Code

```bash
brain-surgeon [options] <file.bf>
```

**Options:**

* `lint`     — Lint the Brainfuck code
* `fmt`   — Format the code and output to stdout

---

## 🧪 Interpreter Usage

Run a Brainfuck program:

```bash
bf program.bf
```

---

## 📸 Example

**Input:**

```
+++[>+++[>+++<-]<-]>>.
```

**Formatted:**

```
+++
[
    > +++
    [
        > +++
        < -
    ]
    < -
]
>> .
```
