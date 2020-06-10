#
# Makefile
#

#.EXPORT_ALL_VARIABLES:

include $(TOPDIR)/.config

ALL_OBJS = ftp.o osapi.o


MODULE = $(shell pwd | sed "s/.*\///" ).o

all: $(MODULE) ../include/autoconf.h

include $(TOPDIR)/rules.mak

$(MODULE): $(ALL_OBJS:.o=.d)
	$(XLD) -r $(ENDIAN) -o $(MODULE) $(ALL_OBJS)

clean:
	rm $(MODULE) $(ALL_OBJS)


