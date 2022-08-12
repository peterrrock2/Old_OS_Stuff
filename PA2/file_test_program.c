#include <stdio.h>
#include <stdlib.h>  // for malloc
#include <fcntl.h>   // for open()
#include <unistd.h>  // for read() write() lseek() close()
#include <errno.h>   // to print errors
#include <string.h>

#define BUFF_BYTES 1024  // We use this buff size in the next part


void print_menu();
void convertStdinToInt(int *meint);


int main(int argc, char *argv[])
{
    char* buff = malloc(sizeof(char) * BUFF_BYTES);
    int rbytes, offset, whence;

    int file = open(argv[1], O_RDWR);

    if(file < 0)
    {
        printf("Error opening file.\n");
        printf("Error %s\n", strerror(errno));
        return -1;
    }

    while(1)
    {
        print_menu();
        memset(buff, 0, BUFF_BYTES);        // Clear the buffer before every loop
        if (!fgets(buff, BUFF_BYTES, stdin)) break;  // End loop with ctrl+d
                                            // Note: only the first character is read
        switch (*buff)
        {
            case 'r':
                printf("Enter the number of bytes you want to read: ");
                convertStdinToInt(&rbytes);
                if (rbytes >= BUFF_BYTES)
                {
                    printf("Input number of bytes exceeds limit of %d\n", BUFF_BYTES - 1);
                    break;
                }
                memset(buff, 0, BUFF_BYTES);  // Clear the buffer!
                read(file, buff, rbytes);
                printf("Output of file: \n");
                puts(buff);
                printf("\n");
                break;
                
            case 'w':
                printf("Enter the data you want to write: ");
                memset(buff, 0, BUFF_BYTES);     // Clear the buffer!
                fgets(buff, BUFF_BYTES, stdin);
                write(file, buff, BUFF_BYTES);
                break;

            case 's':
                printf("Enter an offset value: ");
                convertStdinToInt(&offset);
                printf("Enter a value for whence: ");
                convertStdinToInt(&whence);
                printf("Observed values of offset and whence %d and %d\n", offset, whence);
                lseek(file, offset, whence);
                break;

            default:
                printf("Invalid input of ");
                puts(buff);
                break;
        }
    }

    free(buff);
    close(file);
    return 0;
}

void print_menu()
{
    printf("Please select an option:\n");
        printf("\t(r) Read from file.\n");
        printf("\t(w) Write to file.\n");
        printf("\t(s) Set offset and whence.\n");
        printf("\tPress ctrl+d to quit.\n");
}

void convertStdinToInt(int* meint)
{
    char *intbuff = malloc(128);
    memset(intbuff, 0, 128);
    char *input = NULL;
    input = fgets(intbuff, 128, stdin);
    char *endptr = NULL;
    *meint = strtol(input, &endptr, 10);
    free(intbuff);
}