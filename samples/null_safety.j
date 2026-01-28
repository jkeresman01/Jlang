// Null safety in jlang
// Pointer types are non-nullable by default. Use *? to allow null.

struct Person {
    Name: i32;
    Age: i32;
}

fn main() -> i32 {
    // Non-nullable pointer - guaranteed to be valid
    var p: Person* = alloc<Person>();
    p.Name;  // OK - p is guaranteed non-null
    p.Age;   // OK

    // Nullable pointer - explicitly allows null
    var q: Person*? = null;

    // Nullable pointers can be compared to null
    if (q == null) {
        printf("q is null\n");
    }

    // Reassigning null to nullable is fine
    q = null;

    // The following would cause compile errors:
    // var bad: Person* = null;   // ERROR: Cannot assign null to non-nullable pointer type
    // var bad2: Person*;         // ERROR: Non-nullable pointer must be initialized
    // q.Name;                    // ERROR: Cannot access member on nullable type
    // p = null;                  // ERROR: Cannot assign null to non-nullable variable

    free(p);
    return 0;
}
