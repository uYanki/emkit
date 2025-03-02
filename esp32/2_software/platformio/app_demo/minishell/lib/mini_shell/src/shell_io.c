#include "shell_io.h"

#include <stdarg.h>
#include <stdio.h>

#include "shell_config.h"

#if defined(__GNUC__)
#ifndef __weak
#define __weak __attribute__((weak))
#endif /* __weak */
#endif /* __GNUC__ */

__weak int shell_printf(const char *format, ...) {
  static char shell_printf_buffer[CONFIG_SHELL_PRINTF_BUFFER_SIZE];

  int length = 0;

  va_list ap;
  va_start(ap, format);

  length = vsnprintf(shell_printf_buffer, CONFIG_SHELL_PRINTF_BUFFER_SIZE,
                     format, ap);

  va_end(ap);

  for (int i = 0; i < length; i++) {
    shell_putc(shell_printf_buffer[i]);
  }

  return length;
}

__weak void shell_puts(const char *str) {
  while (*str) {
    shell_putc(*str);
    str++;
  }
}

__weak void shell_putc(char ch) {}
