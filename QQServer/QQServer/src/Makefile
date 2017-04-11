CPPFLAGS= -std=c++11
CFLAGS=   -g -Wall -D QQDebug -fpermissive
LDFLAGS=-lmysqlcppconn
CC=g++

#CC=arm-linux-gcc

CPP_SRCS = $(wildcard *.cpp)
C_SRCS =   $(wildcard *.c)
CPP_OBJS =$(patsubst %.cpp,%.o,$(CPP_SRCS))
C_OBJS   =$(patsubst %.c,%.o,$(C_SRCS))

target =QQServerd

$(target):$(CPP_OBJS) $(C_OBJS)
	$(CC) $^ $(LDFLAGS) -o $@
	mv $(target) ../

%.o:%.cpp
	$(CC) -c $< $(CFLAGS) $(CPPFLAGS) -o $@
%.o:%.c
	$(CC) -c $< $(CFLAGS)  -o $@

.PHONY:clean

#彻底清除生生过程文件
clean:
	rm -rf *.o
	rm -rf $(target)
	rm -rf ../bin/*


