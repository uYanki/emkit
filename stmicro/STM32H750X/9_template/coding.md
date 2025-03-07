# 编码规则

* 变量命名

```c
#include <stdint.h>
#include <stdbool.h>

typedef struct {
} uart_t;

typedef struct {
} cmd_t;

typedef enum {
} tformat_cf_did_e;

typedef struct {
    uart_t* pUart;

    tformat_cf_did_e eCF;
    tformat_cf_did_e eLastCF;

    const char* szName; // 常量字符串不加指针 p 前缀
    const char* szDesc;

    char* pszString; // 非常量字符串加指针 p 前缀

    const uint8_t* cpau8Data[10];
    
    cmd_t sCmd; // 结构体以 s 开头
    cmd_t cmd_t  aCmd[10]; // 结构体有其他前缀时省略 s
    const cmd_t* cpaCmd[10];
    const cmd_t  cCmd;
    
    uint8_t  au8TxData[10];
    uint8_t  au8RxData[10];
    uint16_t u16TxSize;
    uint16_t u16RxSize;

    uint32_t u32SignleTurnValue;
    uint32_t u32MultiTurnValue;

    bool bMatched; // b 就是 is, 不要写 bIsMacthed 

    union {
        uint32_t u32All;

        struct {
            uint8_t u8ReadWrite;
            uint8_t u8Access;
        } sBit;

    } uAttr;
    
    struct {
        uint8_t u8Minutes;
        uint8_t u8Second;
    } sTime;

    void* (*pfnCbk)();
    
} tformat_t;

static const uint8_t m_cu8Number;

uint8_t g_u8Number;
```

|             |      |      |
| ----------- | ---- | ---- |
| global      | g_   |      |
| static      | s_   |      |
| member      | m_   |      |
|             |      |      |
| const       | c    |      |
| pointer     | p    |      |
| array       | a    |      |
| union       | u    |      |
| struct      | s    |      |
| enum        | e    |      |
| function    | fn   |      |
|             |      |      |
| boolean     | b    |      |
| int8_t      | s8   |      |
| int16_t     | s16  |      |
| int32_t     | s32  |      |
| int64_t     | s64  |      |
|             |      |      |
| uint8_t     | u8   |      |
| uint16_t    | u16  |      |
| uint32_t    | u32  |      |
| uint64_t    | u64  |      |
|             |      |      |
|             |      |      |
| float32     | f32  |      |
| flaot64     | f64  |      |
|             |      |      |
| const char* | sz   |      |
| char*       | psz  |      |
|             |      |      |

