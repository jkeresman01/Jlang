interface IPrintable
{
    void print();
}

struct Person -> IPrintable 
{
    var firstName char*; 
    var age int32; 
}

void print() -> Person p
{
    jout("First name: %s", p.firstName); 
    jout("Age: %d", p.age); 
}

int32 main()
{
    var person Person* = (struct Person*) jalloc(sizeof(struct Person));

    if (person == NULL) 
    {
        jout("No can do"); 
    }
    else 
    {
        jout("Incredible");
    }

    jfree(p);
}