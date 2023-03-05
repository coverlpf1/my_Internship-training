all:check main
check:
	@echo "当前需要预编译的文件为\n"
	@echo $(exam)
#未知错误，main：之后使用$(exam)会显示没有输入然后报错，但是在gcc处使用exam却可正常执行
main :db.o net.o ui.o server.o
	@echo "正在根据二进制文件编译"
	@echo $(objects)
	@echo $(exam)
	gcc -I. -o $@ $(exam)

path = $(shell pwd)

include_path =$(path)/include

lib_path =$(path)/lib
#通过wildcard函数读取文件
objects = $(wildcard */*/*.c)
#通过notdir函数删除路径
dir = $(notdir $(objects))
#通过patsubst函数转换格式
exam = $(patsubst %.c,%.o,$(dir))

db.o: $(include_path)/db/db.h $(lib_path)/db/db.c
	gcc -c  $(lib_path)/db/db.c

net.o:$(include_path)/net/net.h $(lib_path)/net/net.c
	gcc -c  $(lib_path)/net/net.c

ui.o:db.o net.o
	gcc -c  $(lib_path)/ui/ui.c

server.o:ui.o /home/lpf/starnet/makefile-project/include/server/server.h
	gcc -c  $(path)/app/server/server.c


#清理工程规则
.PHONY:clean
clean:
	@echo "正在清除编译产生的二进制文件"
	-rm *.o main
#定义编译器优化选项参数
#CXXFLAGS = -02
#定义编译器路径包含参数
#INCPATH = -I.
#安装命令变量定义
#INSTALL_FILE = install -m 644 -p
#安装工程规则
#install:
#	-$(INSTALL_FILE) main ../

