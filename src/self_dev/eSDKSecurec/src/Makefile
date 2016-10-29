#############################################################################
# Makefile for building: eSDKSecurec
#############################################################################

CC         = g++

ROOT_DIR  = ./

#GNU Make 3.80版本不支持ifeq else ifeq这种语法，SUSE10是GNU Make 3.80，SUSE11和Redhat5.4是GNU Make 3.81
ifeq ($(bit),64)
  C++FLAGS  += -pipe -g -Wall -O0 -m64 -DENV_LINUX
  LIBS      += -shared -m64 -lm -lpthread
endif
ifeq ($(bit),32)
  C++FLAGS  += -pipe -g -Wall -O0 -m32 -DLINUX
  LIBS      += -shared -m32 -lm -lpthread
endif

ifeq ($(os), v100nptl)
  CC	= /opt/hisi-linux-nptl/arm-hisiv100-linux/target/bin/arm-hisiv100nptl-linux-g++
  C++FLAGS  = -pipe -g -Wall -O0 -DLINUX
  LIBS      = -shared -lm -lpthread
endif

#Security compiler options
#stack protector
STACK_PRO = -fstack-protector --param ssp-buffer-size=4
#GOT protector
GOT_PRO = -Wl,-z,relro,-z,now
#Dynamic library search path
#DLL_SEARCHPATH = -Wl,--disable-new-dtags,--rpath,/libpath1:/libpath2
#Stack cannot be executed
STACK_NOTEXE_PRO = -Wl,-z,noexecstack
#Address independent
ADD_IND = -fPIC
C++FLAGS += $(STACK_PRO) $(GOT_PRO) $(STACK_NOTEXE_PRO) $(ADD_IND)

LIBDIR += -Ldir $(ROOT_DIR)Huawei_Secure_C_V100R001C01B019/lib/libsecurec.so
INCPATH += -I $(ROOT_DIR)Huawei_Secure_C_V100R001C01B019/include

TARGET_DIR = $(ROOT_DIR)../release/lib/
TARGET     = libeSDKSecurec.so

CPPFILES  += $(wildcard $(ROOT_DIR)*.cpp)
CFILES    += $(wildcard $(ROOT_DIR)*.c)
HEADFILES += $(wildcard $(ROOT_DIR)*.h)
HEADFILES += $(wildcard $(INCPATH)*.h)

all:
	$(CC) $(LIBS) $(C++FLAGS) $(HEADFILES) $(CPPFILES) $(LIBDIR) $(INCPATH) -o $(TARGET)
	
copy:
	cp ./$(TARGET) ../release/
	cp $(HEADFILES) ../release/inc

clean:
		rm -f $(TARGET) $(CPPOBJS) $(COBJS)


