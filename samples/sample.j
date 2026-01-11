interface IPrintable {
    fn print();
}

struct Person : IPrintable {
    firstName: char*;
    age: i32;
}

fn print(self: Person*) {
    jout("First name: %s", self.firstName);
    jout("Age: %d", self.age);
}

struct Frog : IPrintable {
    name: char*;
    age: i32;
}

fn print(self: Frog*) {
    jout("Frogs name: %s", self.name);
    jout("Age: %d", self.age);
}

fn main() -> i32 {
    var person: Person* = alloc<Person>();

    if (person == null) {
        jout("No can do");
    } else {
        jout("Incredible");
    }

    free(person);
    return 0;
}
