#ifndef __UART_ESPAT_H__
#define __UART_ESPAT_H__

#include "typebasic.h"
#include "pindefs.h"

/**
 * @brief
 * @ref http://github.com/sparkfun/SparkFun_ESP8266_AT_Arduino_Library
 */

/**
 * @brief Common Responses
 */
#define ESPAT_RSP_OK    "OK\r\n"
#define ESPAT_RSP_ERROR "ERROR\r\n"
#define ESPAT_RSP_FAIL  "FAIL"
#define ESPAT_RSP_READY "READY!"

/**
 * @brief Basic AT Commands
 */
#define ESPAT_CMD_TEST         ""          // Test AT startup
#define ESPAT_CMD_RESET        "+RST"      // Restart module
#define ESPAT_CMD_VERSION      "+GMR"      // View version info
#define ESPAT_CMD_SLEEP        "+GSLP"     // Enter deep-sleep mode
#define ESPAT_CMD_ECHO_ENABLE  "E1"        // AT commands echo
#define ESPAT_CMD_ECHO_DISABLE "E0"        // AT commands echo
#define ESPAT_CMD_RESTORE      "+RESTORE"  // Factory reset
#define ESPAT_CMD_UART         "+UART"     // UART configuration

/**
 * @brief WiFi Functions
 */
#define ESPAT_CMD_WIFI_MODE    "+CWMODE"      // WiFi mode (sta/AP/sta+AP)
#define ESPAT_CMD_CONNECT_AP   "+CWJAP"       // Connect to AP
#define ESPAT_CMD_LIST_AP      "+CWLAP"       // List available AP's
#define ESPAT_CMD_DISCONNECT   "+CWQAP"       // Disconnect from AP
#define ESPAT_CMD_AP_CONFIG    "+CWSAP"       // Set softAP configuration
#define ESPAT_CMD_STATION_IP   "+CWLIF"       // List station IP's connected to softAP
#define ESPAT_CMD_DHCP_EN      "+CWDHCP"      // Enable/disable DHCP
#define ESPAT_CMD_AUTO_CONNECT "+CWAUTOCONN"  // Connect to AP automatically
#define ESPAT_CMD_SET_STA_MAC  "+CIPSTAMAC"   // Set MAC address of station
#define ESPAT_CMD_GET_STA_MAC  "+CIPSTAMAC"   // Get MAC address of station
#define ESPAT_CMD_SET_AP_MAC   "+CIPAPMAC"    // Set MAC address of softAP
#define ESPAT_CMD_SET_STA_IP   "+CIPSTA"      // Set IP address of ESP8266 station
#define ESPAT_CMD_SET_AP_IP    "+CIPAP"       // Set IP address of ESP8266 softAP

/**
 * @brief TCP/IP Commands
 */
#define ESPAT_CMD_TCP_STATUS         "+CIPSTATUS"  // Get connection status
#define ESPAT_CMD_TCP_CONNECT        "+CIPSTART"   // Establish TCP connection or register UDP port
#define ESPAT_CMD_TCP_SEND           "+CIPSEND"    // Send Data
#define ESPAT_CMD_TCP_CLOSE          "+CIPCLOSE"   // Close TCP/UDP connection
#define ESPAT_CMD_GET_LOCAL_IP       "+CIFSR"      // Get local IP address
#define ESPAT_CMD_TCP_MULTIPLE       "+CIPMUX"     // Set multiple connections mode
#define ESPAT_CMD_SERVER_CONFIG      "+CIPSERVER"  // Configure as server
#define ESPAT_CMD_TRANSMISSION_MODE  "+CIPMODE"    // Set transmission mode
#define ESPAT_CMD_SET_SERVER_TIMEOUT "+CIPSTO"     // Set timeout when ESP8266 runs as TCP server
#define ESPAT_CMD_PING               "+PING"       // Function PING

/**
 * @brief Custom GPIO Commands
 */
#define ESPAT_CMD_PINMODE  "+PINMODE"   // Set GPIO mode (input/output)
#define ESPAT_CMD_PINWRITE "+PINWRITE"  // Write GPIO (high/low)
#define ESPAT_CMD_PINREAD  "+PINREAD"   // Read GPIO digital value

typedef enum {
    ESPAT_MODE_STA   = 1,
    ESPAT_MODE_AP    = 2,
    ESPAT_MODE_STAAP = 3,
} espat_wifi_mode_e;

typedef enum {
    ESPAT_CMD_QUERY,
    ESPAT_CMD_SETUP,
    ESPAT_CMD_EXECUTE,
} espat_command_type_e;

typedef enum {
    ESPAT_ECN_OPEN,
    ESPAT_ECN_WPA_PSK,
    ESPAT_ECN_WPA2_PSK,
    ESPAT_ECN_WPA_WPA2_PSK,
} espat_encryption_e;

typedef enum {
    ESPAT_STATUS_GOTIP        = 2,
    ESPAT_STATUS_CONNECTED    = 3,
    ESPAT_STATUS_DISCONNECTED = 4,
    ESPAT_STATUS_NOWIFI       = 5,
} espat_connect_status_e;

typedef enum {
    ESPAT_SOFTWARE_SERIAL,
    ESPAT_HARDWARE_SERIAL,
} espat_serial_port_e;

typedef enum {
    AVAILABLE = 0,
    TAKEN     = 1,
} espat_socket_state_e;

typedef enum {
    ESPAT_TCP,
    ESPAT_UDP,
    ESPAT_TYPE_UNDEFINED,
} espat_connection_type_e;

