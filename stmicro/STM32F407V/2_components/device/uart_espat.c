#include "uart_espat.h"
#include "usart.h"

// https://blog.csdn.net/tabactivity/article/details/103341796

#define ESPAT_DISABLE_ECHO

/**
 * @brief Command Response Timeouts
 */
#define COMMAND_RESPONSE_TIMEOUT 1000
#define COMMAND_PING_TIMEOUT     3000
#define WIFI_CONNECT_TIMEOUT     30000
#define COMMAND_RESET_TIMEOUT    5000
#define CLIENT_CONNECT_TIMEOUT   5000

bool ESPAT_DataReady(uart_espat_t* pHandle)
{
    return __HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE);
}

#include <stdarg.h>

int ESPAT_Printf(uart_espat_t* pHandle, const char* szFormat, ...);

int ESPAT_Printf(uart_espat_t* pHandle, const char* szFormat, ...)
{
    static char* str[128];

    va_list ap;
    va_start(ap, szFormat);
    int n = vsnprintf(str, ARRAY_SIZE(str), szFormat, ap);
    va_end(ap);

    str[n] = '\0';
    pHandle->puts(str);

    return n;
}

char ESPAT_GetChar(uart_espat_t* pHandle)
{
    uint8_t ch;
    HAL_UART_Receive(&huart2, &ch, 1, 0xFFFF);
    HAL_UART_Transmit(&huart1, &ch, 1, 0xFFFF);
    return ch;
}

size_t ESPAT_PutChar(uart_espat_t* pHandle, uint8_t ch)
{
    HAL_UART_Transmit(&huart2, &ch, 1, 0xFFFF);
    HAL_UART_Transmit(&huart1, &ch, 1, 0xFFFF);
    return ch;
}

size_t ESPAT_Write(uart_espat_t* pHandle, uint8_t c)
{
    return ESPAT_PutChar(pHandle, c);
}

int ESPAT_Available(uart_espat_t* pHandle)
{
    return ESPAT_DataReady(pHandle);
}

int ESPAT_Read(uart_espat_t* pHandle)
{
    return ESPAT_GetChar(pHandle);
}

int ESPAT_Peek(uart_espat_t* pHandle)
{
    // return _serial->ESPAT_Peek(pHandle);
}

void ESPAT_Flush(uart_espat_t* pHandle)
{
    // _serial->ESPAT_Flush(pHandle);
}

void ESPAT_Puts(const char* s)
{
    HAL_UART_Transmit(&huart1, s, strlen(s), 0xFFFF);
    HAL_UART_Transmit(&huart2, s, strlen(s), 0xFFFF);
}

void ESPAT_Test()
{
    uart_espat_t espat = {
        .puts = ESPAT_Puts,
    };

    uint8_t IP[8];

    ESPAT_Printf(&espat, "AT+CIPCLOSE\r\n");
    ESPAT_Init(&espat);
    ESPAT_SetMode(&espat, ESPAT_MODE_STA);
    DelayBlockS(1);
    ESPAT_Connect(&espat, "uYanki", "12345678");
    DelayBlockS(5);
    ESPAT_LocalIP(&espat, IP);
    PRINTLN("ip %d.%d.%d.%d", IP[0], IP[1], IP[2], IP[3]);
    DelayBlockS(1);
    // ESPAT_TcpConnect(&espat, 0, "192.168.4.2", 10086, 0); // AP
    ESPAT_TcpConnect(&espat, 0, "192.168.43.1", 8888, 0);  // STA
    ESPAT_Printf(&espat, "AT+CIPMODE=1\r\n");
    ESPAT_Printf(&espat, "AT+CIPSEND\r\n");

    while (1)
    {
        while (ESPAT_Available(&espat))
        {
            ESPAT_GetChar(&espat);
        }

        PeriodicTask(UNIT_S, espat.puts("hello\r\n"));
    }
}

////////////////////////
// Buffer Definitions //
////////////////////////
#define ESPAT_RX_BUFFER_LEN 128  // Number of bytes in the serial receive buffer
char         au8RxBuff[ESPAT_RX_BUFFER_LEN];
unsigned int bufferHead;  // Holds position of latest byte placed in buffer.

////////////////////
// Initialization //
////////////////////

