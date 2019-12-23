include config.mk
ifneq ($X,)
%$X : %.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
%$X : %.c
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@
endif
#####
all :: gltime$X
clean :: ; $(RM) gltime$X
gltime$X : gltime.c
gltime$X : LDLIBS := -lgdi32 -lopengl32 -lglu32
gltime$X : CFLAGS := -Wall -W -Os -g
#####
all :: pal
clean :: ; $(RM) pal
pal : CC=gcc