typedef enum {
    ESPAT_CLIENT,
    ESPAT_SERVER,
} espat_tetype_e;

//

#define ESPAT_MAX_SOCK_NUM   5
#define ESPAT_SOCK_NOT_AVAIL 255

#include "uart_espat.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t                 linkID;
    espat_connection_type_e type;
    uint8_t                 remoteIP[4];
    uint16_t                port;
    espat_tetype_e          tetype;
} espat_ipstatus_t;

typedef struct {
    espat_connect_status_e stat;
    espat_ipstatus_t       ipstatus[ESPAT_MAX_SOCK_NUM];
} espat_status_t;

typedef struct {
    int16_t        _eState[ESPAT_MAX_SOCK_NUM];
    espat_status_t _eStatus;
    void (*puts)(const char*);
    uint8_t au8RxBuff[128];
} uart_espat_t;

bool ESPAT_Init(uart_espat_t* pHandle);

///////////////////////
// Basic AT Commands //
///////////////////////
bool    ESPAT_IsExist(uart_espat_t* pHandle);
bool    ESPAT_Reset(uart_espat_t* pHandle);
int16_t ESPAT_GetVersion(uart_espat_t* pHandle, char* ATversion, char* SDKversion, char* compileTime);
bool    ESPAT_Echo(uart_espat_t* pHandle, bool enable);
bool    ESPAT_SetBaud(uart_espat_t* pHandle, unsigned long baud);

////////////////////
// WiFi Functions //
////////////////////
int16_t  ESPAT_GetMode(uart_espat_t* pHandle);
int16_t  ESPAT_SetMode(uart_espat_t* pHandle, espat_wifi_mode_e mode);
int16_t  ESPAT_Connect(uart_espat_t* pHandle, const char* ssid, const char* pwd);
int16_t  ESPAT_GetAP(uart_espat_t* pHandle, char* ssid);
int16_t  ESPAT_LocalMAC(uart_espat_t* pHandle, char* mac);
int16_t  ESPAT_Disconnect(uart_espat_t* pHandle);
status_t ESPAT_LocalIP(uart_espat_t* pHandle, uint8_t returnIP[4]);
/////////////////////
// TCP/IP Commands //
/////////////////////
int16_t  ESPAT_Status(uart_espat_t* pHandle);
int16_t  ESPAT_UpdateStatus(uart_espat_t* pHandle);
int16_t  ESPAT_TcpConnect(uart_espat_t* pHandle, uint8_t linkID, const char* destination, uint16_t port, uint16_t keepAlive);
int16_t  ESPAT_TcpSend(uart_espat_t* pHandle, uint8_t linkID, const uint8_t* buf, size_t size);
int16_t  ESPAT_Close(uart_espat_t* pHandle, uint8_t linkID);
int16_t  ESPAT_SetTransferMode(uart_espat_t* pHandle, uint8_t mode);
int16_t  ESPAT_SetMux(uart_espat_t* pHandle, uint8_t mux);
int16_t  ESPAT_ConfigureTCPServer(uart_espat_t* pHandle, uint16_t port, uint8_t create);
int16_t  ESPAT_Ping(uart_espat_t* pHandle, const char* server);

//////////////////////////
// Custom GPIO Commands //
//////////////////////////
int16_t ESPAT_PinMode(uart_espat_t* pHandle, uint8_t pin, pin_mode_e mode, pin_pull_e pull);
int16_t ESPAT_DigitalWrite(uart_espat_t* pHandle, uint8_t pin, uint8_t state);
int8_t  ESPAT_DigitalRead(uart_espat_t* pHandle, uint8_t pin);

///////////////////////////////////
// Virtual Functions from Stream //
///////////////////////////////////
size_t ESPAT_Write(uart_espat_t* pHandle, uint8_t);
int    ESPAT_Available(uart_espat_t* pHandle);
int    ESPAT_Read(uart_espat_t* pHandle);
int    ESPAT_Peek(uart_espat_t* pHandle);
void   ESPAT_Flush(uart_espat_t* pHandle);

// private:
//////////////////////////
// Command Send/Receive //
//////////////////////////

void    ESPAT_ExecCommand(uart_espat_t* pHandle, const char* cmd);
void    ESPAT_SendCommand(uart_espat_t* pHandle, const char* cmd, espat_command_type_e type, const char* params);
int16_t ESPAT_ReadForResponse(uart_espat_t* pHandle, const char* rsp, unsigned int timeout);
int16_t ESPAT_ReadForResponses(uart_espat_t* pHandle, const char* pass, const char* fail, unsigned int timeout);

//////////////////
// Buffer Stuff //
//////////////////
/// ESPAT_ClearBuffer(uart_espat_t* pHandle - Reset buffer pointer, set all values to 0
void ESPAT_ClearBuffer(uart_espat_t* pHandle);

/// ESPAT_ReadByteToBuffer(uart_espat_t* pHandle - Read first byte from UART receive buffer
/// and store it in rxBuffer.
unsigned int ESPAT_ReadByteToBuffer(uart_espat_t* pHandle);

/// ESPAT_SearchBuffer(uart_espat_t* pHandle,[test]) - Search buffer for string [test]
/// Success: Returns pointer to beginning of string
/// Fail: returns NULL
//! TODO: Fix this function so it searches circularly
char* ESPAT_SearchBuffer(uart_espat_t* pHandle, const char* test);

uint8_t ESPAT_Sync(uart_espat_t* pHandle);

#endif
