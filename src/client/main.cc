// Copyright tang.  All rights reserved.
// https://github.com/tangyibo/libolptnet
//
// Use of this source code is governed by a BSD-style license
//
// Author: tang (inrgihc@126.com)
// Data : 2018/5/2
// Location: beijing , china
/////////////////////////////////////////////////////////////
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "net/net_base.h"
#include "protocol/packet_message.h"

using namespace olptnet;

class tcp_connector {
public:

    tcp_connector()
    : sock_fd_(-1) {
    }

    bool connect(const char *ipaddr, const short port) {
        sock_fd_ = net_tcp_connect(ipaddr, port);
        return sock_fd_ > 0;
    }

    ssize_t send(const char *buf, const size_t len) {
        return ::send(sock_fd_, buf, len, 0);
    }

    ssize_t recv(char *buf, const size_t len) {
        return ::recv(sock_fd_, buf, len, 0);
    }

    void close() {
        ::close(sock_fd_);
    }

private:
    int sock_fd_;
};

int main(int argc, char *argv[]) {
    tcp_connector client;
    if (client.connect("127.0.0.1", 6543)) {
        do {
            request_message req;
            req.txid(1);
            req.service("test");
            req.method("ping");
            req.paramSet("p1", "param1");
            req.paramSet("p2", "param2");
            req.paramSet("p3", 2);

            std::string encodestr;
            req.encode(encodestr);

            client.send(encodestr.c_str(), encodestr.size());
            std::cout << "send data..." << std::endl;

            response_message resp;
            std::string msg;
            do {
                char buf[1024];
                int len = client.recv(buf, sizeof (buf));
                if (len > 0) {
                    msg.append(buf, len);
                    int ret = resp.decode(msg);
                    if (0 != ret) {
                        if (-1 == ret) {
                            std::cout << "decode data error:"<<resp.errmsg().c_str() << std::endl;
                            exit(0);
                        }
                        
                        continue;
                    }
                    
                    break;
                } else {
                    std::cout << "recv data error!" << std::endl;
                    exit(0);
                }

            } while (1);

            std::cout << "Status:" << resp.status() << std::endl;
            std::cout << "P1:" << (std::string)resp.wantString(std::string("p1")) << std::endl;
            std::cout << "P2:" << (std::string)resp.wantString(std::string("p2")) << std::endl;
            std::cout << "P3:" << (int)resp.wantInt(std::string("p3")) << std::endl;

            sleep(2);
        } while (1);

        client.close();
    }

    return 0;

}