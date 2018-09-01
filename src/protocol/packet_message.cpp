#include "packet_message.h"
#include <exception>
#include <cstring>
#include "serialize.h"

namespace olptnet{

static union kxi_magic
{
    char bytes[4];
    uint32_t u32;
} valid_magic = {
    {'K', 'X', 'I', 'M'}
};

static packet_message::Header default_header = { {{'K', 'X', 'I', 'M'}}, 1, 1, (uint8_t)-1, 0, (uint32_t)-1 };

packet_message::packet_message()
: msg_type_(-1)
, body_size_(-1)
{

}

packet_message::~packet_message()
{
    params_.clear();
}

int packet_message::decode(const std::string &data)
{
    packet_message::Header hdr;

    if (data.size()< sizeof (packet_message::Header))
    {
        return 1;
    }

    char *p = (char *)data.c_str();
    std::memcpy(&hdr, p, sizeof (hdr));

    if (hdr.magic.u32 != valid_magic.u32)
    {
        errmsg_="Invalid packet header magic";
        return -1;
    }

    if (!(hdr.verMajor == 1 && hdr.verMinor == 1))
    {
        errmsg_="Invalid packet version";
        return -1;
    }

    if (hdr.flag != 0)
    {
        errmsg_="Unknown packet header flag ";
        return -1;
    }

    if (hdr.bodySize > MESSAGE_SIZE_DEFAULT)
    {
        errmsg_="Huge packet bodySize";
        return -1;
    }
    
    if(hdr.msgType<1 || hdr.msgType>2)
    {
         errmsg_="Invalid packet message type";
         return -1;
    }

    if (data.size()< sizeof (packet_message::Header) + hdr.bodySize) {
        return 1;
    }
    
    msg_type_=hdr.msgType;
    body_size_=hdr.bodySize;

    return 0;
}

//////////////////////////////////////////////////////////////////

request_message::request_message() 
:packet_message()
, txid_(0)
, service_()
, method_()
{

}

request_message::~request_message() 
{
}

int request_message::decode(const std::string &data)
{
    int ret=packet_message::decode(data);
    if(0!=ret) return ret;
 
    char* p=(char *) data.c_str() + sizeof (packet_message::Header);
    size_t size=data.size() - sizeof (packet_message::Header);
    std::string body(p,size);

    try {
        InStream is(body);
        is >> txid_ >> service_ >> method_>>params_;
    } catch (std::exception &e) {
        return -1;
    }

    return 0;
}


std::string& request_message::encode(std::string &packet) {
    OutStream os;
    os << txid_ << service_ << method_ << params_;
    
    std::string body=os.str();
    
    packet_message::Header  hdr=default_header;
    hdr.msgType=MESSAGE_TYPE_REQUEST;
    hdr.flag=0;
    hdr.bodySize=body.size();
    
    packet.clear();
    packet.append((char *)(&hdr),sizeof(packet_message::Header));
    packet.append(body);
    
    return packet;
}

/////////////////////////////////////////////////////////////////////
response_message::response_message() 
:packet_message()
, txid_(0)
, status_(0)
{

}

response_message::~response_message() 
{

}

int response_message::decode(const std::string &data)
{
    int ret=packet_message::decode(data);
    if(0!=ret) return ret;

    char *p=(char *) data.c_str() + sizeof (packet_message::Header);
    size_t size=data.size() - sizeof (packet_message::Header);
    std::string body(p,size);

    try {
        InStream is(body);
        is >> txid_ >> status_>>params_;
    } catch (std::exception &e) {
        return -1;
    }

    return 0;
}

std::string& response_message::encode(std::string &packet) {
    OutStream os;
    os << txid_ << status_ << params_;
    
    std::string body=os.str();
    
    packet_message::Header  hdr=default_header;
    hdr.msgType=MESSAGE_TYPE_RESPONSE;
    hdr.flag=0;
    hdr.bodySize=body.size();
    
    packet.clear();
    packet.append((char *)(&hdr),sizeof(packet_message::Header));
    packet.append(body);
    
    return packet;
}

}//end namespace
