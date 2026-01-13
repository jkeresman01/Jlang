fn main() -> i32 {
    var a: bool = true;
    var b: bool = false;

    if (a && b) {
        printf("both true\n");
    }

    if (a || b) {
        printf("at least one true\n");
    }

    if (!b) {
        printf("b is false\n");
    }

    if (!a) {
        printf("a is false\n");
    }

    return 0;
}
