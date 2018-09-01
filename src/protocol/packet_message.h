#ifndef PACKET_MESSAGE_H
#define PACKET_MESSAGE_H
#include <stdint.h>
#include <string>
#include <map>
#include <sstream>

#define MESSAGE_SIZE_DEFAULT (16*1024*1024)
#define MESSAGE_TYPE_REQUEST  1
#define MESSAGE_TYPE_RESPONSE 2

namespace olptnet{

class packet_message
{
public:

    struct Header
    {
        union
        {
            char bytes[4];
            uint32_t u32;
        } magic; //包开始标记为：{'K', 'X', 'I', 'M'}, 即 0x4b, 0x58, 0x49, 0x4d

        uint8_t verMajor;  //为消息主版本号，总是 0x01
        uint8_t verMinor;  //为消息次版本号，总是 0x01
        uint8_t msgType;   //消息类型，其取值如下表所示1-request;2-response
        uint8_t flag;      //目前没有使用，总是 0x00
        uint32_t bodySize; //每个包到包体最大为16M：
    };
    
    typedef std::map<std::string, std::string> param_list_type;

    packet_message ( );
    virtual ~packet_message ( );

    bool isRequest ( ) const     {    return msg_type_ == MESSAGE_TYPE_REQUEST;   }
    bool isResponse ( ) const    {    return msg_type_ == MESSAGE_TYPE_RESPONSE;    }
    
    uint32_t get_body_size() { return body_size_; }
    uint32_t get_header_size() { return sizeof(Header); }
    uint32_t get_total_size() { return sizeof(Header)+body_size_; }

    virtual int decode(const std::string &data);
    virtual std::string& encode(std::string &packet) = 0;
    
    param_list_type& param_list() { return params_; }
    
    template<typename BasicType>
    void paramSet(const std::string key,const BasicType val)
    {
        std::stringstream ss;
        ss<<val;
        return paramSetString(key,ss.str ());
    }
    
    void paramSetString(const std::string key,const std::string value) 
    { 
        params_[key]=value; 
    }
    
    int wantInt(const std::string &key) 
    {
        std::string value=paramGetString(key);
        std::stringstream ss(value);
        int val;
        ss>>val;
        return val;
    }
    
    float wantFloat(const std::string &key) 
    {
        std::string value=paramGetString(key);
        std::stringstream ss(value);
        float val;
        ss>>val;
        return val;
    }
    
    std::string wantString(const std::string &key) 
    {
        std::string value=paramGetString(key);
        std::stringstream ss(value);
        std::string val;
        ss>>val;
        return val;
    }
    
    std::string paramGetString(const std::string &key) 
    { 
        param_list_type::const_iterator it;
        it=params_.find(key);
        if(it==params_.end())
            return "";
            
        return it->second;
    }
    
    std::string& errmsg() { return errmsg_; }
protected:
    uint8_t msg_type_;
    uint32_t body_size_;
    
    param_list_type params_;

private:
    std::string errmsg_;
};

class request_message:public packet_message
{
public:
    request_message();
    virtual ~request_message();
    
    int64_t txid() { return txid_; }
    void txid(int64_t id) { txid_=id; }
    
    void service (const std::string s) { service_=s; }
    std::string service() { return service_; }
    
    void method(const std::string m) { method_=m; }
    std::string method() { return method_; }
    
    virtual int decode(const std::string &data);         //server use
    virtual std::string& encode(std::string &packet);    //client use
private:
    int64_t txid_;
    std::string service_;
    std::string method_;
};

class response_message:public packet_message
{
public:
    response_message();
    virtual ~response_message();
    
    int64_t txid() { return txid_; }
    void txid(int64_t id) { txid_=id; }
    
    int32_t status() { return status_; }
    void status(int32_t s) { status_=s; }
    
    virtual int decode(const std::string &data);                    //client use
    virtual std::string& encode(std::string &packet);             //server use

private:
    int64_t txid_;
    int32_t status_;
};

}//end namespace
#endif /* PACKET_MESSAGE_H */

