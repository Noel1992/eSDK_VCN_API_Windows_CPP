#                     MAKE MACRO DEFINATION

ROOT =.
CC = g++
LINK = g++

OBJDIR  = $(ROOT)/PlayerObj
BINDIR	= $(ROOT)/build/linux
LIBDIR	= $(ROOT)/lib

#GNU Make 3.80版本不支持ifeq else ifeq这种语法，SUSE10是GNU Make 3.80，SUSE11和Redhat5.4是GNU Make 3.81
ifeq ($(os),suse11)
  RELEASEDIR = $(ROOT)/../release/linux/linux_x64
endif
ifeq ($(os),suse10)
  RELEASEDIR = $(ROOT)/../release/linux/linux_x32_SUSE10
endif
ifeq ($(os),redhat54)
  RELEASEDIR = $(ROOT)/../release/linux/linux_x32_RedHat5.4
endif
ifeq ($(os),v100nptl)
  RELEASEDIR = $(ROOT)/../release/linux/linux_arm_v100nptl
  CC = /opt/hisi-linux-nptl/arm-hisiv100-linux/target/bin/arm-hisiv100nptl-linux-g++
  LINK = /opt/hisi-linux-nptl/arm-hisiv100-linux/target/bin/arm-hisiv100nptl-linux-g++
endif

INCDIR1 = $(ROOT)/../../IVS_COMMON/inc
INCDIR2 = $(ROOT)/inc
INCDIR3 = $(ROOT)/inc/common
INCDIR4 = $(ROOT)/IVS_Player/inc
INCDIR5 = $(ROOT)/IVS_Player/src/Decoder/Hisilicon
INCDIR6 = $(ROOT)/ThirdParty/Intel

SOURCEDIR0 = $(ROOT)/inc/common/BaseThread
SOURCEDIR1 = $(ROOT)/inc/common/log
SOURCEDIR2 = $(ROOT)/inc/common/Timer
SOURCEDIR3 = $(ROOT)/IVS_Player/src/Capture
SOURCEDIR4 = $(ROOT)/IVS_Player/src/channel
SOURCEDIR5 = $(ROOT)/IVS_Player/src/Decoder
SOURCEDIR6 = $(ROOT)/IVS_Player/src/MP4Praser
SOURCEDIR7 = $(ROOT)/IVS_Player/src/Reassemble
SOURCEDIR8 = $(ROOT)/IVS_Player/src/Recorder
SOURCEDIR9 = $(ROOT)/IVS_Player/src/Render
SOURCEDIR10 = $(ROOT)/IVS_Player/src/source
SOURCEDIR11 = $(ROOT)/IVS_Player/src/Voice
SOURCEDIR12 = $(ROOT)/IVS_Player/src
SOURCEDIR13 = $(ROOT)/IVS_Player/src/Decoder/BufferManager
SOURCEDIR14 = $(ROOT)/IVS_Player/src/Reassemble/DecryptData
SOURCEDIR15 = $(ROOT)/IVS_Player/src/Voice/NVSTimer

ifeq ($(ver),debug)
CCFlag =  -O -g -w -c -fmessage-length=0
else
CCFlag =  -O2 -w -c -fmessage-length=0
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
CCFlag += $(STACK_PRO) $(GOT_PRO) $(STACK_NOTEXE_PRO) $(ADD_IND)

LINKFlag = -shared
PRJNAME = IVS_Player

PROGRAM = lib$(PRJNAME).so

INCLUDES = -I$(SOURCEDIR0) -I$(SOURCEDIR1) -I$(SOURCEDIR2) -I$(SOURCEDIR3) -I$(SOURCEDIR4) \
-I$(SOURCEDIR5) -I$(SOURCEDIR6) -I$(SOURCEDIR7) -I$(SOURCEDIR8) -I$(SOURCEDIR9) -I$(SOURCEDIR10) \
-I$(SOURCEDIR11) -I$(SOURCEDIR12) -I$(SOURCEDIR13) -I$(SOURCEDIR14) -I$(SOURCEDIR15) \
-I$(INCDIR1) -I$(INCDIR2) -I$(INCDIR3) -I$(INCDIR4) -I$(INCDIR5) -I$(INCDIR6)

