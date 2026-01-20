fn main() -> i32 {
    var a: i8 = 127;
    var b: i16 = 32767;
    var c: i32 = 42;
    var d: i64 = 9223372036854775807;

    var e: u8 = 255;
    var f: u16 = 65535;
    var g: u32 = 100;
    var h: u64 = 18446744073709551615;

    var pi: f32 = 3.14;
    var precise: f64 = 3.141592653589793;

    var isReady: bool = true;
    var isEmpty: bool = false;

    var letter: char = 'A';
    var digit: char = '9';
    var newline: char = '\n';
    var tab: char = '\t';
    var backslash: char = '\\';
    var quote: char = '\'';
    var null_char: char = '\0';

    var name: char* = "milica";
    var greeting: char* = "Hello, World!";

    var ptr: i32* = null;

    return 0;
}

fn printMessage() {
    printf("This function returns nothing (void)");
}

fn add(x: i32, y: i32) -> i32 {
    return x + y;
}

fn printName(name: char*) {
    printf("Name: %s", name);
}