bool ESPAT_Init(uart_espat_t* pHandle)
{
    for (int i = 0; i < ESPAT_MAX_SOCK_NUM; i++)
    {
        pHandle->_eState[i] = AVAILABLE;
    }

    if (ESPAT_IsExist(pHandle))
    {
        // if (!ESPAT_SetTransferMode(uart_espat_t* pHandle,0))
        //	return false;
        if (!ESPAT_SetMux(pHandle, 0))
        {
            return false;
        }
#ifdef ESPAT_DISABLE_ECHO
        if (!ESPAT_Echo(pHandle, false))
        {
            return false;
        }
#endif
        return true;
    }

    return false;
}

///////////////////////
// Basic AT Commands //
///////////////////////

bool ESPAT_IsExist(uart_espat_t* pHandle)
{
    ESPAT_ExecCommand(pHandle, ESPAT_CMD_TEST);  // Send AT

    if (ESPAT_ReadForResponse(pHandle, ESPAT_RSP_OK, COMMAND_RESPONSE_TIMEOUT) > 0)
    {
        return true;
    }

    return false;
}

bool ESPAT_Reset(uart_espat_t* pHandle)
{
    ESPAT_ExecCommand(pHandle, ESPAT_CMD_RESET);  // Send AT+RST

    if (ESPAT_ReadForResponse(pHandle, ESPAT_RSP_READY, COMMAND_RESET_TIMEOUT) > 0)
    {
        return true;
    }

    return false;
}

bool ESPAT_Echo(uart_espat_t* pHandle, bool enable)
{
    if (enable)
    {
        ESPAT_ExecCommand(pHandle, ESPAT_CMD_ECHO_ENABLE);
    }
    else
    {
        ESPAT_ExecCommand(pHandle, ESPAT_CMD_ECHO_DISABLE);
    }

    if (ESPAT_ReadForResponse(pHandle, ESPAT_RSP_OK, COMMAND_RESPONSE_TIMEOUT) > 0)
    {
        return true;
    }

    return false;
}

bool ESPAT_SetBaud(uart_espat_t* pHandle, unsigned long baud)
{
    char parameters[16];
    memset(parameters, 0, 16);
    // Constrain parameters:
    baud = CLAMP(baud, 110, 115200);

    // Put parameters into string
    sprintf(parameters, "%d,8,1,0,0", baud);

    // Send AT+UART=baud,databits,stopbits,parity,flowcontrol
    ESPAT_SendCommand(pHandle, ESPAT_CMD_UART, ESPAT_CMD_SETUP, parameters);

    if (ESPAT_ReadForResponse(pHandle, ESPAT_RSP_OK, COMMAND_RESPONSE_TIMEOUT) > 0)
    {
        return true;
    }

    return false;
}

int16_t ESPAT_GetVersion(uart_espat_t* pHandle, char* ATversion, char* SDKversion, char* compileTime)
{
    ESPAT_ExecCommand(pHandle, ESPAT_CMD_VERSION);  // Send AT+GMR
    // Example Response: AT version:0.30.0.0(Jul  3 2015 19:35:49)\r\n (43 chars)
    //                   SDK version:1.2.0\r\n (19 chars)
    //                   compile time:Jul  7 2015 18:34:26\r\n (36 chars)
    //                   OK\r\n
    // (~101 characters)
    // Look for "OK":
    int16_t rsp = (ESPAT_ReadForResponse(pHandle, ESPAT_RSP_OK, COMMAND_RESPONSE_TIMEOUT) > 0);
    if (rsp > 0)
    {
        char *p, *q;
        // Look for "AT version" in the rxBuffer
        p = strstr(pHandle->au8RxBuff, "AT version:");
        if (p == nullptr)
        {
            return ERR_BAD_MESSAGE;
        }
        p += strlen("AT version:");
        q = strchr(p, '\r');  // Look for \r
        if (q == nullptr)
        {
            return ERR_BAD_MESSAGE;
        }
        sprintf(ATversion, p, q - p);

        // Look for "SDK version:" in the rxBuffer
        p = strstr(pHandle->au8RxBuff, "SDK version:");
        if (p == nullptr)
        {
            return ERR_BAD_MESSAGE;
        }
        p += strlen("SDK version:");
        q = strchr(p, '\r');  // Look for \r
        if (q == nullptr)
        {
            return ERR_BAD_MESSAGE;
        }
        sprintf(SDKversion, p, q - p);

        // Look for "compile time:" in the rxBuffer
        p = strstr(pHandle->au8RxBuff, "compile time:");
        if (p == nullptr)
        {
            return ERR_BAD_MESSAGE;
        }
        p += strlen("compile time:");
        q = strchr(p, '\r');  // Look for \r
        if (q == nullptr)
        {
            return ERR_BAD_MESSAGE;
        }
        sprintf(compileTime, p, q - p);
    }

    return rsp;
}

