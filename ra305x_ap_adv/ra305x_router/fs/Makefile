# Makefile

include $(TOPDIR)/.config


MODULE = $(shell pwd | sed "s/.*\///" ).o
#makefile
EXTRA_CFLAGS += -I./ -I./fat/include -I./usb -I./dm -I./ntfs/include/ntfs-3g
#EXTRA_CFLAGS = -I./ -I./fat/include -I./usb -I./dm
all: $(MODULE)
include $(TOPDIR)/rules.mak

ROOTFSDIR = rootfs
FATDIR = fat
USBDIR = usb
DMDIR = dm
NTFSDIR = ntfs
LIBNTFSDIR = $(NTFSDIR)/libntfs-3g

USBOBJ=$(USBDIR)/usb_disk.o
ROOTFSOBJ=$(ROOTFSDIR)/rootfs.o
FATOBJ=\
	$(FATDIR)/fatfs.o		\
	$(FATDIR)/fatfs_ncache.o		\
	$(FATDIR)/fatfs_supp.o	
#$(NTFSDIR)/ntfs-3g.o   \			
NTFSOBJ=\
	$(NTFSDIR)/ntfs.o   \
	$(LIBNTFSDIR)/acls.o 	\
	$(LIBNTFSDIR)/attrib.o 	\
	$(LIBNTFSDIR)/attrlist.o 	\
	$(LIBNTFSDIR)/bitmap.o 	\
	$(LIBNTFSDIR)/bootsect.o 	\
	$(LIBNTFSDIR)/cache.o 	\
	$(LIBNTFSDIR)/collate.o 	\
	$(LIBNTFSDIR)/compat.o 	\
	$(LIBNTFSDIR)/compress.o 	\
	$(LIBNTFSDIR)/debug.o 	\
	$(LIBNTFSDIR)/device.o 	\
	$(LIBNTFSDIR)/dir.o 		\
	$(LIBNTFSDIR)/efs.o 		\
	$(LIBNTFSDIR)/index.o 	\
	$(LIBNTFSDIR)/inode.o 	\
	$(LIBNTFSDIR)/lcnalloc.o 	\
	$(LIBNTFSDIR)/logfile.o 	\
	$(LIBNTFSDIR)/logging.o 	\
	$(LIBNTFSDIR)/mft.o 		\
	$(LIBNTFSDIR)/misc.o 		\
	$(LIBNTFSDIR)/mst.o 		\
	$(LIBNTFSDIR)/object_id.o 	\
	$(LIBNTFSDIR)/realpath.o	\
	$(LIBNTFSDIR)/reparse.o 	\
	$(LIBNTFSDIR)/runlist.o 	\
	$(LIBNTFSDIR)/unistr.o 	\
	$(LIBNTFSDIR)/volume.o 	\
	$(LIBNTFSDIR)/unix_io.o 	\
	$(LIBNTFSDIR)/xattrs.o

#$(LIBNTFSDIR)/security.o 	\	
DMOBJ=$(DMDIR)/dm.o          	


ALL_OBJS= $(USBOBJ)  $(DMOBJ) $(ROOTFSOBJ) $(FATOBJ) # $(NTFSOBJ) 



$(MODULE): $(ALL_OBJS)
	$(XLD) -r $(ENDIAN) -o $(MODULE) $(ALL_OBJS)

clean:
	rm $(MODULE) $(ALL_OBJS)
