<div align="center">

  <h1>jlang</h1>

  <h4>A procedural programming language mostly inspired by Go/Rust</h4>
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

#### Integer sizes (supported)

```rust
var a: i8 = 127;
var b: i16 = 32767;
var c: i32 = 42;
var d: i64 = 9223372036854775807;

var e: u8 = 255;
var f: u16 = 65535;
var g: u32 = 100;
var h: u64 = 18446744073709551615;
```

<h6><i>All integer types (i8, i16, i32, i64, u8, u16, u32, u64) are supported. The variable's declared type determines the storage size. Integer literals are parsed as i32 by default.</i></h6>

#### Floating point (supported)

```rust
var pi: f32 = 3.14;
var precise: f64 = 3.141592653589793;
```

<h6><i>Float literals are parsed and generate LLVM double constants. Both f32 and f64 type keywords are supported.</i></h6>

#### Character literals (supported)

```rust
var letter: char = 'A';
var newline: char = '\n';
```

<h6><i>Character literals use single quotes and support escape sequences (`\n`, `\t`, `\r`, `\\`, `\'`, `\0`). Characters are stored as i8 values.</i></h6>

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

#### Non-short-circuit operators: `and` / `or`

jlang also provides `and` and `or` keyword operators that work like `&&` and `||` but **always evaluate both operands**:

```rust
if (a and b) {
    printf("both are true");
}

if (a or b) {
    printf("at least one is true");
}
```

<h6><i>Unlike `&&` and `||`, both sides are always evaluated regardless of the left operand's value.</i></h6>

This is useful when both operands have side effects that must always execute:

```rust
// Both validate() and prepare() are ALWAYS called
if (validate() and prepare()) {
    printf("ready to proceed");
}

// Both logAttempt() and checkPermission() are ALWAYS called
if (logAttempt() or checkPermission()) {
    printf("action recorded or permitted");
}
```

| Operator | Short-circuit | Use case |
|----------|---------------|----------|
| `&&` | Yes | Guard clauses, null checks |
| `\|\|` | Yes | Default values, fallbacks |
| `and` | No | When both sides must execute |
| `or` | No | When both sides must execute |

<details>
<summary><b>LLVM IR comparison</b></summary>

Given this jlang code:

```rust
if (a && b) { ... }
if (a and b) { ... }
```

**Short-circuit `&&`** generates conditional branching:

```llvm
  %a1 = load i1, ptr %a              ; load left operand
  br i1 %a1, label %and.rhs, label %and.merge  ; if false, skip right side

and.rhs:
  %b2 = load i1, ptr %b              ; load right operand (only if left was true)
  br label %and.merge

and.merge:
  %and.result = phi i1 [ false, %entry ], [ %b2, %and.rhs ]  ; merge results
```

<h6><i>The `phi` node selects `false` if we came from entry (left was false), or `%b2` if we evaluated the right side. The right operand is only loaded when the left is true.</i></h6>

**Non-short-circuit `and`** evaluates both operands unconditionally:

```llvm
  %a1 = load i1, ptr %a              ; load left operand
  %b2 = load i1, ptr %b              ; load right operand (always)
  %and.result = and i1 %a1, %b2      ; simple bitwise AND
```

<h6><i>Both operands are always loaded and combined with a single `and` instruction. No branching, no phi nodes - just straight-line code.</i></h6>

The same pattern applies to `||` vs `or`:

**Short-circuit `||`:**
```llvm
  %a1 = load i1, ptr %a
  br i1 %a1, label %or.merge, label %or.rhs  ; if true, skip right side

or.rhs:
  %b2 = load i1, ptr %b              ; only evaluated if left was false
  br label %or.merge

or.merge:
  %or.result = phi i1 [ true, %entry ], [ %b2, %or.rhs ]
```

**Non-short-circuit `or`:**
```llvm
  %a1 = load i1, ptr %a
  %b2 = load i1, ptr %b              ; always evaluated
  %or.result = or i1 %a1, %b2
```

</details>

<h6><i>Most languages (C, C++, Java, Rust, Go) only provide short-circuit operators. jlang gives you both options - use `&&`/`||` when you want to skip evaluation, use `and`/`or` when both sides must run.</i></h6>

> **Note:** Take with a grain of salt, need to double check.

### Memory: manual management

```rust
var p: Person* = alloc<Person>();
// ... use p ...
free(p);
```

<h6><i>jlang uses explicit manual memory management with `alloc<T>()` and `free()`. This gives developers full control over memory and keeps the language simple without requiring a garbage collector or complex ownership system.</i></h6>

> [!IMPORTANT]
> You are responsible for freeing all allocated memory. Forgetting to call `free()` will cause memory leaks.

#### Functions: `fn` keyword with trailing return type

```rust
fn add(a: i32, b: i32) -> i32 {
    return a + b;
}
```

<h6><i>The `fn` keyword is concise and widely recognized. Trailing return types (using `->`) improve readability, especially for longer parameter lists.</i></h6>

### Type Reference

| Type | Description | Size | Range/Values |
|------|-------------|------|--------------|
| `i8` | Signed 8-bit integer | 1 byte | -128 to 127 |
| `i16` | Signed 16-bit integer | 2 bytes | -32,768 to 32,767 |
| `i32` | Signed 32-bit integer | 4 bytes | -2³¹ to 2³¹-1 |
| `i64` | Signed 64-bit integer | 8 bytes | -2⁶³ to 2⁶³-1 |
| `u8` | Unsigned 8-bit integer | 1 byte | 0 to 255 |
| `u16` | Unsigned 16-bit integer | 2 bytes | 0 to 65,535 |
| `u32` | Unsigned 32-bit integer | 4 bytes | 0 to 2³²-1 |
| `u64` | Unsigned 64-bit integer | 8 bytes | 0 to 2⁶⁴-1 |
| `f32` | Single-precision float | 4 bytes | ±3.4×10³⁸ |
| `f64` | Double-precision float | 8 bytes | ±1.8×10³⁰⁸ |
| `bool` | Boolean | 1 bit | `true` or `false` |
| `char` | Character | 1 byte | ASCII 0-255 |
| `char*` | String (char pointer) | 8 bytes | Memory address |
| `void` | No value | - | Functions only |
| `T*` | Pointer to type T | 8 bytes | Memory address |

<h6><i>See `samples/types.j` for a complete working example of all types.</i></h6>

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
