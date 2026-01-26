// This sample demonstrates unused variable detection.
// The compiler will report errors for variables that are declared but never used.

fn add(a: i32, b: i32) -> i32 {
    // ERROR: 'b' is unused
    return a;
}

fn main() -> i32 {
    var x: i32 = 10;
    var y: i32 = 20;
    var z: i32 = 30;  // ERROR: 'z' is unused

    return add(x, y);
}
