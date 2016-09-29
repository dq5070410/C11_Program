######################################

# Generic Share Library makefile

######################################

 

#target you can change test to what you want

# 共享库文件名， lib*.so

TARGET  := Thread

#compile and lib parameter

# 编译参数

CC      := g++

LIBS    :=

LDFLAGS := 

DEFINES := -DWEBRTC_LINUX

INCLUDE := -I../src

CFLAGS  := -ggdb -Wall -O3 -std=c++11 $(DEFINES) $(INCLUDE)

CXXFLAGS:= $(CFLAGS) -DHAVE_CONFIG_H

SHARE   := -fPIC -o


#source file

# 源文件，自动找所有 .cc 和 .cpp 文件，并将目标定义为同名 .o 文件

SOURCE  := $(wildcard *.cc ./linux/*.cc) $(wildcard *.cpp)

OBJS    := $(patsubst %.cc,%.o,$(patsubst %.cpp,%.o,$(SOURCE)))

 

.PHONY : everything objs clean veryclean rebuild

 

everything : $(TARGET)

 

all : $(TARGET)

 

objs : $(OBJS)

 

rebuild: veryclean everything

               

clean :

    rm -fr *.o

   

veryclean : clean

    rm -fr $(TARGET)

 

$(TARGET) : $(OBJS) 

    $(CC) $(CXXFLAGS) $(SHARE) $@ $(OBJS) $(LDFLAGS) $(LIBS)
