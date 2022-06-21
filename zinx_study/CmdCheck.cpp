#include "CmdCheck.h"
#include "CmdMsg.h"
#include "EchoRole.h"

CmdCheck* CmdCheck::poSingle = new CmdCheck();

CmdCheck::CmdCheck()
{
}

CmdCheck::~CmdCheck()
{
}

UserData* CmdCheck::raw2request(std::string _szInput)
{
    //原始数据转换为业务数据 
    if ("exit" == _szInput)
    {
        ZinxKernel::Zinx_Exit();
        return nullptr;
    }

    //赋值原始数据字符串到用户数据中字符串字段
    CmdMsg* pret = new CmdMsg();
    
    if ("open" == _szInput)
    {
        pret->isCmd = true;
        pret->isOpen = true;
    }
    if ("close" == _szInput)
    {
        pret->isCmd = true;
        pret->isOpen = false;
    }
    if ("date" == _szInput)
    {
        pret->needDatePre = true;
        pret->isCmd = true;
     }
    if ("cleardate" == _szInput)
    {
        pret->needDatePre = false;
        pret->isCmd = true;
    }
    
    pret->szUserData = _szInput;    
    return pret;
}

std::string* CmdCheck::response2raw(UserData& _oUserData)
{
    //业务数据转换未原始数据
    //数据转换 将父类转换为子类
    GET_REF2DATA(CmdMsg, output, _oUserData);
    return new std::string(output.szUserData);
}

Irole* CmdCheck::GetMsgProcessor(UserDataMsg& _oUserDataMsg)
{

    szOutChannel = _oUserDataMsg.szInfo;
    if ("stdin" == szOutChannel)
    {
        szOutChannel = "stdout";
    }
    //根据命令不同 交给不同的处理role对象

    //Zinx_GetAllRole获取当前zinxkernel中所有的角色对象
    auto rolelist =  ZinxKernel::Zinx_GetAllRole();

    auto pCmdMsg = dynamic_cast<CmdMsg*>(_oUserDataMsg.poUserData);

    
    //读取当前消息是否是命令
    bool isCmd = pCmdMsg->isCmd;

    Irole* pRetRole = nullptr;


    for (Irole* prole : rolelist)
    {
        if (isCmd)
        {
            auto pOutCtrl = dynamic_cast<OutputCtrl*>(prole);
            if (nullptr != pOutCtrl)
            {
                // 动态类型转换成功 --> 当前 prole 就是 OutputCtrl* 子类对象
                //因为dynamic_cast转换失败 就会返回nullptr
                pRetRole = pOutCtrl;
                break;
            }
        }
        else
        {
            auto pEcho = dynamic_cast<DatePreRole*>(prole);
            // 动态类型转换成功 --> 当前 prole 就是 EchoRole* 子类对象
            if (nullptr != pEcho)
            {
                pRetRole = pEcho;
                break;
            }
        }
        
    }

    return pRetRole;
}

Ichannel* CmdCheck::GetMsgSender(BytesMsg& _oBytes)
{
    //指定数据要通过的标准输出通道
    //可以通过字符串名称指定通道

    /*
    BytesMsg 类型中有
    std::string szInfo;
    std::string szData;

    szData 是 数据
    而 szInfo 是 通道消息
    */
    //return ZinxKernel::Zinx_GetChannel_ByInfo("stdout");
    return ZinxKernel::Zinx_GetChannel_ByInfo(szOutChannel);
}
