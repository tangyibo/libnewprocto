#include "protocol/packet_message.h"
#include <iostream>
#include <assert.h>

using namespace olptnet;

void request_unit_test()
{
    request_message rm;
    rm.txid(1);
    rm.service("test");
    rm.method("ping");
    rm.paramSet("p1","param1");
    rm.paramSet("p2","param2");
    rm.paramSet("p3",2);
    
    std::string encodestr;
    rm.encode(encodestr);
    
    request_message rcv;
    int ret=rcv.decode(encodestr);
    assert(0==ret);
    assert(rcv.isRequest());
    std::cout<<"Service:"<<rcv.service()<<std::endl;
    std::cout<<"Method:"<<rcv.method()<<std::endl;
    std::cout<<"P1:"<<rcv.wantString("p1")<<std::endl;
    std::cout<<"P2:"<<rcv.wantString("p2")<<std::endl;
    std::cout<<"P3:"<<rcv.wantInt("p3")<<std::endl;
}

void response_unit_test()
{
    response_message rm;
    rm.txid(1); 
    rm.status(12);
    rm.paramSet("p1","param1");
    rm.paramSet("p2","param2");
    rm.paramSet("p3",2);
    
    std::string encodestr;
    rm.encode(encodestr);
    
    response_message rcv;
    int ret=rcv.decode(encodestr);
    assert(0==ret);
    assert(rcv.isResponse());
    std::cout<<"Status:"<<rcv.status()<<std::endl;
    std::cout<<"P1:"<<rcv.wantString("p1")<<std::endl;
    std::cout<<"P2:"<<rcv.wantString("p2")<<std::endl;
    std::cout<<"P3:"<<rcv.wantInt("p3")<<std::endl;
}


int main(int argc,char *argv[])
{
    std::cout<<"Request:"<<std::endl;
    request_unit_test();
    std::cout<<"Response:"<<std::endl;
    response_unit_test();
}
