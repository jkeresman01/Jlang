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
    Name: char*;    // Public - uppercase
    Age: i32;       // Public - uppercase
}

fn print(self: Person*) {
    printf("Name: %s", self.Name);
    printf("Age: %d", self.Age);
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

#### Type Inference with `:=`

jlang supports type inference using the `:=` operator. The type is automatically inferred from the initializer expression:

```rust
// Type inferred as i32
var x := 42;

// Type inferred as f64
var pi := 3.14159;

// Type inferred as bool
var ready := true;

// Type inferred from expression (i32)
var sum := x + 10;

// Type inferred from comparison (bool)
var isPositive := x > 0;
```

<h6><i>Type inference reduces verbosity while maintaining type safety. The compiler determines the type at compile-time, so there's no runtime overhead.</i></h6>

#### Immutable variables with `val`

jlang provides `val` as the immutable counterpart to `var`. A `val` variable must be initialized at declaration and cannot be reassigned, incremented, or decremented afterward:

```rust
// Immutable with explicit type
val pi: f64 = 3.14159;

// Immutable with type inference
val maxRetries := 3;

// Mutable variable
var counter: i32 = 0;
counter = counter + 1;  // OK

// Compile errors:
// pi = 2.71;           // ERROR: Cannot assign to immutable variable 'pi'
// maxRetries++;        // ERROR: Cannot modify immutable variable 'maxRetries'
// maxRetries += 1;     // ERROR: Cannot assign to immutable variable 'maxRetries'
```

All mutation operations are rejected at compile time: direct assignment (`=`), compound assignment (`+=`, `-=`, `*=`, `/=`, `%=`), and increment/decrement (`++`, `--`).

| Keyword | Mutability | Use case |
|---------|-----------|----------|
| `var` | Mutable | Loop counters, accumulators, state that changes |
| `val` | Immutable | Constants, configuration, values that shouldn't change |

<h6><i>The `val` keyword was chosen over `const` to pair naturally with `var` - the single-character visual difference makes mutability easy to spot when scanning code. This convention follows Kotlin and Scala where `val`/`var` is the standard mutable/immutable distinction. Using `val` by default communicates intent: if a binding doesn't need to change, declare it immutable so the compiler can enforce it.</i></h6>

> [!TIP]
> Prefer `val` over `var` when possible. Immutable bindings make code easier to reason about and prevent accidental mutation bugs.

<h6><i>See `samples/immutable_val.j` for a working example.</i></h6>

#### Unused variables are compile-time errors

jlang enforces that all declared variables must be used. If you declare a variable but never read from it, the compiler will emit an error:

```rust
fn main() -> i32 {
    var x: i32 = 10;
    var y: i32 = 20;  // ERROR: unused variable 'y'
    return x;
}
```

This also applies to function parameters:

```rust
fn add(a: i32, b: i32) -> i32 {  // ERROR: unused parameter 'b'
    return a;
}
```

<h6><i>This strictness catches common bugs like typos in variable names, forgotten logic, or dead code. It follows the philosophy of Go and Rust where unused variables indicate a likely mistake.</i></h6>

> [!TIP]
> If you intentionally want to ignore a value, use a naming convention like `_unused` or refactor the code to not declare the variable at all.

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

#### Comments

```rust
// Single-line comment

/* Multi-line
   block comment */

var x: i32 = 42; // inline comment
```

<h6><i>jlang supports C-style single-line (`//`) and block (`/* */`) comments. Block comments can span multiple lines.</i></h6>

#### Control Flow: C-style if/else, while, and for

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

#### For loops

```rust
for (var i: i32 = 0; i < 10; i++) {
    printf("i = %d", i);
}
```

The `for` loop has three clauses separated by semicolons: `for (init; condition; update) { body }`.

- **init**: Executed once before the loop starts. Can be a variable declaration or an expression. Can be empty.
- **condition**: Evaluated before each iteration. The loop continues while this is true. Can be empty (infinite loop).
- **update**: Executed after each iteration. Can be empty.

```rust
// With type inference
for (var i := 0; i < 5; i++) {
    printf("%d", i);
}

// Counting down
for (var i: i32 = 10; i > 0; i--) {
    printf("Countdown: %d", i);
}

// Using prefix increment
for (var i: i32 = 0; i < 5; ++i) {
    printf("%d", i);
}
```

<h6><i>The `for` loop is syntactic sugar over the `while` loop, following standard C-style semantics. All three clauses are optional - omitting the condition creates an infinite loop. Braces are optional for single statements.</i></h6>

<h6><i>See `samples/control_flow.j` for comprehensive examples of all control flow constructs.</i></h6>

#### Structs: colon for interface implementation

```rust
struct Person : IPrintable {
    Name: char*;
    age: i32;
}
```

<h6><i>Using `:` for interface implementation follows C++/C# conventions.</i></h6>

#### Struct field visibility

Field visibility is determined by the first character of the field name:

| First Character | Visibility | Access |
|-----------------|------------|--------|
| **Uppercase** (A-Z) | Public | Accessible from anywhere |
| **lowercase** (a-z) | Private | Only accessible within struct methods |

```rust
struct Person {
    Name: char*;    // Public - starts with uppercase 'N'
    Age: i32;       // Public - starts with uppercase 'A'
    id: i64;        // Private - starts with lowercase 'i'
    cache: bool;    // Private - starts with lowercase 'c'
}

fn printPerson(p: Person*) {
    printf("Name: %s", p.Name);  // OK - Name is public
    printf("Age: %d", p.Age);    // OK - Age is public
    // printf("ID: %d", p.id);   // ERROR - id is private
}
```

<h6><i>This convention is inspired by Go's visibility rules, where capitalization determines whether an identifier is exported. It makes visibility immediately visible in the code without requiring additional keywords.</i></h6>

> [!TIP]
> Use **Uppercase** for fields that are part of your struct's public API. Use **lowercase** for internal implementation details that should not be accessed directly.

<h6><i>See `samples/struct_features.j` for a comprehensive example of all struct features including visibility, interfaces, and various field types.</i></h6>

#### Methods: explicit `self` parameter

```rust
fn print(self: Person*) {
    printf("Name: %s", self.Name);
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

#### Null Safety

Pointer types in jlang are **non-nullable by default**. To allow a pointer to hold `null`, you must explicitly mark it with `*?`:

```rust
var p: Person* = alloc<Person>();   // non-nullable, CANNOT be null
var q: Person*? = null;              // nullable, CAN be null
```

The compiler enforces null safety at compile time with three rules:

**1. Cannot assign `null` to a non-nullable pointer:**

```rust
var p: Person* = null;   // ERROR: Cannot assign null to non-nullable pointer type 'Person*'. Use 'Person*?' to allow null.
p = null;                // ERROR: Cannot assign null to non-nullable variable 'p'.
```

**2. Non-nullable pointers must be initialized:**

```rust
var p: Person*;          // ERROR: Non-nullable pointer 'p' must be initialized.
var q: Person*? = null;  // OK - nullable pointers can be left null
```

**3. Cannot access members on a nullable pointer:**

```rust
var q: Person*? = null;
q.Name;                  // ERROR: Cannot access member 'Name' on nullable type 'Person*?'. Check for null first.
```

Non-nullable pointers are guaranteed safe to dereference - no null check needed:

```rust
var p: Person* = alloc<Person>();
p.Name;                  // OK - p is guaranteed non-null
```

| Syntax | Nullable | Can be `null` | Member access |
|--------|----------|---------------|---------------|
| `Type*` | No | Compile error | Allowed |
| `Type*?` | Yes | Allowed | Compile error |

<h6><i>This design is inspired by Kotlin's null safety system, where `Type` is non-nullable and `Type?` is nullable. By making non-null the default, the most common case (pointers that should never be null) requires no extra syntax, while the less common case (nullable pointers) is explicitly marked. This eliminates null pointer dereferences at compile time rather than at runtime.</i></h6>

> [!TIP]
> Prefer `Type*` (non-nullable) whenever possible. Only use `Type*?` when the pointer genuinely needs to represent the absence of a value.

<h6><i>See `samples/null_safety.j` for a working example.</i></h6>

> [!NOTE]
> Smart casts after null checks (e.g., automatically treating a `Person*?` as `Person*` inside an `if (p != null)` block) are a planned future enhancement.

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

#### Increment and Decrement Operators

jlang supports both prefix and postfix increment (`++`) and decrement (`--`) operators:

```rust
var x: i32 = 5;

// Prefix: increment first, then return new value
var a: i32 = ++x;  // x becomes 6, a is 6

// Postfix: return original value, then increment
var b: i32 = x++;  // b is 6, x becomes 7

// Same for decrement
var y: i32 = 10;
var c: i32 = --y;  // y becomes 9, c is 9
var d: i32 = y--;  // d is 9, y becomes 8
```

| Operator | Name | Returns | When mutation happens |
|----------|------|---------|----------------------|
| `++x` | Prefix increment | New value | Before returning |
| `x++` | Postfix increment | Original value | After returning |
| `--x` | Prefix decrement | New value | Before returning |
| `x--` | Postfix decrement | Original value | After returning |

<h6><i>The operand must be a variable - you cannot use these operators on literals or expressions like `++5` or `(a+b)++`.</i></h6>

<details>
<summary><b>LLVM IR comparison</b></summary>

Given this jlang code:

```rust
var x: i32 = 5;
printf("%d", ++x);  // prefix
printf("%d", x++);  // postfix
```

**Prefix `++x`** - returns the new value:

```llvm
  %load = load i32, ptr %x           ; load current value (5)
  %inc = add i32 %load, 1            ; add 1 (result: 6)
  store i32 %inc, ptr %x             ; store new value back
  ; %inc (6) is used as the result
```

<h6><i>The incremented value `%inc` is passed to printf, so it prints 6.</i></h6>

**Postfix `x++`** - returns the original value:

```llvm
  %load = load i32, ptr %x           ; load current value (6)
  %inc = add i32 %load, 1            ; add 1 (result: 7)
  store i32 %inc, ptr %x             ; store new value back
  ; %load (6) is used as the result (not %inc!)
```

<h6><i>The original value `%load` is passed to printf, so it prints 6. But x is now 7.</i></h6>

The key difference is which value gets returned:
- **Prefix**: returns `%inc` (the new value)
- **Postfix**: returns `%load` (the original value)

Both perform the same mutation (load, add, store), but they differ in what value they produce as the expression result.

</details>

<h6><i>These operators follow C/C++/Java semantics. Use prefix when you need the updated value; use postfix when you need the value before the update.</i></h6>

> [!NOTE]
> **No performance difference between prefix and postfix**
>
> Unlike C++ iterators, there is no performance benefit to using `++x` over `x++` in jlang. Both generate the same three operations (load, add, store), and the only difference is which already-computed register value gets returned. No temporary copy is created.
>
> The "prefer `++i` over `i++`" advice comes from C++ where postfix on complex objects (like iterators) requires constructing a temporary copy:
>
> ```cpp
> // C++ iterator postfix - expensive!
> Iterator operator++(int) {
>     Iterator copy = *this;  // make a copy
>     ++(*this);              // increment original
>     return copy;            // return the copy
> }
> ```
>
> For primitive types, modern compilers optimize both to identical machine code. Choose based on semantics, not performance.

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
