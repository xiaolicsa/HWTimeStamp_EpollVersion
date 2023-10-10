#include "TestNIC.h"
//可以对从以太网进入p0的捕获到硬件时间戳
namespace TestNIC
{
	void TestNIC::EnableTimestamping(int sock)
	{
		int flags =
			SOF_TIMESTAMPING_RX_HARDWARE
			| SOF_TIMESTAMPING_TX_HARDWARE
			| SOF_TIMESTAMPING_RAW_HARDWARE;
 			// | SOF_TIMESTAMPING_SYS_HARDWARE; // Apparently this option is deprecated?

		std::cout << "Selecting hardware timestamping mode 0x";
		std::cout << std::hex << flags << std::dec << " [fd = " << sock << "]" << std::endl;

		int rc = setsockopt(sock, SOL_SOCKET, SO_TIMESTAMPING, &flags, sizeof(flags));
		std::cout << "Return code from setsockopt() is [rc = " << rc << "]" << std::endl;

		if (true)
		{
			int enable = 1;
			rc = setsockopt(sock, SOL_SOCKET, SO_TIMESTAMP, &enable, sizeof(enable));
			std::cout << "Return code of SO_TIMESTAMP [rc = " << rc << "]" << std::endl;
		}

		if (true)
		{
			int enable = 1;
			rc = setsockopt(sock, SOL_SOCKET, SO_TIMESTAMPNS, &enable, sizeof(enable));
			std::cout << "Return code of SO_TIMESTAMPNS [rc = " << rc << "]" << std::endl;
		}
	}

	void TestNIC::ReadPacket(int sock)
	{
		struct msghdr msg;
		struct sockaddr_in host_address;
		struct iovec iov;
		int flags = 0;

		char control[1024]; // Buffer for CMSGs
		char buffer[4000]; // Data Buffer

		host_address.sin_family = AF_INET;
		host_address.sin_port = htons(ETH_P_IP);
		host_address.sin_addr.s_addr = INADDR_ANY;

		msg.msg_namelen = sizeof(struct sockaddr_in); // Needs to be set each call
		msg.msg_name = &host_address;

		msg.msg_iov = &iov;
		msg.msg_iovlen = 1; // One buffer
		iov.iov_base = buffer;
		iov.iov_len = 4000;

		msg.msg_control = control;
		msg.msg_controllen = 1024; // Needs to be set each call

		// Do a recvmsg()
		ssize_t bytes_rcvd = recvmsg(sock, &msg, MSG_DONTWAIT);
		std::cout<<"---------------------start---------------"<<std::endl;
		std::cout << "Bytes received = " << bytes_rcvd << std::endl;
		std::cout << "Length of CMSGs = " << msg.msg_controllen << std::endl;
		std::cout << "Message flags: " << msg.msg_flags << std::endl;

		std::cout << "Attempting to extract timestamps ..." << std::endl;

		struct cmsghdr* cmsg;
		for (cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg))
		{
			std::cout << "CMSG level = " << cmsg->cmsg_level << std::endl;
			std::cout << "CMSG type = " << cmsg->cmsg_type << std::endl;

			if (cmsg->cmsg_type == 35) // SO_TIMESTAMPNS
			{
				// SO_TIMESTAMPNS
				struct timespec *ts = (struct timespec *)CMSG_DATA(cmsg);
				std::cout << "SO_TIMESTAMPNS: [sec = " << ts->tv_sec << "]";
				std::cout << "[nsec = " << ts->tv_nsec << "]" << std::endl;
			}

			if (cmsg->cmsg_type == 37) // SO_TIMESTAMPING ？只有这一种包？
			{
				struct timespec *ts = (struct timespec *)CMSG_DATA(cmsg);
				std::cout << "SO_TIMESTAMPING:" << std::endl;

				std::cout << "SW  Timespec [sec = " << ts[0].tv_sec << "]";
				std::cout << "[nsec = " << ts[0].tv_nsec << "]" << std::endl; // 0 = Software

				std::cout << "HW  Timespec [sec = " << ts[1].tv_sec << "]";
				std::cout << "[nsec = " << ts[1].tv_nsec << "]" << std::endl; // 1 = HW transformed

				std::cout << "RAW Timespec [sec = " << ts[2].tv_sec << "]";
				std::cout << "[nsec = " << ts[2].tv_nsec << "]" << std::endl; // 2 = HW Raw

				
			}
			

		}
			struct ethhdr *eth_header;
			struct ip *ip_header;
			struct tcphdr *tcp_header;
			struct udphdr *udp_header;
			struct icmphdr *icmp_header;
			eth_header = (struct ethhdr *)buffer;
            uint16_t eth_type = ntohs(eth_header->h_proto);

