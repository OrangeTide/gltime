include config.mk
gltime : gltime.c
gltime : LDLIBS := -lgdi32 -lopengl32
gltime : CFLAGS := -Wall -W -Os -g
