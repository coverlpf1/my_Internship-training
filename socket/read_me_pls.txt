本次程序依托于所给教材中的UDP发送消息示例代码与TCP发送文件示例代码
通过一定的修改，在原代码的基础上实现的
UDP：
    1.发送消息自定义
    2.可重复发送消息
    3.使用线程实现，不影响主进程的进行。
TCP：
    1.设置发送目录与接受目录
    2.使用字符串拼接的方式，实现了路径与文件名的分离
此处涉及strcpy，与strcat的使用，c语言中string字符串功能较弱
    3.发送端（即服务器端）可自行选择发送文件（通过输入文件名的方式）
    4.使用线程实现，不影响主进程的进行。
文件夹内容讲解：
    file_c.c程序客户端的实现代码
    file_s.c程序服务器的实现代码
    client：根据客户端代码生成的客户端程序
    server：根据服务器代码生成的服务器程序
    send文件夹：发送文件的指定路径
    receive文件夹：接收文件的指定路径
    Makefile：根据file_c.c与file_s.c生成可使用的程序的文件，
    用于在进行修改后，重新生成文件。
    udp_C与udp_s为本人测试将UDP连接转为线程实现时所使用的测试代码