////////////////////
// WiFi Functions //
////////////////////

// ESPAT_GetMode(uart_espat_t* pHandle
// Input: None
// Output:
//    - Success: 1, 2, 3 (ESPAT_MODE_STA, ESPAT_MODE_AP, ESPAT_MODE_STAAP)
//    - Fail: <0 (esp8266_cmd_rsp)
int16_t ESPAT_GetMode(uart_espat_t* pHandle)
{
    ESPAT_SendCommand(pHandle, ESPAT_CMD_WIFI_MODE, ESPAT_CMD_QUERY, nullptr);

    // Example response: \r\nAT+CWMODE_CUR?\r+CWMODE_CUR:2\r\n\r\nOK\r\n
    // Look for the OK:
    int16_t rsp = ESPAT_ReadForResponse(pHandle, ESPAT_RSP_OK, COMMAND_RESPONSE_TIMEOUT);
    if (rsp > 0)
    {
        // Then get the number after ':':
        char* p = strchr(pHandle->au8RxBuff, ':');
        if (p != nullptr)
        {
            char mode = *(p + 1);
            if ((mode >= '1') && (mode <= '3'))
            {
                return (mode - 48);  // Convert ASCII to decimal
            }
        }

        return ERR_BAD_MESSAGE;
    }

    return rsp;
}

// ESPAT_SetMode(uart_espat_t* pHandle
// Input: 1, 2, 3 (ESPAT_MODE_STA, ESPAT_MODE_AP, ESPAT_MODE_STAAP)
// Output:
//    - Success: >0
//    - Fail: <0 (esp8266_cmd_rsp)
int16_t ESPAT_SetMode(uart_espat_t* pHandle, espat_wifi_mode_e mode)
{
    char modeChar[2] = {0, 0};
    sprintf(modeChar, "%d", mode);
    ESPAT_SendCommand(pHandle, ESPAT_CMD_WIFI_MODE, ESPAT_CMD_SETUP, modeChar);

    return ESPAT_ReadForResponse(pHandle, ESPAT_RSP_OK, COMMAND_RESPONSE_TIMEOUT);
}

// ESPAT_Connect(uart_espat_t* pHandle
// Input: ssid and pwd const char's
// Output:
//    - Success: >0
//    - Fail: <0 (esp8266_cmd_rsp)
int16_t ESPAT_Connect(uart_espat_t* pHandle, const char* ssid, const char* pwd)
{
    ESPAT_Printf(pHandle, "AT%s=\"%s\"", ESPAT_CMD_CONNECT_AP, ssid);

    if (pwd != nullptr)
    {
        ESPAT_Printf(pHandle, ",\"%s\"", pwd);
    }

    pHandle->puts("\r\n");

    return ESPAT_ReadForResponses(pHandle, ESPAT_RSP_OK, ESPAT_RSP_FAIL, WIFI_CONNECT_TIMEOUT);
}

int16_t ESPAT_GetAP(uart_espat_t* pHandle, char* ssid)
{
    ESPAT_SendCommand(pHandle, ESPAT_CMD_CONNECT_AP, ESPAT_CMD_QUERY, nullptr);  // Send "AT+CWJAP?"

    int16_t rsp = ESPAT_ReadForResponse(pHandle, ESPAT_RSP_OK, COMMAND_RESPONSE_TIMEOUT);
    // Example Responses: No AP\r\n\r\nOK\r\n
    // - or -
    // +CWJAP:"WiFiSSID","00:aa:bb:cc:dd:ee",6,-45\r\n\r\nOK\r\n
    if (rsp > 0)
    {
        // Look for "No AP"
        if (strstr(pHandle->au8RxBuff, "No AP") != nullptr)
        {
            return 0;
        }

        // Look for "+CWJAP"
        char* p = strstr(pHandle->au8RxBuff, ESPAT_CMD_CONNECT_AP);
        if (p != nullptr)
        {
            p += strlen(ESPAT_CMD_CONNECT_AP) + 2;
            char* q = strchr(p, '"');
            if (q == nullptr)
            {
                return ERR_BAD_MESSAGE;
            }
            sprintf(ssid, p, q - p);  // Copy string to temp char array:
            return 1;
        }
    }

    return rsp;
}

