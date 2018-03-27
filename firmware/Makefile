# Project name
NAME			:= dht-i2c

# Project source files
SRCS			:= dht.c \
			   main.c \
			   i2c_slave.c \
			   util.c

# Project fuse bits
# 16 Mhz PLL, WDTon, BOD 2.7
LFUSE			:= 0xF1
HFUSE			:= 0xCD
EFUSE			:= 0xFF

# CPU speed, in Hz
F_CPU			:= 16000000UL

# Architecture configuration
GCC_ARCH		:= attiny85
AVRDUDE_ARCH		:= t85
FUNC_STACK_LIMIT	:=

# Programmer selection.
# Values can be:  avrisp2, mysmartusb
PROGRAMMER		:= avrisp2

# Additional compiler flags
I2CS_EXPECTED_KHZ	:= 40
CFLAGS			:= -DI2CS_MAX_NR_SLAVES=1 \
			   -DI2CS_CLKSTRETCH_WORKAROUND \
			   -DI2CS_EXPECTED_KHZ=$(I2CS_EXPECTED_KHZ)
LDFLAGS			:=

# Additional "clean" and "distclean" target files
CLEAN_FILES		:=
DISTCLEAN_FILES		:=


include avrmakelib.mk
