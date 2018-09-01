// Copyright tang.  All rights reserved.
// https://github.com/tangyibo/libolptnet
//
// Use of this source code is governed by a BSD-style license
//
// Author: tang (inrgihc@126.com)
// Data : 2018/5/2
// Location: beijing , china
/////////////////////////////////////////////////////////////
#include "net/tcp_socket_addr.h"
#include "net/service_loop.h"
#include "acceptor_server.h"
#define LOG4Z_FORMAT_INPUT_ENABLE
#include "log4z.h"

using namespace olptnet;

int main(int argc, char *argv[])
{
    zsummer::log4z::ILog4zManager::getRef().config("../log4z.properties");
    zsummer::log4z::ILog4zManager::getRef().start();
    LOGFMT_TRACE(LOG4Z_MAIN_LOGGER_ID, "start server ....");

    SocketAddress endpoint("127.0.0.1", 6543);
    ServiceLoop loop;
    acceptor_server acceptor(&loop);

    acceptor.start_thread_pool(4);
    if (0 == acceptor.open(endpoint))
    {
        loop.run();
    }

    LOGFMT_TRACE(LOG4Z_MAIN_LOGGER_ID, "stop server ....");

    return 0;
}