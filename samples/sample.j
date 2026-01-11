interface IPrintable {
    fn print();
}

struct Person : IPrintable {
    firstName: char*;
    age: i32;
}

fn print(self: Person*) {
    printf("First name: %s", self.firstName);
    printf("Age: %d", self.age);
}

struct Frog : IPrintable {
    name: char*;
    age: i32;
}

fn print(self: Frog*) {
    printf("Frogs name: %s", self.name);
    printf("Age: %d", self.age);
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
