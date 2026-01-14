fn sideEffect() -> i32 {
    printf("sideEffect() was called!\n");
    return 1;
}

fn main() -> i32 {
    printf("Testing short-circuit &&:\n");
    if (false && sideEffect()) {
        printf("This won't print\n");
    }
    printf("\n");

    printf("Testing non-short-circuit 'and':\n");
    if (false and sideEffect()) {
        printf("This won't print\n");
    }
    printf("\n");

    printf("Testing short-circuit ||:\n");
    if (true || sideEffect()) {
        printf("This will print\n");
    }
    printf("\n");

    printf("Testing non-short-circuit 'or':\n");
    if (true or sideEffect()) {
        printf("This will print\n");
    }

    return 0;
}
