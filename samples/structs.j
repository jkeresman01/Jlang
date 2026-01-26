interface IPrintable {
    fn print();
}

struct Person : IPrintable {
    Name: char*;    // Public - uppercase
    Age: i32;       // Public - uppercase
}

fn print(self: Person*) {
    printf("Name: %s", self.Name);
    printf("Age: %d", self.Age);
}

struct Frog : IPrintable {
    Name: char*;    // Public
    Age: i32;       // Public
}

fn print(self: Frog*) {
    printf("Frog's name: %s", self.Name);
    printf("Age: %d", self.Age);
}

fn main() -> i32 {
    var person: Person* = alloc<Person>();

    if (person == null) {
        printf("No can do");
    } else {
        printf("Incredible");
    }

    free(person);
    return 0;
}
