int a;
int b;
int main()
{
    int a;
    a = 1;
    int b = 3;
    {
        int a = 4;
        int b = a;
        int c;
    }
    return 0;
}