int16_t ESPAT_Disconnect(uart_espat_t* pHandle)
{
    ESPAT_ExecCommand(pHandle, ESPAT_CMD_DISCONNECT);  // Send AT+CWQAP
    // Example response: \r\n\r\nOK\r\nWIFI DISCONNECT\r\n
    // "WIFI DISCONNECT" comes up to 500ms _after_ OK.
    int16_t rsp = ESPAT_ReadForResponse(pHandle, ESPAT_RSP_OK, COMMAND_RESPONSE_TIMEOUT);
    if (rsp > 0)
    {
        rsp = ESPAT_ReadForResponse(pHandle, "WIFI DISCONNECT", COMMAND_RESPONSE_TIMEOUT);
        if (rsp > 0)
        {
            return rsp;
        }
        return 1;
    }

    return rsp;
}

// ESPAT_Status(uart_espat_t* pHandle
// Input: none
// Output:
//    - Success: 2, 3, 4, or 5 (ESPAT_STATUS_GOTIP, ESPAT_STATUS_CONNECTED, ESPAT_STATUS_DISCONNECTED, ESPAT_STATUS_NOWIFI)
//    - Fail: <0 (esp8266_cmd_rsp)
int16_t ESPAT_Status(uart_espat_t* pHandle)
{
    int16_t statusRet = ESPAT_UpdateStatus(pHandle);
    if (statusRet > 0)
    {
        switch (pHandle->_eStatus.stat)
        {
            case ESPAT_STATUS_GOTIP:         // 3
            case ESPAT_STATUS_DISCONNECTED:  // 4 - "Client" disconnected, not wifi
                return 1;
                break;
            case ESPAT_STATUS_CONNECTED:  // Connected, but haven't gotten an IP
            case ESPAT_STATUS_NOWIFI:     // No WiFi configured
                return 0;
                break;
        }
    }
    return statusRet;
}

int16_t ESPAT_UpdateStatus(uart_espat_t* pHandle)
{
    ESPAT_ExecCommand(pHandle, ESPAT_CMD_TCP_STATUS);  // Send AT+CIPSTATUS\r\n
    // Example response: (connected as client)
    // STATUS:3\r\n
    // +CIPSTATUS:0,"TCP","93.184.216.34",80,0\r\n\r\nOK\r\n
    // - or - (clients connected to ESP8266 server)
    // STATUS:3\r\n
    // +CIPSTATUS:0,"TCP","192.168.0.100",54723,1\r\n
    // +CIPSTATUS:1,"TCP","192.168.0.101",54724,1\r\n\r\nOK\r\n
    int16_t rsp = ESPAT_ReadForResponse(pHandle, ESPAT_RSP_OK, COMMAND_RESPONSE_TIMEOUT);
    if (rsp > 0)
    {
        char* p = ESPAT_SearchBuffer(pHandle, "STATUS:");
        if (p == nullptr)
        {
            return ERR_BAD_MESSAGE;
        }

        p += strlen("STATUS:");
        pHandle->_eStatus.stat = *p - 48;

        for (int i = 0; i < ESPAT_MAX_SOCK_NUM; i++)
        {
            p = strstr(p, "+CIPSTATUS:");
            if (p == nullptr)
            {
                // Didn't find any IPSTATUS'. Set linkID to 255.
                for (int j = i; j < ESPAT_MAX_SOCK_NUM; j++)
                {
                    pHandle->_eStatus.ipstatus[j].linkID = 255;
                }
                return rsp;
            }
            else
            {
                p += strlen("+CIPSTATUS:");
                // Find linkID:
                uint8_t linkId = *p - 48;
                if (linkId >= ESPAT_MAX_SOCK_NUM)
                {
                    return rsp;
                }
                pHandle->_eStatus.ipstatus[linkId].linkID = linkId;

                // Find type (p pointing at linkID):
                p += 3;  // Move p to either "T" or "U"
                if (*p == 'T')
                {
                    pHandle->_eStatus.ipstatus[linkId].type = ESPAT_TCP;
                }
                else if (*p == 'U')
                {
                    pHandle->_eStatus.ipstatus[linkId].type = ESPAT_UDP;
                }
                else
                {
                    pHandle->_eStatus.ipstatus[linkId].type = ESPAT_TYPE_UNDEFINED;
                }

                // Find remoteIP (p pointing at first letter or type):
                p += 6;  // Move p to first digit of first octet.
                for (uint8_t j = 0; j < 4; j++)
                {
                    char tempOctet[4];
                    memset(tempOctet, 0, 4);  // Clear tempOctet

                    size_t octetLength = strspn(p, "0123456789");  // Find length of numerical string:

                    sprintf(tempOctet, p, octetLength);                                // Copy string to temp char array:
                    pHandle->_eStatus.ipstatus[linkId].remoteIP[j] = atoi(tempOctet);  // Move the temp char into IP Address octet

                    p += (octetLength + 1);  // Increment p to next octet
                }

                // Find port (p pointing at ',' between IP and port:
                p += 1;  // Move p to first digit of port
                char tempPort[6];
                memset(tempPort, 0, 6);
                size_t portLen = strspn(p, "0123456789");  // Find length of numerical string:
                sprintf(tempPort, p, portLen);
                pHandle->_eStatus.ipstatus[linkId].port = atoi(tempPort);
                p += portLen + 1;

                // Find tetype (p pointing at tetype)
                if (*p == '0')
                {
                    pHandle->_eStatus.ipstatus[linkId].tetype = ESPAT_CLIENT;
                }
                else if (*p == '1')
                {
                    pHandle->_eStatus.ipstatus[linkId].tetype = ESPAT_SERVER;
                }
            }
        }
    }

    return rsp;
}

