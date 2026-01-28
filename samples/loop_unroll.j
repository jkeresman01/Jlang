fn main() -> i32 {
    // Should be unrolled (4 iterations, <= MAX_UNROLL_COUNT)
    for (var i: i32 = 0; i < 4; i++) {
        printf("i = %d", i);
    }

    // Should be unrolled (3 iterations, prefix increment)
    for (var j: i32 = 0; j < 3; ++j) {
        printf("j = %d", j);
    }

    // Should be unrolled (counting down, 4 iterations)
    for (var k: i32 = 4; k > 0; k--) {
        printf("k = %d", k);
    }

    // Should NOT be unrolled (10 > MAX_UNROLL_COUNT)
    for (var m: i32 = 0; m < 10; m++) {
        printf("m = %d", m);
    }

    // Should be unrolled (<= operator, 5 iterations)
    for (var n: i32 = 1; n <= 5; n++) {
        printf("n = %d", n);
    }

    return 0;
}
