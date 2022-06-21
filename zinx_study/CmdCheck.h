#pragma once

#include <zinx.h>


class CmdCheck :
    public Iprotocol
{
private:
    CmdCheck();
    virtual ~CmdCheck();

    //单例模型
    static CmdCheck* poSingle;
public:
    // 通过 Iprotocol 继承
     //原始数据转换为业务数据 
    virtual UserData* raw2request(std::string _szInput) override;

    //业务数据转换为原始数据
    virtual std::string* response2raw(UserData& _oUserData) override;


    virtual Irole* GetMsgProcessor(UserDataMsg& _oUserDataMsg) override;


    virtual Ichannel* GetMsgSender(BytesMsg& _oBytes) override;


    static CmdCheck *GetInstance() {
        return poSingle;
    }

    std::string szOutChannel;
};

