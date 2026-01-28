fn main() -> i32 {
    var a: i32 = 255;
    var b: i32 = 0;

    // Bitwise AND
    b = a & 15;
    printf("0xFF & 15 = %d\n", b);

    // Bitwise OR
    b = 16 | 8;
    printf("16 | 8 = %d\n", b);

    // Bitwise XOR
    b = 255 ^ 15;
    printf("255 ^ 15 = %d\n", b);

    // Bitwise NOT
    var c: i32 = ~0;
    printf("~0 = %d\n", c);

    // Left shift
    b = 1 << 4;
    printf("1 << 4 = %d\n", b);

    // Right shift
    b = 32 >> 2;
    printf("32 >> 2 = %d\n", b);

    // Compound assignment
    var x: i32 = 255;
    x &= 15;
    printf("255 &= 15 -> %d\n", x);

    x |= 48;
    printf("|= 48 -> %d\n", x);

    x ^= 5;
    printf("^= 5 -> %d\n", x);

    var s: i32 = 1;
    s <<= 8;
    printf("1 <<= 8 -> %d\n", s);

    s >>= 3;
    printf(">>= 3 -> %d\n", s);

    // XOR swap
    var p: i32 = 42;
    var q: i32 = 99;
    printf("before swap: p=%d q=%d\n", p, q);
    p ^= q;
    q ^= p;
    p ^= q;
    printf("after swap:  p=%d q=%d\n", p, q);

    return 0;
}
