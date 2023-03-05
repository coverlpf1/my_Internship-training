#!/bin/bash
#作者： LPF
#作用： 大作业内容
#cat arp.txt | awk 'gsub(/\(/,b)gsub(/\)/,b)gsub(/:/,b) {print $4,$2}' > arp_tmp.txt
#此处为个人尝试
#从所有后缀为_logcat的文件中提取出包含GL error的文件
#find /home/lpf/starnet-shell/homework/logs -name "*_logcat" -exec grep -m 1 -I -in "GL error" {} -H \;
#结果为/home/lpf/starnet-shell/homework/logs/c40938ef97c2_logcat:344157:01-01 17:57:12.030 E/SurfaceFlinger( 1578): GL error 0x0505
#      /home/lpf/starnet-shell/homework/logs/c40938ed0804_logcat:558877:01-02 21:38:46.300 E/SurfaceFlinger( 1549): GL error 0x0505
#做了特定的处理，每个文件只输出第一次出错的地方，防止大量错误记录

FILE_LIST=`find ./logs/ -name "*_logcat"`
#根据文件名获取MAC地址
function Get_MAC()
{
    MAC_tem=`find ./logs/  -ipath $1 | cut -c 8-19`
}
#根据MAC地址获取IP
function GET_IP()
{
    IP_tem=`grep -w $1 arp_tmp.txt| awk '{print $2}'`
}
#根据要求筛选文件
function check()
{
    CHECK_res_time=`grep -m 1 -w $2 $1 -r -s | cut -c 29-45`
    CHECK_res_location=`grep -m 1 -w $2 $1 -r -s | cut -c 47-69`
    CHECK_res_type=`grep -m 1 -w $2 $1 -r -s | cut -c 72-86`
}
NUM=5
while [ $NUM -gt 0 ]; do
    echo "IP                MAC                 ERROR_TIME            ERROR_LOCATION               ERROR_TYPE"
    for FILE in $FILE_LIST; do
        Get_MAC $FILE
#    echo $MAC_tem
        GET_IP $MAC_tem
#    echo "当前文件IP地址为"
        check $FILE "GL error"
        echo $IP_tem   "    "  $MAC_tem       "    "    $CHECK_res_time          "    "       $CHECK_res_location         "    "        $CHECK_res_type
    done
    NUM=`expr $NUM - 1`
    sleep 5
done
#函数功能测试
#Get_MAC ./logs/c4093805b14d_logcat
#echo $MAC_tem
#GET_IP $MAC_tem
#echo $IP_tem
#Get_MAC ./logs/c40938ecdd0d_logcat
#echo $MAC_tem
#GET_IP $MAC_tem
#echo $IP_tem