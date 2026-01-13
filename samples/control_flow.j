fn main() -> i32 {
    var x: i32 = 10;
    var y: i32 = 20;

    // Simple if statement
    if (x == 10) {
        printf("x is ten");
    }

    // If-else statement
    if (x == y) {
        printf("x equals y");
    } else {
        printf("x does not equal y");
    }

    // If-else if-else chain
    if (x == 0) {
        printf("x is zero");
    } else if (x == 10) {
        printf("x is ten");
    } else if (x == 20) {
        printf("x is twenty");
    } else {
        printf("x is something else");
    }

    // Nested if statements
    if (x == 10) {
        if (y == 20) {
            printf("x is 10 and y is 20");
        }
    }

    // Single-line statements (without braces)
    if (x == 10)
        printf("single line if");
    else
        printf("single line else");

    // While loops
    var i: i32 = 0;

    while (i < 5) {
        printf("i = %d", i);
        i = i + 1;
    }

    printf("Loop finished, i = %d", i);

    var count: i32 = 3;
    while (count > 0) {
        printf("Countdown: %d", count);
        count = count - 1;
    }
    printf("Liftoff!");

    return 0;
}