// ESPAT_LocalIP
// Input: none
// Output:
//    - Success: Device's local IPAddress
//    - Fail: 0
status_t ESPAT_LocalIP(uart_espat_t* pHandle, uint8_t returnIP[4])
{
    ESPAT_ExecCommand(pHandle, ESPAT_CMD_GET_LOCAL_IP);  // Send AT+CIFSR\r\n
    // Example Response: +CIFSR:STAIP,"192.168.0.114"\r\n
    //                   +CIFSR:STAMAC,"18:fe:34:9d:b7:d9"\r\n
    //                   \r\n
    //                   OK\r\n
    // Look for the OK:
    int16_t rsp = ESPAT_ReadForResponse(pHandle, ESPAT_RSP_OK, COMMAND_RESPONSE_TIMEOUT);

    // if (rsp > 0)
    // {
    //     // Look for "STAIP" in the rxBuffer
    //     char* p = strstr(pHandle->au8RxBuff, "STAIP");
    //     if (p != nullptr)
    //     {
    //         p += 7;  // Move p seven places. (skip STAIP,")
    //         for (uint8_t i = 0; i < 4; i++)
    //         {
    //             char tempOctet[4];
    //             memset(tempOctet, 0, 4);  // Clear tempOctet

    //             size_t octetLength = strspn(p, "0123456789");  // Find length of numerical string:
    //             if (octetLength >= 4)                          // If it's too big, return an error
    //             {
    //                 return ERR_BAD_MESSAGE;
    //             }

    //             sprintf(tempOctet, p, octetLength);  // Copy string to temp char array:
    //             returnIP[i] = atoi(tempOctet);       // Move the temp char into IP Address octet

    //             p += (octetLength + 1);  // Increment p to next octet
    //         }

    //         return ERR_NONE;
    //     }
    // }

    return rsp;
}

int16_t ESPAT_LocalMAC(uart_espat_t* pHandle, char* mac)
{
    ESPAT_SendCommand(pHandle, ESPAT_CMD_GET_STA_MAC, ESPAT_CMD_QUERY, nullptr);  // Send "AT+CIPSTAMAC?"

    int16_t rsp = ESPAT_ReadForResponse(pHandle, ESPAT_RSP_OK, COMMAND_RESPONSE_TIMEOUT);

    if (rsp > 0)
    {
        // Look for "+CIPSTAMAC"
        char* p = strstr(pHandle->au8RxBuff, ESPAT_CMD_GET_STA_MAC);
        if (p != nullptr)
        {
            p += strlen(ESPAT_CMD_GET_STA_MAC) + 2;
            char* q = strchr(p, '"');
            if (q == nullptr)
            {
                return ERR_BAD_MESSAGE;
            }
            sprintf(mac, p, q - p);  // Copy string to temp char array:
            return 1;
        }
    }

    return rsp;
}

