// This is a single-line comment

/* This is a
   multi-line
   block comment */

fn main() -> i32 {
    // Comment before variable
    var x: i32 = 10; // inline comment

    /* Block comment in code */
    var y: i32 = 20;

    // Division should still work
    var z: i32 = x / y;

    return 0;
}
