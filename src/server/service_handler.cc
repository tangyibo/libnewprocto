// Copyright tang.  All rights reserved.
// https://github.com/tangyibo/libolptnet
//
// Use of this source code is governed by a BSD-style license
//
// Author: tang (inrgihc@126.com)
// Data : 2018/5/2
// Location: beijing , china
/////////////////////////////////////////////////////////////
#include "service_handler.h"
#include "acceptor_server.h"
#include <iostream>
#include "protocol/packet_message.h"
#define LOG4Z_FORMAT_INPUT_ENABLE
#include "log4z.h"
#include <assert.h>

using namespace olptnet;

void service_handler::on_connect(const SocketAddress &remote)
{
    remote_=remote;
    
    //std::cout<<"Connect:"<<remote.to_string().c_str()<<std::endl;
    LOGFMT_TRACE(LOG4Z_MAIN_LOGGER_ID, "Connect client[%s] and wait receive data ...",remote.to_string().c_str());
}

int service_handler::on_read(const input_stream_buffer &input)
{
    std::string msg=input.to_string();

    request_message req;
    int ret = req.decode(msg);
    if(0!=ret)
    {
        if (1 == ret) 
            return 0;
    
        return req.get_header_size();
    }

    assert(req.isRequest());
    
    response_message resp;
    resp.txid(req.txid());
    resp.status(0);
    
    std::string param;
    packet_message::param_list_type::iterator it;
    for(it=req.param_list().begin();it!=req.param_list().end();++it)
    {
        resp.paramSet(it->first,it->second);
        param.append(it->first).append("=").append(it->second).append(",");
    }
    param=param.substr(0,param.size()-1);
    LOGFMT_TRACE(LOG4Z_MAIN_LOGGER_ID, "Call:%s:%s(%s)",req.service().c_str(),req.method().c_str(),param.c_str());
    
    std::string packet;
    resp.encode(packet);
    send(packet);

    return req.get_total_size();
}

void service_handler::on_close()
{
    acceptor_->remove_handler(this);
    //std::cout<<"Close...:"<<std::endl;
    LOGFMT_TRACE(LOG4Z_MAIN_LOGGER_ID, "Disconnect with client[%s] ",remote_.to_string().c_str());
}
