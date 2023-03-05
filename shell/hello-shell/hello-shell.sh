#!/bin/bash
#名称： hello-shell.sh
#作者： 李鹏飞
#日期： 2022-06-04
#功能： 练习

CURDATE=`date`
printf "i wanna get the date, output like this?\n %s\n" '$CURDATE'
echo -e "no! just output like this:\n $CURDATE"
echo -e "the hostname is:\n$(hostname)"
echo -e "the current user is:\n$(whoami)"
echo -e "the os info is:\n$(uname -a)"
echo -e "the process info is:\n$(ps -u)"
echo -e "the disk status is:\n$(du -sh /home)"
cd ~/starnet-shell/hello-shell
mkdir logdir
cd logdir
touch file1 \
	file3 \
	file5 \
cp file1 file2
mv file5 file4
ls -i
cat file1
echo $?
cat file5
echo $?
rm -r ~/starnet-shell/hello-shell/logdir

