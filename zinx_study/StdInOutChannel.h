#pragma once
#include <zinx.h>
#include <ZinxTCP.h>


class StdInChannel :
    public Ichannel
{
    // ͨ�� Ichannel �̳�
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
    // ͨ�� Ichannel �̳�
    virtual bool Init() override;
    virtual bool ReadFd(std::string& _input) override;
    virtual bool WriteFd(std::string& _output) override;
    virtual void Fini() override;
    virtual int GetFd() override;
    virtual std::string GetChannelInfo() override;
    virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override;
};

/*tcp�����׽���ͨ���࣬�̳�ͨ���࣬����Ҳ��һ�������࣬��Ҫ�����߼̳и��࣬
��дGetInputNextStage������ָ����ȡ�����ֽ����Ĵ���ʽ*/
class myTcpData : public ZinxTcpData
{
public:
    myTcpData(int _fd) : ZinxTcpData(_fd) {}//�̳й���
    // ͨ�� ZinxTcpData �̳�
    virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override;
};


/*����tcp�����׽���ͨ����ĳ��󹤳��࣬
��������Ҫ��дCreateTcpDataChannel������������һ��tcpͨ������
һ��أ�������Ӧ��ͬʱ����һ��tcpͨ����͹�����*/
//class IZinxTcpConnFact
class myFact : public IZinxTcpConnFact
{
    // ͨ�� IZinxTcpConnFact �̳�
    virtual ZinxTcpData* CreateTcpDataChannel(int _fd) override;
};



    /*tcp����ͨ���࣬����һ��ʵ���ࣨ������̳и��ࣩ�������߿���ֱ�Ӵ���tcp����ͨ������
    һ��أ�������Ӧ���ڸ���Ĺ��캯���У�ָ��һ��tcp�׽���ͨ����Ĺ����࣬�������ӵ����󣬸ù�����ĳ�Ա�����ᱻ����*/
    //class ZinxTCPListen : public Ichannel