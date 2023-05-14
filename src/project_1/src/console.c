#include "console.h"
#include <stdio.h>

#define RED "\033[31m"
#define GRN "\033[32m"
#define YEL "\033[33m"
#define RESET "\033[0m"

void red()
{
    printf(RED);
}

void green()
{
    printf(GRN);
}

void yellow()
{
    printf(YEL);
}

void reset()
{
    printf(RESET);
}

void console_print_error(char message[])
{
    red();
    printf("%s\n", message);
    reset();
}

void console_print_warning(char message[])
{
    yellow();
    printf("%s\n", message);
    reset();
}

void console_print(char message[])
{
    green();
    printf("%s\n", message);
    reset();
}
