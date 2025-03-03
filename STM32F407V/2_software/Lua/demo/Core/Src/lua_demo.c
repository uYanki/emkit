#include "main.h"
#include "board.h"
#include "logger.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "time.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "lua"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

static int lua_led_on(lua_State* L);
static int lua_led_off(lua_State* L);
static int lua_delay(lua_State* L);
static int lua_print(lua_State* L);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

static luaL_Reg mylib[] = {
    {"delay",   lua_delay  },
    {"led_on",  lua_led_on },
    {"led_off", lua_led_off},
    {NULL,      NULL       }
};

const char lua_test[] = {
    "off = 500\n"
    "on = 500\n"
    "while 1 do\n"
    " led_on()\n"
    " delay(on)\n"
    " led_off()\n"
    " delay(off)\n"
    " print('hello')\n"
    "end",
};

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

time_t time(time_t* time)
{
    return 0;
}

int system(const char* string)
{
    return 0;
}

void exit(int status)
{
}

static int lua_led_on(lua_State* L)
{
    HAL_GPIO_WritePin(LED1_PIN, GPIO_PIN_RESET);
    return 1;
}

static int lua_led_off(lua_State* L)
{
    HAL_GPIO_WritePin(LED1_PIN, GPIO_PIN_SET);
    return 1;
}

static int lua_delay(lua_State* L)
{
    HAL_Delay(lua_tointeger(L, 1));
    return 1;
}

void LUA_Test(void)
{
    lua_State* L;
    L = luaL_newstate();

    if (L == NULL)
    {
        LOGE("fail setup lua");
        while (1) {};
    }

    luaopen_base(L);
    luaL_setfuncs(L, mylib, 0);
    luaL_dostring(L, lua_test);
    lua_close(L);
}
