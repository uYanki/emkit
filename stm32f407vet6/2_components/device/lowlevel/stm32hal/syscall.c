#include <stdio.h>

#if !defined(__MICROLIB)

// https://mklimenko.github.io/english/2018/11/02/disable-semihosting/

#if __ARMCC_VERSION >= 6000000
__asm(".global __use_no_semihosting");
#elif __ARMCC_VERSION >= 5000000
#pragma import(__use_no_semihosting)
#else
#error Unsupported compiler
#endif

#include <rt_sys.h>
#include <rt_misc.h>
#include <time.h>

const char __stdin_name[]  = ":tt";
const char __stdout_name[] = ":tt";
const char __stderr_name[] = ":tt";

FILEHANDLE _sys_open(const char* name, int openmode)
{
    return 1;
}

int _sys_close(FILEHANDLE fh)
{
    return 0;
}

char* _sys_command_string(char* cmd, int len)
{
    return NULL;
}

int _sys_write(FILEHANDLE fh, const unsigned char* buf, unsigned len, int mode)
{
    return 0;
}

int _sys_read(FILEHANDLE fh, unsigned char* buf, unsigned len, int mode)
{
    return -1;
}

void _ttywrch(int ch)
{
}

int _sys_istty(FILEHANDLE fh)
{
    return 0;
}

int _sys_seek(FILEHANDLE fh, long pos)
{
    return -1;
}

long _sys_flen(FILEHANDLE fh)
{
    return -1;
}

void _sys_exit(int return_code)
{
    while (1);
}

clock_t clock(void)
{
    clock_t tmp;
    return tmp;
}

void _clock_init(void)
{
}

time_t time(time_t* timer)
{
    time_t tmp;
    return tmp;
}

int system(const char* string)
{
    return 0;
}

char* getenv(const char* name)
{
    return NULL;
}

void _getenv_init(void)
{
}

#endif

#ifndef NDEBUG

#include <stdlib.h>
#include <stdio.h>

int fputs(const char* str, FILE* stream)
{
    printf("%s", str);
    return 0;
}

__attribute__((weak, noreturn)) void __aeabi_assert(const char* expr, const char* file, int line)
{
    char str[12], *p;

    fputs("*** assertion failed: ", stderr);
    fputs(expr, stderr);
    fputs(", file ", stderr);
    fputs(file, stderr);
    fputs(", line ", stderr);

    p    = str + sizeof(str);
    *--p = '\0';
    *--p = '\n';
    while (line > 0)
    {
        *--p = '0' + (line % 10);
        line /= 10;
    }
    fputs(p, stderr);

    abort();
}

__attribute__((weak)) void abort(void)
{
    for (;;);
}

#endif
