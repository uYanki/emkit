#include "autoinit.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "autoinit"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static int system_begin(void)
{
    return INIT_RESULT_SUCCESS;
}

INIT_EXPORT(system_begin, "0")  // first

static int system_end(void)
{
    return INIT_RESULT_SUCCESS;
}

INIT_EXPORT(system_end, "9")  // last

void core_startup(void)
{
    core_preinit();

#if CONFIG_USDK_INIT_DEBUG

    volatile const autoinit_t* ptr;

    for (ptr = &__core_init_system_begin; ptr <= &__core_init_system_end; ptr++)
    {
        printf("%s: %d\n", ptr->name, (ptr->func)());
    }

#else

    volatile const autoinit_t* ptr;

    for (ptr = &__core_init_system_begin; ptr <= &__core_init_system_end; ptr++)
    {
        (*ptr)();
    }

#endif

    core_postinit();
}

//

#if defined(__ARMCC_VERSION)
/* re-define main function */
int $Sub$$main(void)
{
    extern int $Super$$main(void);
    core_startup();
    return $Super$$main(); /* for ARMCC. */
}
#elif defined(__ICCARM__)
/* __low_level_init will auto called by IAR cstartup */
int __low_level_init(void)
{
    extern void __iar_data_init3(void);
    // call IAR table copy function.
    __iar_data_init3();
    core_startup();
    return 0;
}
#elif defined(__GNUC__)
/* Add -eentry to arm-none-eabi-gcc argument */
int entry(void)
{
    extern int main(void);
    core_startup();
    return main();
}
#elif defined(__TI_COMPILER_VERSION__)
#pragma INIT_SECTION(core_startup)
#endif

__WEAK void core_preinit(void)
{
    // sysclk、hw uart ...
#if CONFIG_USDK_INIT_DEBUG
    while (1);
#endif
}

__WEAK void core_postinit(void)
{
}
