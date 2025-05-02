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
    var Person* p = (struct Person*) jalloc(sizeof(struct Person));

    if (p == NULL) 
    {
        jout("No can do"); 
    }
    else 
    {
        jout("Incredible");
    }

    jfree(p);
}