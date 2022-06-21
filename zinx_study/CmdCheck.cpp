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
    //ԭʼ����ת��Ϊҵ������ 
    if ("exit" == _szInput)
    {
        ZinxKernel::Zinx_Exit();
        return nullptr;
    }

    //��ֵԭʼ�����ַ������û��������ַ����ֶ�
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
    //ҵ������ת��δԭʼ����
    //����ת�� ������ת��Ϊ����
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
    //�������ͬ ������ͬ�Ĵ���role����

    //Zinx_GetAllRole��ȡ��ǰzinxkernel�����еĽ�ɫ����
    auto rolelist =  ZinxKernel::Zinx_GetAllRole();

    auto pCmdMsg = dynamic_cast<CmdMsg*>(_oUserDataMsg.poUserData);

    
    //��ȡ��ǰ��Ϣ�Ƿ�������
    bool isCmd = pCmdMsg->isCmd;

    Irole* pRetRole = nullptr;


    for (Irole* prole : rolelist)
    {
        if (isCmd)
        {
            auto pOutCtrl = dynamic_cast<OutputCtrl*>(prole);
            if (nullptr != pOutCtrl)
            {
                // ��̬����ת���ɹ� --> ��ǰ prole ���� OutputCtrl* �������
                //��Ϊdynamic_castת��ʧ�� �ͻ᷵��nullptr
                pRetRole = pOutCtrl;
                break;
            }
        }
        else
        {
            auto pEcho = dynamic_cast<DatePreRole*>(prole);
            // ��̬����ת���ɹ� --> ��ǰ prole ���� EchoRole* �������
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
    //ָ������Ҫͨ���ı�׼���ͨ��
    //����ͨ���ַ�������ָ��ͨ��

    /*
    BytesMsg ��������
    std::string szInfo;
    std::string szData;

    szData �� ����
    �� szInfo �� ͨ����Ϣ
    */
    //return ZinxKernel::Zinx_GetChannel_ByInfo("stdout");
    return ZinxKernel::Zinx_GetChannel_ByInfo(szOutChannel);
}
