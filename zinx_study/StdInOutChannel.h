#pragma once
#include <zinx.h>
#include <ZinxTCP.h>


class StdInChannel :
    public Ichannel
{
    // 通过 Ichannel 继承
    virtual bool Init() override;
    virtual bool ReadFd(std::string& _input) override;
    virtual bool WriteFd(std::string& _output) override;
    virtual void Fini() override;
    virtual int GetFd() override;
    virtual std::string GetChannelInfo() override;
    virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override;
};

class StdOutChannel :
    public Ichannel {
    // 通过 Ichannel 继承
    virtual bool Init() override;
    virtual bool ReadFd(std::string& _input) override;
    virtual bool WriteFd(std::string& _output) override;
    virtual void Fini() override;
    virtual int GetFd() override;
    virtual std::string GetChannelInfo() override;
    virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override;
};

/*tcp数据套接字通道类，继承通道类，该类也是一个抽象类，需要开发者继承该类，
重写GetInputNextStage函数以指定读取到的字节流的处理方式*/
class myTcpData : public ZinxTcpData
{
public:
    myTcpData(int _fd) : ZinxTcpData(_fd) {}//继承构造
    // 通过 ZinxTcpData 继承
    virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override;
};


/*产生tcp数据套接字通道类的抽象工厂类，
开发者需要重写CreateTcpDataChannel函数，来返回一个tcp通道对象
一般地，开发者应该同时创建一对tcp通道类和工厂类*/
//class IZinxTcpConnFact
class myFact : public IZinxTcpConnFact
{
    // 通过 IZinxTcpConnFact 继承
    virtual ZinxTcpData* CreateTcpDataChannel(int _fd) override;
};



    /*tcp监听通道类，这是一个实体类（不建议继承该类），开发者可以直接创建tcp监听通道对象，
    一般地，开发者应该在该类的构造函数中，指定一个tcp套接字通道类的工厂类，当有连接到来后，该工厂类的成员方法会被调用*/
    //class ZinxTCPListen : public Ichannel