main:client server clean

client:file_c.o
	gcc -I. -o $@ file_c.o -lpthread

server:file_s.o 
	gcc -I. -o $@ file_s.o -lpthread

file_c.o:file_c.c 
	gcc -c file_c.c

file_s.o:file_s.c 
	gcc -c  file_s.c 

#清理工程规则
.PHONY:clean
clean:
	@echo "正在清除编译产生的二进制文件"
	-rm *.o 