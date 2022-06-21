#pragma once

#include <zinx.h>


class CmdCheck :
    public Iprotocol
{
private:
    CmdCheck();
    virtual ~CmdCheck();

    //����ģ��
    static CmdCheck* poSingle;
public:
    // ͨ�� Iprotocol �̳�
     //ԭʼ����ת��Ϊҵ������ 
    virtual UserData* raw2request(std::string _szInput) override;

    //ҵ������ת��Ϊԭʼ����
    virtual std::string* response2raw(UserData& _oUserData) override;


    virtual Irole* GetMsgProcessor(UserDataMsg& _oUserDataMsg) override;


    virtual Ichannel* GetMsgSender(BytesMsg& _oBytes) override;


    static CmdCheck *GetInstance() {
        return poSingle;
    }

    std::string szOutChannel;
};