/////////////////////
// TCP/IP Commands //
/////////////////////

int16_t ESPAT_TcpConnect(uart_espat_t* pHandle, uint8_t linkID, const char* destination, uint16_t port, uint16_t keepAlive)
{
    ESPAT_Printf(pHandle, "AT%s=\"TCP\",\"%s\",%d", ESPAT_CMD_TCP_CONNECT, destination, port);
    // ESPAT_Printf(pHandle, "AT%s=%d,\"TCP\",\"%s\",%d", ESPAT_CMD_TCP_CONNECT, linkID, destination, port);

    if (keepAlive > 0)
    {
        // keepAlive is in units of 500 milliseconds.
        // Max is 7200 * 500 = 3600000 ms = 60 minutes.
        ESPAT_Printf(pHandle, ",%d", keepAlive / 500);
    }

    pHandle->puts("\r\n");

    // Example good: CONNECT\r\n\r\nOK\r\n
    // Example bad: DNS Fail\r\n\r\nERROR\r\n
    // Example meh: ALREADY CONNECTED\r\n\r\nERROR\r\n
    int16_t rsp = ESPAT_ReadForResponses(pHandle, ESPAT_RSP_OK, ESPAT_RSP_ERROR, CLIENT_CONNECT_TIMEOUT);

    if (rsp < 0)
    {
        // We may see "ERROR", but be "ALREADY CONNECTED".
        // Search for "ALREADY", and return success if we see it.
        char* p = ESPAT_SearchBuffer(pHandle, "ALREADY");
        if (p != nullptr)
        {
            return 2;
        }
        // Otherwise the connection failed. Return the error code:
        return rsp;
    }
    // Return 1 on successful (new) connection
    return 1;
}

int16_t ESPAT_TcpSend(uart_espat_t* pHandle, uint8_t linkID, const uint8_t* buf, size_t size)
{
    if (size > 2048)
    {
        return ERR_INVALID_VALUE;
    }

    char params[8];
    sprintf(params, "%d,%d", linkID, size);
    ESPAT_SendCommand(pHandle, ESPAT_CMD_TCP_SEND, ESPAT_CMD_SETUP, params);

    int16_t rsp = ESPAT_ReadForResponses(pHandle, ESPAT_RSP_OK, ESPAT_RSP_ERROR, COMMAND_RESPONSE_TIMEOUT);
    // if (rsp > 0)
    if (rsp != ESPAT_RSP_FAIL)
    {
        pHandle->puts((const char*)buf);

        rsp = ESPAT_ReadForResponse(pHandle, "SEND OK", COMMAND_RESPONSE_TIMEOUT);

        if (rsp > 0)
        {
            return size;
        }
    }

    return rsp;
}

int16_t ESPAT_Close(uart_espat_t* pHandle, uint8_t linkID)
{
    char params[2];
    sprintf(params, "%d", linkID);
    ESPAT_SendCommand(pHandle, ESPAT_CMD_TCP_CLOSE, ESPAT_CMD_SETUP, params);

    // Eh, client virtual function doesn't have a return value.
    // We'll wait for the OK or timeout anyway.
    return ESPAT_ReadForResponse(pHandle, ESPAT_RSP_OK, COMMAND_RESPONSE_TIMEOUT);
}

int16_t ESPAT_SetTransferMode(uart_espat_t* pHandle, uint8_t mode)
{
    char params[2] = {0, 0};
    params[0]      = (mode > 0) ? '1' : '0';
    ESPAT_SendCommand(pHandle, ESPAT_CMD_TRANSMISSION_MODE, ESPAT_CMD_SETUP, params);

    return ESPAT_ReadForResponse(pHandle, ESPAT_RSP_OK, COMMAND_RESPONSE_TIMEOUT);
}

int16_t ESPAT_SetMux(uart_espat_t* pHandle, uint8_t mux)
{
    char params[2] = {0, 0};
    params[0]      = (mux > 0) ? '1' : '0';  // 0=单连接
    ESPAT_SendCommand(pHandle, ESPAT_CMD_TCP_MULTIPLE, ESPAT_CMD_SETUP, params);

    return ESPAT_ReadForResponse(pHandle, ESPAT_RSP_OK, COMMAND_RESPONSE_TIMEOUT);
}