LIBDIRS = -L$(LIBDIR)
LIBS = -lpthread
MACROS = -DUE_TERM_ROUTE -DDEBUG -DMODULE_PLAYER_NAMESPACE
RM = rm -f

#                       SOURCE FILES
SOURCE0 := $(wildcard $(SOURCEDIR0)/*.cpp)
OBJ0 := $(patsubst $(SOURCEDIR0)/%.cpp,$(OBJDIR)/%.o,$(SOURCE0))
SOURCE1 := $(wildcard $(SOURCEDIR1)/*.cpp)
OBJ1 := $(patsubst $(SOURCEDIR1)/%.cpp,$(OBJDIR)/%.o,$(SOURCE1))
SOURCE2 := $(wildcard $(SOURCEDIR2)/*.cpp)
OBJ2 := $(patsubst $(SOURCEDIR2)/%.cpp,$(OBJDIR)/%.o,$(SOURCE2))
SOURCE3 := $(wildcard $(SOURCEDIR3)/*.cpp)
OBJ3 := $(patsubst $(SOURCEDIR3)/%.cpp,$(OBJDIR)/%.o,$(SOURCE3))
SOURCE4 := $(wildcard $(SOURCEDIR4)/*.cpp)
OBJ4 := $(patsubst $(SOURCEDIR4)/%.cpp,$(OBJDIR)/%.o,$(SOURCE4))
SOURCE5 := $(wildcard $(SOURCEDIR5)/*.cpp)
OBJ5 := $(patsubst $(SOURCEDIR5)/%.cpp,$(OBJDIR)/%.o,$(SOURCE5))
SOURCE6 := $(wildcard $(SOURCEDIR6)/*.cpp)
OBJ6 := $(patsubst $(SOURCEDIR6)/%.cpp,$(OBJDIR)/%.o,$(SOURCE6))
SOURCE7 := $(wildcard $(SOURCEDIR7)/*.cpp)
OBJ7 := $(patsubst $(SOURCEDIR7)/%.cpp,$(OBJDIR)/%.o,$(SOURCE7))
SOURCE8 := $(wildcard $(SOURCEDIR8)/*.cpp)
OBJ8 := $(patsubst $(SOURCEDIR8)/%.cpp,$(OBJDIR)/%.o,$(SOURCE8))
SOURCE9 := $(wildcard $(SOURCEDIR9)/*.cpp)
OBJ9 := $(patsubst $(SOURCEDIR9)/%.cpp,$(OBJDIR)/%.o,$(SOURCE9))
SOURCE10 := $(wildcard $(SOURCEDIR10)/*.cpp)
OBJ10 := $(patsubst $(SOURCEDIR10)/%.cpp,$(OBJDIR)/%.o,$(SOURCE10))
SOURCE11 := $(wildcard $(SOURCEDIR11)/*.cpp)
OBJ11 := $(patsubst $(SOURCEDIR11)/%.cpp,$(OBJDIR)/%.o,$(SOURCE11))
SOURCE12 := $(wildcard $(SOURCEDIR12)/*.cpp)
OBJ12 := $(patsubst $(SOURCEDIR12)/%.cpp,$(OBJDIR)/%.o,$(SOURCE12))
SOURCE13 := $(wildcard $(SOURCEDIR13)/*.cpp)
OBJ13 := $(patsubst $(SOURCEDIR13)/%.cpp,$(OBJDIR)/%.o,$(SOURCE13))
SOURCE14 := $(wildcard $(SOURCEDIR14)/*.cpp)
OBJ14 := $(patsubst $(SOURCEDIR14)/%.cpp,$(OBJDIR)/%.o,$(SOURCE14))
SOURCE15 := $(wildcard $(SOURCEDIR15)/*.cpp)
OBJ15 := $(patsubst $(SOURCEDIR15)/%.cpp,$(OBJDIR)/%.o,$(SOURCE15))

OBJS := $(OBJ0) $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) $(OBJ5) $(OBJ6) $(OBJ7) \
	$(OBJ8) $(OBJ9) $(OBJ10) $(OBJ11) $(OBJ12) $(OBJ13) $(OBJ14) $(OBJ15)

$(PROGRAM):  $(OBJS)
	$(LINK) $(MACROS) -o $(PRJNAME)/$(PROGRAM) $(OBJS) $(LINKFlag) $(LIBS) $(LIBDIRS)
$(OBJ0): $(OBJDIR)/%.o : $(SOURCEDIR0)/%.cpp
	$(CC) -c $(MACROS) $(CCFlag) $(INCLUDES) $< -o $@ 
$(OBJ1): $(OBJDIR)/%.o : $(SOURCEDIR1)/%.cpp
	$(CC) -c $(MACROS) $(CCFlag) $(INCLUDES) $< -o $@
$(OBJ2): $(OBJDIR)/%.o : $(SOURCEDIR2)/%.cpp
	$(CC) -c $(MACROS) $(CCFlag) $(INCLUDES) $< -o $@	
$(OBJ3): $(OBJDIR)/%.o : $(SOURCEDIR3)/%.cpp
	$(CC) -c $(MACROS) $(CCFlag) $(INCLUDES) $< -o $@
$(OBJ4): $(OBJDIR)/%.o : $(SOURCEDIR4)/%.cpp
	$(CC) -c $(MACROS) $(CCFlag) $(INCLUDES) $< -o $@
$(OBJ5): $(OBJDIR)/%.o : $(SOURCEDIR5)/%.cpp
	$(CC) -c $(MACROS) $(CCFlag) $(INCLUDES) $< -o $@
$(OBJ6): $(OBJDIR)/%.o : $(SOURCEDIR6)/%.cpp
	$(CC) -c $(MACROS) $(CCFlag) $(INCLUDES) $< -o $@
$(OBJ7): $(OBJDIR)/%.o : $(SOURCEDIR7)/%.cpp
	$(CC) -c $(MACROS) $(CCFlag) $(INCLUDES) $< -o $@
$(OBJ8): $(OBJDIR)/%.o : $(SOURCEDIR8)/%.cpp
	$(CC) -c $(MACROS) $(CCFlag) $(INCLUDES) $< -o $@ 
$(OBJ9): $(OBJDIR)/%.o : $(SOURCEDIR9)/%.cpp
	$(CC) -c $(MACROS) $(CCFlag) $(INCLUDES) $< -o $@
$(OBJ10): $(OBJDIR)/%.o : $(SOURCEDIR10)/%.cpp
	$(CC) -c $(MACROS) $(CCFlag) $(INCLUDES) $< -o $@	
$(OBJ11): $(OBJDIR)/%.o : $(SOURCEDIR11)/%.cpp
	$(CC) -c $(MACROS) $(CCFlag) $(INCLUDES) $< -o $@
$(OBJ12): $(OBJDIR)/%.o : $(SOURCEDIR12)/%.cpp
	$(CC) -c $(MACROS) $(CCFlag) $(INCLUDES) $< -o $@
$(OBJ13): $(OBJDIR)/%.o : $(SOURCEDIR13)/%.cpp
	$(CC) -c $(MACROS) $(CCFlag) $(INCLUDES) $< -o $@
$(OBJ14): $(OBJDIR)/%.o : $(SOURCEDIR14)/%.cpp
	$(CC) -c $(MACROS) $(CCFlag) $(INCLUDES) $< -o $@
$(OBJ15): $(OBJDIR)/%.o : $(SOURCEDIR15)/%.cpp
	$(CC) -c $(MACROS) $(CCFlag) $(INCLUDES) $< -o $@

clean::
	$(RM) $(OBJDIR)/*
	$(RM) $(ROOT)/libIVS_Player.so
	
#	$(RM) $(BINDIR)/*
cleanpart::
	$(RM) $(OBJDIR)/*.o
dep: delete_previous_depend $(OBJS:%.o=%_dep)
delete_previous_depend:
	$(RM) -f depend
$(OBJ0:%.o=%_dep): $(OBJDIR)/%_dep : $(SOURCEDIR0)/%.cpp
	$(CC) -MM  -MT '$(OBJDIR)/$*.o' $(CCFlag) $< $(INCLUDES) >>depend
$(OBJ1:%.o=%_dep): $(OBJDIR)/%_dep : $(SOURCEDIR1)/%.cpp
	$(CC) -MM  -MT '$(OBJDIR)/$*.o' $(CCFlag) $< $(INCLUDES) >>depend
$(OBJ2:%.o=%_dep): $(OBJDIR)/%_dep : $(SOURCEDIR2)/%.cpp
	$(CC) -MM  -MT '$(OBJDIR)/$*.o' $(CCFlag) $< $(INCLUDES) >>depend
$(OBJ3:%.o=%_dep): $(OBJDIR)/%_dep : $(SOURCEDIR3)/%.cpp
	$(CC) -MM  -MT '$(OBJDIR)/$*.o' $(CCFlag) $< $(INCLUDES) >>depend
$(OBJ4:%.o=%_dep): $(OBJDIR)/%_dep : $(SOURCEDIR4)/%.cpp
	$(CC) -MM  -MT '$(OBJDIR)/$*.o' $(CCFlag) $< $(INCLUDES) >>depend
$(OBJ5:%.o=%_dep): $(OBJDIR)/%_dep : $(SOURCEDIR5)/%.cpp
	$(CC) -MM  -MT '$(OBJDIR)/$*.o' $(CCFlag) $< $(INCLUDES) >>depend
$(OBJ6:%.o=%_dep): $(OBJDIR)/%_dep : $(SOURCEDIR6)/%.cpp
	$(CC) -MM  -MT '$(OBJDIR)/$*.o' $(CCFlag) $< $(INCLUDES) >>depend
$(OBJ7:%.o=%_dep): $(OBJDIR)/%_dep : $(SOURCEDIR7)/%.cpp
	$(CC) -MM  -MT '$(OBJDIR)/$*.o' $(CCFlag) $< $(INCLUDES) >>depend
$(OBJ8:%.o=%_dep): $(OBJDIR)/%_dep : $(SOURCEDIR8)/%.cpp
	$(CC) -MM  -MT '$(OBJDIR)/$*.o' $(CCFlag) $< $(INCLUDES) >>depend
$(OBJ9:%.o=%_dep): $(OBJDIR)/%_dep : $(SOURCEDIR9)/%.cpp
	$(CC) -MM  -MT '$(OBJDIR)/$*.o' $(CCFlag) $< $(INCLUDES) >>depend
$(OBJ10:%.o=%_dep): $(OBJDIR)/%_dep : $(SOURCEDIR10)/%.cpp
	$(CC) -MM  -MT '$(OBJDIR)/$*.o' $(CCFlag) $< $(INCLUDES) >>depend
$(OBJ11:%.o=%_dep): $(OBJDIR)/%_dep : $(SOURCEDIR11)/%.cpp
	$(CC) -MM  -MT '$(OBJDIR)/$*.o' $(CCFlag) $< $(INCLUDES) >>depend
$(OBJ12:%.o=%_dep): $(OBJDIR)/%_dep : $(SOURCEDIR12)/%.cpp
	$(CC) -MM  -MT '$(OBJDIR)/$*.o' $(CCFlag) $< $(INCLUDES) >>depend
$(OBJ13:%.o=%_dep): $(OBJDIR)/%_dep : $(SOURCEDIR13)/%.cpp
	$(CC) -MM  -MT '$(OBJDIR)/$*.o' $(CCFlag) $< $(INCLUDES) >>depend
$(OBJ14:%.o=%_dep): $(OBJDIR)/%_dep : $(SOURCEDIR14)/%.cpp
	$(CC) -MM  -MT '$(OBJDIR)/$*.o' $(CCFlag) $< $(INCLUDES) >>depend
$(OBJ15:%.o=%_dep): $(OBJDIR)/%_dep : $(SOURCEDIR15)/%.cpp
	$(CC) -MM  -MT '$(OBJDIR)/$*.o' $(CCFlag) $< $(INCLUDES) >>depend

-include depend 
copy:
#copy libIVS_Player.so
	yes|cp -f $(ROOT)/IVS_Player/libIVS_Player.so $(RELEASEDIR)
	yes|cp -f $(ROOT)/inc/IVS_Player.h $(RELEASEDIR)/../inc
	yes|cp -f $(ROOT)/inc/IVSPlayerDataType.h $(RELEASEDIR)/../inc
	yes|cp -f $(ROOT)/inc/RtpDataType.h $(RELEASEDIR)/../inc
