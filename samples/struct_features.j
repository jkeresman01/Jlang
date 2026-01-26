// This file demonstrates all struct-related features in jlang:
//
// - Basic struct definition
// - Field visibility (Uppercase = public, lowercase = private)
// - Interface implementation
// - Memory allocation and deallocation
// - Field access

// Interfaces
// Interfaces define a contract that structs can implement.
// Methods declared in an interface must be implemented for the struct.

interface IPrintable {
    fn print();
}

interface IComparable {
    fn compare();
}

// Basic Struct
// A simple struct with no interface implementation.
// All fields use the visibility convention:
//   - Uppercase first letter = PUBLIC (accessible from anywhere)
//   - lowercase first letter = PRIVATE (internal use only)

struct Point {
    X: i32;         // Public - can be accessed from any function
    Y: i32;         // Public - can be accessed from any function
}

// Struct with Mixed Visibility
// Demonstrates the visibility convention with mixed public/private fields.

struct Counter {
    Value: i32;     // Public - the current count, readable by anyone
    Max: i32;       // Public - the maximum allowed value
    min: i32;       // Private - internal minimum bound
    step: i32;      // Private - internal increment amount
}

// Struct with Interface Implementation
// Uses colon syntax to implement an interface: struct Name : Interface

struct Person : IPrintable {
    Name: char*;    // Public - person's name
    Age: i32;       // Public - person's age
    id: i64;        // Private - internal identifier
    active: bool;   // Private - internal status flag
}

// Method implementing IPrintable for Person
// Uses explicit 'self' parameter (pointer to the struct)
fn print(self: Person*) {
    printf("Person: %s, Age: %d", self.Name, self.Age);
}

// Struct with Multiple Field Types
// Demonstrates all supported field types in structs.

struct DataRecord {
    // Public fields (Uppercase)
    Id: i64;            // 64-bit signed integer
    Name: char*;        // String (char pointer)
    Score: f64;         // Double-precision float
    Active: bool;       // Boolean
    Grade: char;        // Single character
    Count: i32;         // 32-bit signed integer

    // Private fields (lowercase)
    checksum: u32;      // Unsigned 32-bit (internal)
    flags: u8;          // Unsigned 8-bit (internal)
    cache: char*;       // Internal cache pointer
}

// Struct with All Integer Types
// Shows all integer type variants as struct fields.

struct IntegerFields {
    // Signed integers (Public)
    SignedByte: i8;
    SignedShort: i16;
    SignedInt: i32;
    SignedLong: i64;

    // Unsigned integers (Public)
    UnsignedByte: u8;
    UnsignedShort: u16;
    UnsignedInt: u32;
    UnsignedLong: u64;

    // Private integers
    internalCount: i32;
    secretKey: u64;
}

// Struct with Pointer Fields
// Demonstrates pointer fields for linked structures.

struct Node {
    Value: i32;         // Public - the node's value
    Next: Node*;        // Public - pointer to next node (self-referential)
    prev: Node*;        // Private - pointer to previous node
}

// Struct Implementing Multiple Concepts
// A more complex struct showing real-world usage patterns.

struct Employee : IPrintable {
    // Public API
    Name: char*;        // Employee name
    Department: char*;  // Department name
    Salary: f64;        // Annual salary
    Id: i32;            // Employee ID

    // Private implementation details
    taxRate: f32;       // Internal tax calculation
    bonus: f64;         // Internal bonus tracking
    hireDate: i64;      // Internal timestamp
    terminated: bool;   // Internal status
}

fn print(self: Employee*) {
    printf("Employee #%d: %s", self.Id, self.Name);
    printf("Department: %s", self.Department);
    printf("Salary: $%.2f", self.Salary);
}

// Main Function - Demonstrating Struct Usage

fn main() -> i32 {
    // Allocate a Point struct
    var pt: Point* = alloc<Point>();
    if (pt == null) {
        printf("Failed to allocate Point");
        return 1;
    }
    printf("Allocated Point struct");

    // Allocate a Person struct
    var person: Person* = alloc<Person>();
    if (person == null) {
        printf("Failed to allocate Person");
        free(pt);
        return 1;
    }
    printf("Allocated Person struct");

    // Allocate a Counter struct
    var counter: Counter* = alloc<Counter>();
    if (counter == null) {
        printf("Failed to allocate Counter");
        free(pt);
        free(person);
        return 1;
    }
    printf("Allocated Counter struct");

    // Allocate an Employee struct
    var emp: Employee* = alloc<Employee>();
    if (emp == null) {
        printf("Failed to allocate Employee");
        free(pt);
        free(person);
        free(counter);
        return 1;
    }
    printf("Allocated Employee struct");

    // Clean up - always free allocated memory
    free(pt);
    free(person);
    free(counter);
    free(emp);

    printf("All structs freed successfully");

    return 0;
}
