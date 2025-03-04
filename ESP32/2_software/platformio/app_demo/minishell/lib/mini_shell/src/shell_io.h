#ifndef MINI_SHELL_IO_H
#define MINI_SHELL_IO_H
extern void shell_putc(char ch);
extern int shell_getc(char *ch);
int shell_printf(const char *format, ...);
void shell_puts(const char *str);
#endif