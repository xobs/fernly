include mkenv.mk
include magic.mk

CFLAGS = -mtune=arm7tdmi -mcpu=arm7tdmi -mfloat-abi=soft -Wall \
	 -O0 -ggdb -Iinclude

LDFLAGS = --nostdlib -T fernvale.ld
LIBS =

SRC_C = \
	main.c \
	vectors.c \
	serial.c \
	utils.c \
	bionic.c

SRC_S = \
	start.S

OBJ = $(addprefix $(BUILD)/, $(SRC_S:.S=.o) $(SRC_C:.c=.o))

all: $(BUILD)/firmware.bin
clean:
	$(RM) -rf $(BUILD)

HEADER_BUILD = $(BUILD)/genhdr
$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	objcopy -S -O binary $(BUILD)/firmware.elf $@

$(BUILD)/firmware.elf: $(OBJ)
	$(LD) $(LDFLAGS) --entry=reset_handler -o $@ $(OBJ) $(LIBS)

$(OBJ): $(HEADER_BUILD)/generated.h | $(OBJ_DIRS)
$(HEADER_BUILD)/generated.h: | $(HEADER_BUILD)
	  touch $@

OBJ_DIRS = $(sort $(dir $(OBJ)))
$(OBJ_DIRS):
	$(MKDIR) -p $@
$(HEADER_BUILD):
	$(MKDIR) -p $@
-include $(OBJ:.o=.P)
