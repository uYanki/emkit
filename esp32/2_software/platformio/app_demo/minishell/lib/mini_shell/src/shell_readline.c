#include "shell_readline.h"
#include "shell_io.h"
#include "shell_config.h"
#include <string.h>
#define CTL_CH(ch) ((ch) - 'A' + 1)

static int is_line_done;
static int line_end;
static int line_cursor;
static int key_flag;
// static int key_len;
// static char key_buffer[5];
static char line_buffer[CONFIG_SHELL_INPUT_BUFFSIZE + 1];

void add_char(char ch)
{
  if (line_end < CONFIG_SHELL_INPUT_BUFFSIZE && ch >= ' ')
  {
    int len = line_end - line_cursor;
    line_end++;
    line_buffer[line_end] = '\0';

    memmove(&line_buffer[line_cursor + 1], &line_buffer[line_cursor], len);
    line_buffer[line_cursor] = ch;

    shell_puts(&line_buffer[line_cursor++]);
    while (len > 0)
    {
      shell_putc('\b');
      len--;
    }
  }
  else
  {
    shell_putc('\a');
  }
}

void end_input(void)
{
  line_cursor = 0;

  line_end = 0;
  is_line_done = 1;
  shell_puts("\r\n");
}

void rubout(void)
{
  if (line_cursor > 0)
  {
    int len = line_end - (--line_cursor);
    line_end--;
    memmove(&line_buffer[line_cursor], &line_buffer[line_cursor + 1], len);
    shell_putc('\b');
    shell_puts(&line_buffer[line_cursor]);
    shell_putc(' ');
    do
    {
      shell_putc('\b');
      len--;
    } while (len > 0);
  }
  else
  {
    shell_putc('\a');
  }
}

// 回到行首
void to_line_home(void)
{
  while (line_cursor > 0)
  {
    shell_putc('\b');
    line_cursor--;
  }
}

// 回到行尾
void to_line_end()
{
  shell_puts(line_buffer + line_cursor);
  line_cursor = line_end;
}

// 向前移动光标
void forward_cursor(void)
{
  if (line_cursor > 0)
  {
    shell_putc('\b');
    line_cursor--;
  }
  else
  {
    shell_putc('\a');
  }
}

// Move backward (right).
void backward_cursor(void)
{
  if (line_cursor < line_end)
  {
    shell_putc(line_buffer[line_cursor]);
    line_cursor++;
  }
  else
  {
    shell_putc('\a');
  }
}

void delete (void)
{
  if (line_cursor < line_end)
  {
    int len = line_end - line_cursor;
    line_end--;

    memmove(&line_buffer[line_cursor], &line_buffer[line_cursor + 1], len);
    shell_puts(&line_buffer[line_cursor]);
    shell_putc(' ');
    do
    {
      shell_putc('\b');
      len--;
    } while (len > 0);
  }
}

int key_handler(char ch)
{
  if (ch == '\033')
  {
    key_flag = 1;
    return 1;
  }
  if (key_flag == 1)
  {
    if (ch == '[')
    {
      key_flag = 2;
      return 1;
    }
    else
    {
      key_flag = 0;
      return 0;
    }
  }
  if (key_flag == 2)
  {
    switch (ch)
    {
    case 'A':
      key_flag = 0;
      return 1;
    case 'B':
      key_flag = 0;
      return 1;
    case 'C':
      backward_cursor();
      key_flag = 0;
      return 1;
    case 'D':
      forward_cursor();
      key_flag = 0;
      return 1;
    default:
      key_flag = 0;
      return 0;
    }
  }
  return 0;
}

// 处理输入字符
void input_handler(char ch)
{
  if (key_handler(ch))
    return;
  switch (ch)
  {
  case '\r':
  case '\n':
    end_input();
    break;
  case 255:
  case 127:
  case 8: // backspace
    rubout();
    break;
  case CTL_CH('A'): // HOME
    to_line_home();
    break;
  case CTL_CH('E'): // END
    to_line_end();
    break;
  case CTL_CH('B'): // <-- (left arrow)
    forward_cursor();
    break;
  case CTL_CH('F'): // --> (right arrow)
    backward_cursor();
    break;
  case CTL_CH('D'): // DELETE
    delete ();
    break;
  default: // add current character to the buffer
    add_char(ch);
    break;
  }
}

char *shell_readline()
{
  char input;
  shell_puts(CONFIG_SHELL_PROMPT);
  // clean last line.
  is_line_done = 0;
  *line_buffer = '\0';

  // start read the new line.
  while (!is_line_done)
  {
    while (!shell_getc(&input))
    {
    }

    input_handler(input);
  }

  return line_buffer;
}