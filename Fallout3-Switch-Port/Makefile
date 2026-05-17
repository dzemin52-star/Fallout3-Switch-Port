TOPDIR   ?= $(DEVKITPRO)
PORTLIBS ?= $(TOPDIR)/portlibs/switch

TARGET   := Fallout3Switch
BUILD    := build
SOURCES  := src src/engine src/renderer src/physics src/audio src/memory src/debug src/bethesda_format

ARCH     := -march=armv8-a -mtune=cortex-a57 -mtp=soft -fPIE
CFLAGS   := -g -O2 -ffunction-sections -fdata-sections $(ARCH) -D__SWITCH__
CXXFLAGS := $(CFLAGS) -std=c++17
LDFLAGS  := -specs=switch.specs $(ARCH) -Wl,-Map,$(TARGET).map

LIBS     := -lEGL -lGLESv2 -laudren -lz -lnx -lm

include $(DEVKITPRO)/switch_rules