可以单向捕获到接收到的packet的硬件时间戳
例如：./testnic p0 且helong ping fujian时，icmp request具有硬件时间戳，而icmp reply则不具有硬件时间戳  

## 存在的问题
1、和之前tcpdump只能在NIC内部捕获到第一对icmp request-icmp reply一样，也只对第一对icmp包有输出  

