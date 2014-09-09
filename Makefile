include mkenv.mk
include magic.mk

CFLAGS = -march=armv5te -mfloat-abi=soft -Wall \
	 -Os -ggdb -Iinclude
AFLAGS = -D__ASSEMBLY__

LDFLAGS = --nostdlib -T fernvale.ld
LIBS =

SRC_C = \
	bionic.c \
	cmd-irq.c \
	emi.c \
	irq.c \
	main.c \
	scriptic.c \
	serial.c \
	utils.c \
	vectors.c \
	vsprintf.c

SRC_S = \
	scriptic/set-plls.S \
	scriptic/enable-psram.S \
	_udivsi3.S \
	_divsi3.S \
	start.S

OBJ = $(addprefix $(BUILD)/, $(SRC_S:.S=.o) $(SRC_C:.c=.o))

all: $(BUILD)/firmware.bin \
	$(BUILD)/loader.bin \
	$(BUILD)/usb-loader.bin \
	$(BUILD)/fernly-loader
clean:
	$(RM) -rf $(BUILD)

$(BUILD)/fernly-loader: fernly-loader.c
	$(CC) fernly-loader.c -o $@

$(BUILD)/loader.bin: $(BUILD)/loader.o
	objcopy -S -O binary $(BUILD)/loader.o $@

$(BUILD)/usb-loader.bin: $(BUILD)/usb-loader.o
	objcopy -S -O binary $(BUILD)/usb-loader.o $@

$(BUILD)/loader.o: loader.S
	as loader.S -o $@

HEADER_BUILD = $(BUILD)/genhdr
$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	objcopy -S -O binary $(BUILD)/firmware.elf $@

$(BUILD)/firmware.elf: $(OBJ)
	$(LD) $(LDFLAGS) --entry=reset_handler -o $@ $(OBJ) $(LIBS)

$(OBJ): $(HEADER_BUILD)/generated.h | $(OBJ_DIRS)
$(HEADER_BUILD)/generated.h: | $(HEADER_BUILD)
	  touch $@

OBJ_DIRS = $(sort $(dir $(OBJ))) scriptic
$(OBJ_DIRS):
	$(MKDIR) -p $@ $@/scriptic
$(HEADER_BUILD):
	$(MKDIR) -p $@ build/scriptic
-include $(OBJ:.o=.P)
