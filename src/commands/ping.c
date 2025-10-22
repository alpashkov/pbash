#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>

#define PING_PKT_SIZE 64
#define PING_COUNT 4
#define TIMEOUT_SEC 1

/*
было больно
this was painful
*/

static
unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    while (len > 1) {
        sum += *buf++;
        len -= 2;
    }
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

int ping(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ping <host>\n");
        return 1;
    }

    const char *target = argv[1];
    struct hostent *he = gethostbyname(target);
    if (!he) {
        fprintf(stderr, "ping: unknown host %s\n", target);
        return 1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);

    printf("PING %s (%s): %d data bytes\n", target, inet_ntoa(addr.sin_addr), PING_PKT_SIZE);

    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket");
        fprintf(stderr, "Try running as root (sudo).\n");
        return 1;
    }

    struct timeval timeout = {TIMEOUT_SEC, 0};
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    int transmitted = 0, received = 0;
    pid_t pid = getpid() & 0xFFFF;

    for (int seq = 1; seq <= PING_COUNT; ++seq) {
        unsigned char packet[PING_PKT_SIZE];
        memset(packet, 0, sizeof(packet));

        struct icmphdr *icmp = (struct icmphdr *)packet;
        icmp->type = ICMP_ECHO;
        icmp->code = 0;
        icmp->un.echo.id = pid;
        icmp->un.echo.sequence = htons(seq);

        // payload
        size_t payload_len = sizeof(packet) - sizeof(struct icmphdr);
        for (size_t i = 0; i < payload_len; ++i)
            packet[sizeof(struct icmphdr) + i] = (unsigned char)(0x42 + (i % 23));

        icmp->checksum = 0;
        icmp->checksum = checksum(packet, sizeof(packet));

        transmitted++;
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        ssize_t sent = sendto(sockfd, packet, sizeof(packet), 0,
                              (struct sockaddr *)&addr, sizeof(addr));
        if (sent < 0) {
            perror("sendto");
            continue;
        }

        unsigned char recvbuf[1500];
        struct sockaddr_in reply_addr;
        socklen_t reply_len = sizeof(reply_addr);
        ssize_t n = recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0,
                             (struct sockaddr *)&reply_addr, &reply_len);

        clock_gettime(CLOCK_MONOTONIC, &end);
        double ms = (end.tv_sec - start.tv_sec) * 1000.0 +
                    (end.tv_nsec - start.tv_nsec) / 1e6;

        if (n > 0) {
            struct iphdr *ip = (struct iphdr *)recvbuf;
            int iphdrlen = ip->ihl * 4;
            if (n >= iphdrlen + (ssize_t)sizeof(struct icmphdr)) {
                struct icmphdr *r_icmp = (struct icmphdr *)(recvbuf + iphdrlen);
                if (r_icmp->type == ICMP_ECHOREPLY && r_icmp->un.echo.id == pid) {
                    received++;
                    printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n",
                           (int)(n - iphdrlen - (int)sizeof(struct icmphdr)),
                           inet_ntoa(reply_addr.sin_addr),
                           ntohs(r_icmp->un.echo.sequence),
                           ip->ttl, ms);
                }
            }
        } else {
            printf("Request timeout for icmp_seq %d\n", seq);
        }

        sleep(1);
    }

    close(sockfd);

    printf("\n--- %s ping statistics ---\n", target);
    printf("%d packets transmitted, %d received, %.0f%% packet loss\n",
           transmitted, received,
           ((transmitted - received) / (float)transmitted) * 100.0);

    return 0;
}
