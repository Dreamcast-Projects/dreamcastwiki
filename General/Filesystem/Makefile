
TARGET = filesystem.elf
OBJS = romdisk.o main.o

all: clean $(TARGET)

include $(KOS_BASE)/Makefile.rules

clean:
	-rm -f $(TARGET) $(OBJS)
	-rm -f romdisk.o romdisk.img

$(TARGET): $(OBJS) 
	kos-cc -o $(TARGET) $(OBJS) $(DATAOBJS) $(OBJEXTRA) -lkosfat

stages:
	$(KOS_GENROMFS) -f stage1.img -d stage1 -v
	$(KOS_GENROMFS) -f stage2.img -d stage1 -v

romdisk.img:
	$(KOS_GENROMFS) -f romdisk.img -d romdisk -v

romdisk.o: romdisk.img
	$(KOS_BASE)/utils/bin2o/bin2o romdisk.img romdisk romdisk.o

run: $(TARGET)
	$(KOS_SERIAL_LOADER) $(TARGET)

dist:
	rm -f $(OBJS) romdisk.o romdisk.img
	$(KOS_STRIP) $(TARGET)


