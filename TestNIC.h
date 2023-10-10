#pragma once

#include <cstring>
#include <iostream>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>
// Timestamping enums
#include <linux/net_tstamp.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include<sys/ioctl.h>
#include <net/if.h>
#include <linux/sockios.h>
#include <pcap.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
namespace TestNIC
{
	class TestNIC
	{
	public:
		void EnableTimestamping(int sock);
		void ListenMulticast(std::string device);
		void DoEventLoop();
		void ReadPacket(int sock);

	private:
		int m_udp_feed;
		int m_tcp_socket;

		char *read_buffer;
		char *devicename;
	};
}
