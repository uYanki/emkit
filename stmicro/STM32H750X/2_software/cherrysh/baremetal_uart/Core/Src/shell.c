#include <stdio.h>
#include <stdbool.h>
#include "csh.h"
#include "shell.h"

#define BOARD_NAME "stm32"

static chry_shell_t        csh;
static UART_HandleTypeDef* shell_uart = NULL;
static bool                login      = false;

/**
 * The  strnlen()  function  returns the number of characters in the
 * string pointed to by s, excluding the terminating null byte ('\0'),
 * but at most maxlen.  In doing this, strnlen() looks only at the
 * first maxlen characters in the string pointed to by s and never
 * beyond s+maxlen.
 *
 * @param s the string
 * @param maxlen the max size
 * @return the length of string
 */
size_t strnlen(const char* s, size_t maxlen)
{
    const char* sc;

    for (sc = s; *sc != '\0' && (size_t)(sc - s) < maxlen; ++sc) /* nothing */
        ;

    return sc - s;
}

void shell_uart_isr(void)
{
}

static uint16_t csh_sput_cb(chry_readline_t* rl, const void* data, uint16_t size)
{
    uint16_t i;
    (void)rl;

    HAL_UART_Transmit(shell_uart, (uint8_t*)data, size, 0xFF);

    // Return the number of successfully output characters
    return shell_uart->TxXferSize - shell_uart->TxXferCount;
}

static uint16_t csh_sget_cb(chry_readline_t* rl, void* data, uint16_t size)
{
    (void)rl;

    HAL_UART_Receive(shell_uart, data, size, 0);

    return shell_uart->RxXferSize - shell_uart->RxXferCount;
}

int shell_init(UART_HandleTypeDef* uart, bool need_login)
{
    chry_shell_init_t csh_init;

    if (uart == NULL)
    {
        return -1;
    }

    if (need_login)
    {
        login = false;
    }
    else
    {
        login = true;
    }

    shell_uart = uart;

    /*!< I/O callback */
    csh_init.sput = csh_sput_cb;
    csh_init.sget = csh_sget_cb;

#if defined(CONFIG_CSH_SYMTAB) && CONFIG_CSH_SYMTAB

#if defined(__GNUC__)

    extern const int FSymTab$$Base;
    extern const int FSymTab$$Limit;
    extern const int VSymTab$$Base;
    extern const int VSymTab$$Limit;

    /*!< get table from ld symbol */
    csh_init.command_table_beg  = &FSymTab$$Base;
    csh_init.command_table_end  = &FSymTab$$Limit;
    csh_init.variable_table_beg = &VSymTab$$Base;
    csh_init.variable_table_end = &VSymTab$$Limit;

#elif defined(__ICCARM__) || defined(__ICCRX__) || defined(__ICCRISCV__)

#pragma section = "FSymTab"
#pragma section = "VSymTab"

    csh_init.command_table_beg  = __section_begin("FSymTab");
    csh_init.command_table_end  = __section_end("FSymTab");
    csh_init.variable_table_beg = __section_begin("VSymTab");
    csh_init.variable_table_end = __section_end("VSymTab");

#endif

#endif

#if defined(CONFIG_CSH_PROMPTEDIT) && CONFIG_CSH_PROMPTEDIT
    static char csh_prompt_buffer[128];

    /*!< set prompt buffer */
    csh_init.prompt_buffer      = csh_prompt_buffer;
    csh_init.prompt_buffer_size = sizeof(csh_prompt_buffer);
#endif

#if defined(CONFIG_CSH_HISTORY) && CONFIG_CSH_HISTORY
    static char csh_history_buffer[128];

    /*!< set history buffer */
    csh_init.history_buffer      = csh_history_buffer;
    csh_init.history_buffer_size = sizeof(csh_history_buffer);
#endif

#if defined(CONFIG_CSH_LNBUFF_STATIC) && CONFIG_CSH_LNBUFF_STATIC
    static char csh_line_buffer[128];

    /*!< set linebuffer */
    csh_init.line_buffer      = csh_line_buffer;
    csh_init.line_buffer_size = sizeof(csh_line_buffer);
#endif

    csh_init.uid     = 0;
    csh_init.user[0] = "cherry";

    /*!< The port hash function is required,
         and the strcmp attribute is used weakly by default,
         int chry_shell_port_hash_strcmp(const char *hash, const char *str); */
    csh_init.hash[0]   = "12345678"; /*!< If there is no password, set to NULL */
    csh_init.host      = BOARD_NAME;
    csh_init.user_data = NULL;

    int ret = chry_shell_init(&csh, &csh_init);
    if (ret)
    {
        return -1;
    }

    return 0;
}

int shell_cycle(void)
{
    int ret;

restart:

    if (login)
    {
        goto restart2;
    }

    ret = csh_login(&csh);
    if (ret == 0)
    {
        login = true;
    }
    else
    {
        return 0;
    }

restart2:
    chry_shell_task_exec(&csh);

    ret = chry_shell_task_repl(&csh);

    if (ret == -1)
    {
        /*!< error */
        return -1;
    }
    else if (ret == 1)
    {
        /*!< continue */
        return 0;
    }
    else
    {
        /*!< restart */
        goto restart;
    }

    return 0;
}

void shell_lock(void)
{
    chry_readline_erase_line(&csh.rl);
}

void shell_unlock(void)
{
    chry_readline_edit_refresh(&csh.rl);
}

static int csh_exit(int argc, char** argv)
{
    (void)argc;
    (void)argv;
    login = false;
    return 0;
}
CSH_SCMD_EXPORT_ALIAS(csh_exit, exit, );

#define __ENV_PATH "/sbin:/bin"
const char ENV_PATH[] = __ENV_PATH;
CSH_RVAR_EXPORT(ENV_PATH, PATH, sizeof(__ENV_PATH));

#define __ENV_ZERO ""
const char ENV_ZERO[] = __ENV_ZERO;
CSH_RVAR_EXPORT(ENV_ZERO, ZERO, sizeof(__ENV_ZERO));