int16_t ESPAT_ConfigureTCPServer(uart_espat_t* pHandle, uint16_t port, uint8_t create)
{
    char params[10];
    if (create > 1)
    {
        create = 1;
    }
    sprintf(params, "%d,%d", create, port);
    ESPAT_SendCommand(pHandle, ESPAT_CMD_SERVER_CONFIG, ESPAT_CMD_SETUP, params);

    return ESPAT_ReadForResponse(pHandle, ESPAT_RSP_OK, COMMAND_RESPONSE_TIMEOUT);
}

/**
 * @param server "%d.%d.%d.%d"
 */
int16_t ESPAT_Ping(uart_espat_t* pHandle, const char* server)
{
    char params[strlen(server) + 3];
    sprintf(params, "\"%s\"", server);
    // Send AT+Ping=<server>
    ESPAT_SendCommand(pHandle, ESPAT_CMD_PING, ESPAT_CMD_SETUP, params);
    // Example responses:
    //  * Good response: +12\r\n\r\nOK\r\n
    //  * Timeout response: +timeout\r\n\r\nERROR\r\n
    //  * Error response (unreachable): ERROR\r\n\r\n
    int16_t rsp = ESPAT_ReadForResponses(pHandle, ESPAT_RSP_OK, ESPAT_RSP_ERROR, COMMAND_PING_TIMEOUT);
    if (rsp > 0)
    {
        char* p = ESPAT_SearchBuffer(pHandle, "+");
        p += 1;                     // Move p forward 1 space
        char* q = strchr(p, '\r');  // Find the first \r
        if (q == nullptr)
        {
            return ERR_BAD_MESSAGE;
        }
        char tempRsp[10];
        sprintf(tempRsp, p, q - p);
        return atoi(tempRsp);
    }
    else
    {
        if (ESPAT_SearchBuffer(pHandle, "timeout") != nullptr)
        {
            return 0;
        }
    }

    return rsp;
}

int16_t ESPAT_PinMode(uart_espat_t* pHandle, uint8_t pin, pin_mode_e mode, pin_pull_e pull)
{
    char params[5];

    char modeC = 'i';  // Default mode to input

    if (mode == PIN_MODE_OUTPUT_PUSH_PULL)
    {
        modeC = 'o';  // o = OUTPUT
    }
    else if (mode == PIN_MODE_INPUT_FLOATING && pull == PIN_PULL_UP)
    {
        modeC = 'p';  // p = INPUT_PULLUP
    }

    sprintf(params, "%d,%c", pin, modeC);
    ESPAT_SendCommand(pHandle, ESPAT_CMD_PINMODE, ESPAT_CMD_SETUP, params);

    return ESPAT_ReadForResponses(pHandle, ESPAT_RSP_OK, ESPAT_RSP_ERROR, COMMAND_RESPONSE_TIMEOUT);
}

int16_t ESPAT_DigitalWrite(uart_espat_t* pHandle, uint8_t pin, uint8_t state)
{
    char params[5];

    char stateC = 'l';  // Default state to LOW
    if (state == PIN_LEVEL_HIGH)
    {
        stateC = 'h';  // h = HIGH
    }

    sprintf(params, "%d,%c", pin, stateC);
    ESPAT_SendCommand(pHandle, ESPAT_CMD_PINWRITE, ESPAT_CMD_SETUP, params);

    return ESPAT_ReadForResponses(pHandle, ESPAT_RSP_OK, ESPAT_RSP_ERROR, COMMAND_RESPONSE_TIMEOUT);
}

int8_t ESPAT_DigitalRead(uart_espat_t* pHandle, uint8_t pin)
{
    char params[3];

    sprintf(params, "%d", pin);
    ESPAT_SendCommand(pHandle, ESPAT_CMD_PINREAD, ESPAT_CMD_SETUP, params);  // Send AT+PINREAD=n\r\n
    // Example response: 1\r\n\r\nOK\r\n
    if (ESPAT_ReadForResponses(pHandle, ESPAT_RSP_OK, ESPAT_RSP_ERROR, COMMAND_RESPONSE_TIMEOUT) > 0)
    {
        if (strchr(pHandle->au8RxBuff, '0') != nullptr)
        {
            return PIN_LEVEL_LOW;
        }
        else if (strchr(pHandle->au8RxBuff, '1') != nullptr)
        {
            return PIN_LEVEL_HIGH;
        }
    }

    return -1;
}

