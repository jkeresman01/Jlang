// Immutable variables with 'val'
// val declares a variable that cannot be reassigned after initialization.

fn main() -> i32 {
    // Immutable with explicit type
    val pi: f64 = 3.14159;

    // Immutable with type inference
    val x := 42;

    // Mutable variable for comparison
    var counter: i32 = 0;
    counter = counter + 1;

    printf("pi = %f\n", pi);
    printf("x = %d\n", x);
    printf("counter = %d\n", counter);

    // The following would cause compile errors:
    // x = 10;           // ERROR: Cannot assign to immutable variable 'x'
    // x++;              // ERROR: Cannot modify immutable variable 'x'
    // x += 1;           // ERROR: Cannot assign to immutable variable 'x'

    return 0;
}