            // 根据以太网帧类型判断数据包类型
            if (eth_type == ETH_P_IP) {
                ip_header = (struct ip *)(buffer + sizeof(struct ethhdr));
                int ip_protocol = ip_header->ip_p;

                if (ip_protocol == IPPROTO_TCP) {
                    tcp_header = (struct tcphdr *)(buffer + sizeof(struct ethhdr) + sizeof(struct ip));
                    printf("Received TCP packet\n");
                } else if (ip_protocol == IPPROTO_UDP) {
                    udp_header = (struct udphdr *)(buffer + sizeof(struct ethhdr) + sizeof(struct ip));
                    printf("Received UDP packet\n");
                }  else if (ip_protocol == IPPROTO_ICMP) {
                    icmp_header = (struct icmphdr *)(buffer + sizeof(struct ethhdr) + sizeof(struct ip));
                    printf("Received ICMP packet\n");
                } 
                else {
                    printf("Received IP packet with unknown protocol\n");
                }
            } else {
                printf("Received non-IP packet\n");
            }
		std::cout<<"---------------------end---------------"<<std::endl;

	}

	void TestNIC::ListenMulticast(std::string device){
		int sockfd;
		struct sockaddr_ll sll;
		char buffer[65536];  // 缓冲区大小

		// 创建原始套接字
		sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
		if (sockfd < 0) {
			perror("Socket creation error");
			return ;
		}

		// 设置套接字选项，指定捕获的网卡
		int idx = if_nametoindex(device.c_str());
		sll.sll_family = AF_PACKET;
		sll.sll_protocol = htons(ETH_P_ALL);
		sll.sll_ifindex = idx;

		if (bind(sockfd, (struct sockaddr *)&sll, sizeof(sll)) < 0) {
			perror("Binding error");
			return ;
		}
		struct ifreq iface;
		snprintf(iface.ifr_name, IFNAMSIZ, "%s", device.c_str());
		devicename=(char *)device.c_str();
		if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, &iface, sizeof(iface)) < 0)
		{
			
			perror("setsockopt(SO_BINDTODEVICE) error");
			return ;
		}
		
		m_udp_feed=sockfd;
		// EnableTimestamping(m_udp_feed);
		struct ifreq ifr;
		struct hwtstamp_config config;

		config.flags = 0;
		config.tx_type = 1;
		config.rx_filter = 1;
		snprintf(ifr.ifr_name, IFNAMSIZ, "%s", device.c_str());
		ifr.ifr_data = (caddr_t)&config;

		if (ioctl(sockfd, SIOCSHWTSTAMP, &ifr)) {
			perror("ioctl");
			return ;
		}
		int hwts_rp =  SOF_TIMESTAMPING_RX_HARDWARE|SOF_TIMESTAMPING_TX_HARDWARE| SOF_TIMESTAMPING_RAW_HARDWARE;
		if (setsockopt(sockfd, SOL_SOCKET, SO_TIMESTAMPING, &hwts_rp, sizeof(hwts_rp)) < 0)
		{
			printf("error setsockopt(SO_TIMESTAMPING): %s\n", strerror(errno));
			return ;
		}
	return ;
	}

	void TestNIC::DoEventLoop()
	{
		struct epoll_event evt;
		struct epoll_event *evts;

		int epfd = epoll_create(1);
		if (epfd == -1)
			std::cout << "epoll_create() failed";

		std::cout << "epoll created [epfd = " << epfd << "]" << std::endl;

		evt.data.fd = m_udp_feed;
		evt.events = EPOLLIN;
		int rc = epoll_ctl(epfd, EPOLL_CTL_ADD, m_udp_feed, &evt);

		std::cout << "Added socket to epoll" << std::endl;

		while (true)
		{
			int rc = epoll_wait(epfd, &evt, 1, -1);

			if (rc > 0)
			{
				std::cout << "Packet received" << std::endl;
				ReadPacket(m_udp_feed);
			}

			if (rc == 0)
				std::cout << "epoll_wait() timed out" << std::endl;

			if (rc == -1)
			{
				std::cout << "epoll_wait() failed [error = " << errno << "]";
				return;
			}

		} // while (true)



	}

}
