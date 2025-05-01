# Jlang
Procedural programming language based on C

```C
struct Person {
    var firstName char*; 
    var age int32; 
}

int32 main()
{
    var Person* p = (struct Person*) jalloc(sizeof(struct Person));

    if (p == NULL) {
        jout("No can do"); 
    }
    else {
        jout("Incredible");
    }

    jfree(p);
}
