// Type inference examples

fn main() -> i32 {
    // Inferred as i32 (integer literal default)
    var x := 42;

    // Inferred as f64 (float literal default)
    var pi := 3.14159;

    // Inferred as bool
    var flag := true;

    // Explicit type still works
    var count: i32 = 100;

    // Use the variables to avoid unused variable errors
    printf("x = %d\n", x);
    printf("pi = %f\n", pi);
    printf("count = %d\n", count);

    if (flag) {
        printf("flag is true\n");
    }

    // Type inference with expressions
    var sum := x + count;
    printf("sum = %d\n", sum);

    // Inference with boolean expressions
    var isPositive := x > 0;
    if (isPositive) {
        printf("x is positive\n");
    }

    return 0;
}
