#include <stdio.h>
#include <unistd.h>

int main()
{
    if(fork() && fork())
    {
        if (fork() || fork())
        {
            printf("Hello\n");
            fork();
        }
    }
    printf("World\n");
    return 0;
}