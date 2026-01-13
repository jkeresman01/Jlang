<div align="center">

  <h1>jlang</h1>

  <h4>A procedural programming language mostly inspired by C and Go</h4>
  <h6><i>Clean syntax, explicit memory management, and modern language features compiled to native code via LLVM.</i></h6>

[![C++](https://img.shields.io/badge/C++-blue.svg?style=for-the-badge&logo=cplusplus)](https://isocpp.org/)
[![LLVM](https://img.shields.io/badge/LLVM-262D3A.svg?style=for-the-badge&logo=llvm)](https://llvm.org/)

</div>

## About

> [!NOTE]
> This project is still very much a work in progress. Syntax and features may change.

## Language Syntax

```rust
interface IPrintable {
    fn print();
}

struct Person : IPrintable {
    firstName: char*;
    age: i32;
}

fn print(self: Person*) {
    printf("First name: %s", self.firstName);
    printf("Age: %d", self.age);
}

fn main() -> i32 {
    var p: Person* = alloc<Person>();

    if (p == null) {
        printf("No can do");
    } else {
        printf("Incredible");
    }

    free(p);
    return 0;
}
```

### Syntax Design Decisions

<h6><i>jlang's syntax is designed to be clean, explicit, and familiar to developers coming from C, Go, or Rust.</i></h6>

#### Functions: `fn` keyword with trailing return type

```rust
fn add(a: i32, b: i32) -> i32 {
    return a + b;
}
```

<h6><i>The `fn` keyword is concise and widely recognized. Trailing return types (using `->`) improve readability, especially for longer parameter lists.</i></h6>

#### Variables: `var` with colon-separated types

```rust
// Integer (supported)
var count: i32 = 42;

// String (supported)
var name: char* = "jlang";

// Boolean (supported)
var isReady: bool = true;
var isEmpty: bool = false;
```

<h6><i>The colon syntax (`name: Type`) clearly separates identifiers from types and is consistent with modern languages like TypeScript, Kotlin, and Rust.</i></h6>

#### Planned: Integer sizes

```rust
var a: i8 = 127;
var b: i16 = 32767;
var c: i64 = 9223372036854775807;

var d: u8 = 255;
var e: u16 = 65535;
var f: u32 = 100;
var g: u64 = 18446744073709551615;
```

> [!NOTE]
> Type keywords are recognized but literals are always treated as `i32`. Needs: update code generator to infer literal type from variable declaration.

#### Planned: Floating point

```rust
var pi: f32 = 3.14;
var precise: f64 = 3.141592653589793;
```

> [!NOTE]
> Type keywords are recognized but float literals are not parsed. Needs: add decimal point handling in lexer and float literal code generation.

#### Planned: Character literals

```rust
var letter: char = 'A';
```

> [!NOTE]
> The `char` type is recognized but character literals are not parsed. Needs: add single-quote character literal handling in lexer.

#### Control Flow: C-style if/else and while

```rust
if (x == 0) {
    printf("x is zero");
} else if (x == 10) {
    printf("x is ten");
} else {
    printf("x is something else");
}
```

<h6><i>Control flow uses standard C-style syntax with parentheses around conditions. The `else if` chain is achieved by following `else` with another `if` statement - there is no special `elseif` or `elif` keyword. Braces are optional for single statements but recommended for clarity.</i></h6>

#### While loops

```rust
var i: i32 = 0;
while (i < 10) {
    printf("i = %d", i);
    i = i + 1;
}
```

<h6><i>The `while` loop uses C-style syntax: `while (condition) { body }`. The condition is evaluated before each iteration, and the loop continues as long as the condition is true. Braces are optional for single statements.</i></h6>



#### Structs: colon for interface implementation

```rust
struct Person : IPrintable {
    firstName: char*;
    age: i32;
}
```

<h6><i>Using `:` for interface implementation follows C++/C# conventions.</i></h6>

#### Methods: explicit `self` parameter

```rust
fn print(self: Person*) {
    printf("Name: %s", self.firstName);
}
```

<h6><i>Explicit `self` makes the receiver clear and visible. There's no hidden magic - you can see exactly what the method operates on. This approach is similar to Python and Rust.</i></h6>

#### Semicolons: required

<h6><i>All statements must end with a semicolon. This makes parsing unambiguous and aligns with C-family languages.</i></h6>

#### Null: lowercase `null`

```rust
if (p == null) { ... }
```

<h6><i>Using lowercase `null` is consistent with most modern languages (Java, C#, JavaScript) and feels more natural than the C macro `NULL`.</i></h6>

> [!TIP]
> Always check if `alloc<T>()` returns `null` before using the pointer.

#### Logical operators

```rust
if (a && b) {
    printf("both are true");
}

if (a || b) {
    printf("at least one is true");
}

if (!a) {
    printf("a is false");
}
```

<h6><i>Logical AND (`&&`), OR (`||`), and NOT (`!`) work as expected from C-family languages.</i></h6>

#### Short-circuit evaluation

Logical operators use short-circuit evaluation - the right operand is only evaluated if necessary:

```rust
// If isValid is false, expensiveCheck() is never called
if (isValid && expensiveCheck()) {
    printf("both conditions passed");
}

// If hasCache is true, loadFromDisk() is never called
if (hasCache || loadFromDisk()) {
    printf("data is available");
}
```

<h6><i>This is useful for guarding against null pointer access or avoiding expensive operations:</i></h6>

```rust
// Safe: if p is null, we never access p.value
if (p != null && p.value > 0) {
    printf("positive value");
}
```

### Memory: manual management

```rust
var p: Person* = alloc<Person>();
// ... use p ...
free(p);
```

<h6><i>jlang uses explicit manual memory management with `alloc<T>()` and `free()`. This gives developers full control over memory and keeps the language simple without requiring a garbage collector or complex ownership system.</i></h6>

> [!IMPORTANT]
> You are responsible for freeing all allocated memory. Forgetting to call `free()` will cause memory leaks.

## Getting started

### Prerequisites

> [!IMPORTANT]
> You must have LLVM installed on your system to build Jlang.

**Ubuntu/Debian:**
```bash
sudo apt install llvm-dev
```

**Fedora:**
```bash
sudo dnf install llvm-devel
```

**macOS:**
```bash
brew install llvm
```

### Build

```bash
mkdir -p build && cd build
cmake ..
make
```

> [!TIP]
> For a quicker one-liner from the project root:
> ```bash
> cmake -B build && cmake --build build
> ```

### Run

```bash
./build/Jlang samples/sample.j
```
