可以单向捕获到接收到的packet的硬件时间戳
例如：./testnic p0 且helong ping fujian时，icmp request具有硬件时间戳，而icmp reply则不具有硬件时间戳  

## 存在的问题
1、对于p0端口，和之前tcpdump只能在NIC内部捕获到第一对icmp request-icmp reply一样，也只对第一对icmp包有输出  



## 监控资源占用情况
![](https://s3.bmp.ovh/imgs/2023/10/10/f345da6de8e09783.png)  
host运行iperf -c 192.168.37.12  
dpu运行iperf -s -B 192.168.37.12(SF0端口)  
执行./testnic enp3s0f0s4(SF0端口)捕获此端口上tcp packet的硬件时间戳，同时监控testnic进程的cpu和内存占用   
testnic捕获了175121个tcp packet (log文件位于https://drive.google.com/file/d/1eiOW_FnQ4MHQou2Pr6u933HbJU35lG5A/view?usp=drive_link)，一半的packet输出了硬件时间戳，testnic进程的内存占用稳定在1.29M，但cpu占用有较大的波动  
![](https://s3.bmp.ovh/imgs/2023/10/10/d442b09119c8e1cd.png)  