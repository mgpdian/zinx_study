#include "EchoRole.h"
#include "CmdMsg.h"
#include "CmdCheck.h"
bool EchoRole::Init()
{
    return true;
}

UserData* EchoRole::ProcMsg(UserData& _poUserData)
{
    //����Ҫ���Ե�����
    //GET_REF2DATA(CmdMsg, input, _poUserData);
    //std::string szData = input.szUserData;
    
    //д��ȥ
    GET_REF2DATA(CmdMsg, input, _poUserData);
    CmdMsg* pout = new CmdMsg(input);
    ZinxKernel::Zinx_SendOut(*pout,*(CmdCheck::GetInstance()) );

    return nullptr;
}

void EchoRole::Fini()
{
}




bool OutputCtrl::Init()
{
    Irole* pRetRole = NULL;
    //������һ���������� Date ����
    for (auto pRole : ZinxKernel::Zinx_GetAllRole())
    {
        auto pEcho = dynamic_cast<DatePreRole*>(pRole);

        if (nullptr != pEcho)
        {
            pRetRole = pEcho;
            break;
        }
    }
    if (nullptr != pRetRole)
    {
        ///*������һ�������ߺ�����
        //�����߿��Ե��øú�����������ʱ�����̽��в�ּ޽�*/
        //����ͨ��������� ֱ�ӵȵ����ݴ�����ɺ� 
        //�����ص����ݴ��ݸ�ָ�������ݴ�����
        OutputCtrl::SetNextProcessor(*pRetRole);
    }
    return true;
}

UserData* OutputCtrl::ProcMsg(UserData& _poUserData)
{
    //��ȡ�û����õĿ��ر�־
    GET_REF2DATA(CmdMsg, input, _poUserData);

    //��������
    if (true == input.isOpen)
    {
        // �������� open ״̬�ٴ����� open �ᷢ���δ���
        //��������ж�
        if (nullptr != pOut)
        {
            //�����
            
            
            ZinxKernel::Zinx_Add_Channel(*(pOut));
            pOut = nullptr;
        }
        
    }
    else {
        //�����
        // ͬ������ close ״̬�ٴ����� close Ҳ�ᷢ���δ���
        auto pchannel = ZinxKernel::Zinx_GetChannel_ByInfo("stdout");
        if (nullptr != pchannel)
        {
            ZinxKernel::Zinx_Del_Channel(*(pchannel));
            pOut = pchannel;
        }
        
    }
    
    return new CmdMsg(input);
}

void OutputCtrl::Fini()
{
}

bool DatePreRole::Init()
{
    Irole* pRetRole = NULL;
    //������һ����������Echo����
    for (auto pRole : ZinxKernel::Zinx_GetAllRole())
    {
        auto pEcho = dynamic_cast<EchoRole*>(pRole);

        if (nullptr != pEcho)
        {
            pRetRole = pEcho;
            break;
        }
    }
    if (nullptr != pRetRole)
    {
        DatePreRole::SetNextProcessor(*pRetRole);
    }
    
    return true;
}

UserData* DatePreRole::ProcMsg(UserData& _poUserData)
{
    //����û����������� ��ı��־λ
    CmdMsg* pret = nullptr;
    GET_REF2DATA(CmdMsg, input, _poUserData);
    if (input.isCmd)
    {
        needAdd = input.needDatePre;
    }
    else
        //�������Ҫ���Ե����� -- ����ַ������������Թ��ܶ���
    {
        pret = new CmdMsg(input);
        if (needAdd)
        {
            time_t tmp;
            time(&tmp);
            std::string szNew = std::string(ctime(&tmp)) + input.szUserData;
            pret->szUserData = szNew;
        }
        
    }

        
    return pret;
}

void DatePreRole::Fini()
{
}
