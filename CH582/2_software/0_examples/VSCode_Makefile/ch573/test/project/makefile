TARGET = project

BUILD_DIR = build

C_SOURCES =  \
APP/peripheral.c \
APP/peripheral_main.c \
HAL/MCU.c \
HAL/RTC.c \
HAL/SLEEP.c \
Profile/devinfoservice.c \
Profile/gattprofile.c \
RVMSIS/core_riscv.c \
StdPeriphDriver/CH57x_adc.c \
StdPeriphDriver/CH57x_clk.c \
StdPeriphDriver/CH57x_flash.c \
StdPeriphDriver/CH57x_gpio.c \
StdPeriphDriver/CH57x_pwr.c \
StdPeriphDriver/CH57x_sys.c \
StdPeriphDriver/CH57x_timer0.c \
StdPeriphDriver/CH57x_uart1.c 

C_INCLUDES =  \
-IAPP/include \
-IHAL/include \
-ILIB \
-IProfile/include \
-IRVMSIS \
-IStdPeriphDriver/inc \

ASM_SOURCES =  \
Startup/startup_CH573.s

LDSCRIPT = Ld/Link.ld

LIBS = -lISP573 -lCH57xBLE
LIBDIR = \
-LLIB \
-LStdPeriphDriver

PREFIX = riscv-none-embed-
CC = $(PREFIX)gcc
CPP = $(PREFIX)g++
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

CFLAGS = -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -g -Wall -DDEBUG=1 -DBLE_MAC=1 $(C_INCLUDES) -std=gnu99
ASFLAGS = -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -x assembler
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
ASFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

LDFLAGS = -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -g -T
LDFLAGS += $(LDSCRIPT) -nostartfiles -Xlinker --gc-sections $(LIBDIR) -Xlinker --print-memory-usage -Wl,-Map=$(BUILD_DIR)/$(TARGET).map --specs=nano.specs --specs=nosys.specs -o $(TARGET).elf

all: $(TARGET).elf

OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.S $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR):
	mkdir $@

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) $(CFLAGS)

$(BUILD_DIR)/%.o: %.S Makefile | $(BUILD_DIR)
	$(CC) $(ASFLAGS)

$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS)
	$(SZ) $@

clean:
	rm -rf $(BUILD_DIR)