void ESPAT_ExecCommand(uart_espat_t* pHandle, const char* cmd)
{
    ESPAT_SendCommand(pHandle, cmd, ESPAT_CMD_EXECUTE, nullptr);
}

void ESPAT_SendCommand(uart_espat_t* pHandle, const char* cmd, espat_command_type_e type, const char* params)
{
    switch (type)
    {
        case ESPAT_CMD_EXECUTE: ESPAT_Printf(pHandle, "AT%s", cmd); break;
        case ESPAT_CMD_QUERY: ESPAT_Printf(pHandle, "AT%s?", cmd); break;
        case ESPAT_CMD_SETUP: ESPAT_Printf(pHandle, "AT%s=%s", cmd, params); break;
    }

    ESPAT_Printf(pHandle, "\r\n");
}

int16_t ESPAT_ReadForResponse(uart_espat_t* pHandle, const char* rsp, unsigned int timeout)
{
    unsigned long timeIn   = GetTickMs();  // Timestamp coming into function
    unsigned int  received = 0;            // received keeps track of number of chars read

    ESPAT_ClearBuffer(pHandle);             // Clear the class receive buffer (pHandle->au8RxBuff)
    while (timeIn + timeout > GetTickMs())  // While we haven't timed out
    {
        if (ESPAT_DataReady(pHandle))  // If data is available on UART RX
        {
            received += ESPAT_ReadByteToBuffer(pHandle);
            if (ESPAT_SearchBuffer(pHandle, rsp))  // Search the buffer for goodRsp
            {
                return received;  // Return how number of chars read
            }
        }
    }

    if (received > 0)  // If we received any characters
    {
        return ERR_BAD_MESSAGE;  // Return unkown response error code
    }
    else  // If we haven't received any characters
    {
        return ERR_TIMEOUT;  // Return the timeout error code
    }
}

int16_t ESPAT_ReadForResponses(uart_espat_t* pHandle, const char* pass, const char* fail, unsigned int timeout)
{
    unsigned long timeIn   = GetTickMs();  // Timestamp coming into function
    unsigned int  received = 0;            // received keeps track of number of chars read

    ESPAT_ClearBuffer(pHandle);  // Clear the class receive buffer (pHandle->au8RxBuff)

    while (timeIn + timeout > GetTickMs())  // While we haven't timed out
    {
        if (ESPAT_DataReady(pHandle))  // If data is available on UART RX
        {
            received += ESPAT_ReadByteToBuffer(pHandle);

            if (ESPAT_SearchBuffer(pHandle, pass))  // Search the buffer for goodRsp
            {
                return received;  // Return how number of chars read
            }
            if (ESPAT_SearchBuffer(pHandle, fail))
            {
                return ERR_NOT_FOUND;
            }
        }
    }

    if (received > 0)  // If we received any characters
    {
        return ERR_BAD_MESSAGE;  // Return unkown response error code
    }
    else  // If we haven't received any characters
    {
        return ERR_TIMEOUT;  // Return the timeout error code
    }
}

void ESPAT_ClearBuffer(uart_espat_t* pHandle)
{
    memset(pHandle->au8RxBuff, '\0', ESPAT_RX_BUFFER_LEN);
    bufferHead = 0;
}

unsigned int ESPAT_ReadByteToBuffer(uart_espat_t* pHandle)
{
    // Read the data in
    char c = ESPAT_GetChar(pHandle);

    // Store the data in the buffer
    pHandle->au8RxBuff[bufferHead] = c;

    //! TODO: Don't care if we overflow. Should we? Set a flag or something?
    bufferHead = (bufferHead + 1) % ESPAT_RX_BUFFER_LEN;

    return 1;
}

char* ESPAT_SearchBuffer(uart_espat_t* pHandle, const char* test)
{
    int bufferLen = strlen((const char*)pHandle->au8RxBuff);
    // If our buffer isn't full, just do an strstr
    if (bufferLen < ESPAT_RX_BUFFER_LEN)
    {
        return strstr((const char*)pHandle->au8RxBuff, test);
    }
    else
    {  //! TODO
        // If the buffer is full, we need to search from the end of the
        // buffer back to the beginning.
        int testLen = strlen(test);
        for (int i = 0; i < ESPAT_RX_BUFFER_LEN; i++)
        {
        }
    }
}
