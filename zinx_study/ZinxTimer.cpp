#include "ZinxTimer.h"
#include <sys/timerfd.h>
#include <unistd.h>
//������ʱ���ļ�������
bool ZinxTimerChannel::Init()
{

    bool bRet = false;
    //�����ļ�������
    int iFd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (0 <= iFd)
    {
        //���ö�ʱ����
        struct itimerspec period = {
            {1, 0}, {1, 0}
        };
        if (0 == timerfd_settime(iFd, 0, &period, NULL))
        {
            bRet = true;
            m_Timerfd = iFd;
        }
        
    }
    
    return bRet;
}

//��ȡ��ʱ����
bool ZinxTimerChannel::ReadFd(std::string& _input)
{
    bool bRet = false;;
    char buff[8] = { 0 };

    if (sizeof(buff) == read(m_Timerfd, buff, sizeof(buff)))
    {
        bRet = true;
        _input.assign(buff, sizeof(buff));
    }
    return bRet;
}

bool ZinxTimerChannel::WriteFd(std::string& _output)
{
    return false;
}

//�ر��ļ�������
void ZinxTimerChannel::Fini()
{
    close(m_Timerfd);
    m_Timerfd = -1;
}

//���ض�ȡ�Ķ�ʱ���ļ�������
int ZinxTimerChannel::GetFd()
{
    return m_Timerfd;
}

std::string ZinxTimerChannel::GetChannelInfo()
{
    return "TimerFd";
}

class output_hello : public AZinxHandler {
    // ͨ�� AZinxHandler �̳�
    virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
    {
        auto pchannel = ZinxKernel::Zinx_GetChannel_ByInfo("stdout");
        std::string output = "hello world";
        ZinxKernel::Zinx_SendOut(output, *pchannel);
        return nullptr;
    }
    virtual AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override
    {
        return nullptr;
    }
} *pout_hello = new output_hello();



//���ش���ʱ�¼��Ķ���
AZinxHandler* ZinxTimerChannel::GetInputNextStage(BytesMsg& _oInput)
{
    return &TimerOutMng::GetInstance();
}




//���õ���
TimerOutMng TimerOutMng::single;


TimerOutMng::TimerOutMng()
{
    //����10������
    for (int i = 0; i < WHELLSIZE; i++)
    {
        std::list<TimerOutProc *> tmp;
        m_timer_wheel.emplace_back(tmp);
    }
}

IZinxMsg* TimerOutMng::InternelHandle(IZinxMsg& _oInput)
{
    //��Ϊ��ʱ���᷵�� �����ϴν��� ��ʱ���ٴ� 
    //�������ǽ���ʱ������¼���� ��ֹ��Ϊ��������ʱ������ 
    //û�н��յ���ʱ����ʱ�ź�
    uint64_t iTimeOutCout = 0;
    GET_REF2DATA(BytesMsg, oBytes, _oInput);
    oBytes.szData.copy((char*)&iTimeOutCout, sizeof(iTimeOutCout), 0);

    while (iTimeOutCout-- > 0)
    {
        //�ƶ��̶� 
        cur_index = (++cur_index) % WHELLSIZE;
        std::list<TimerOutProc*> m_cache;
        //������ǰ�̶ȵ����нڵ� ��Ȧ���Ļ� -1 ����ִ�д�����
        //for (auto task : m_timer_wheel[cur_index])
        //����ʹ��for(auto) ��Ϊ ����ʱ�� ��Ҫ����ʱ��ɾ��Ȼ���������һ��λ��
        // �����ᵼ�µ�������Ч  ��Ҫ��¼listԭ���ĳ��������� ��ֹ������ʧЧ
        for (auto itr = m_timer_wheel[cur_index].begin(); itr != m_timer_wheel[cur_index].end();)
        {
            if ((*itr)->iCount <= 0)
            {
                //����ֱ�����ж�ʱ���Ĵ����� ���ڰ�ȫ����
                //(*itr)->Proc() : TimerOutMng::GetInstance().DelTask(this); ɾ�����Լ�
                //�������ᵼ�� �������ĵ�����ȫ��ʧЧ ֱ�ӱ���
                //������� ���ݻ����˼·   ���Խ���ʱ�Ķ�ʱ�� ����һ�������� �ȵ��������һ�� ����ִ��
                //(*itr)->Proc();
                m_cache.emplace_back(*itr);
                auto ptmp = *itr;
                itr = m_timer_wheel[cur_index].erase(itr);
                //ɾ���������� ����ֵ�����ʧЧ ��Ϊ����֪����һ��������
                //��erase �᷵����һ����Ч��������ֵ
                //�������ǲ��ò���  ������ֵ��itr����

                AddTask(ptmp);
            }
            else {
                (*itr)->iCount--;
                ++itr;
            }
        }

        //ͳһ�����γ�ʱ����
        for (auto& task : m_cache)
        {
            task->Proc();
        }
    }
    
    return nullptr;
}

AZinxHandler* TimerOutMng::GetNextHandler(IZinxMsg& _oNextMsg)
{
    return nullptr;
}

void TimerOutMng::AddTask(TimerOutProc* _ptask)
{
    //���㵱ǰ������Ҫ�ŵ��ĸ�������
    int index = (_ptask->GetTimeSec() + cur_index )% WHELLSIZE;
    //�������ŵ��ó�����
    m_timer_wheel[index].emplace_back(_ptask);
    //��������Ȧ��
    _ptask->iCount =  _ptask->GetTimeSec() / WHELLSIZE;

    /*m_task_list.emplace_back(_ptask);
    _ptask->iCount = _ptask->GetTimeSec();*/
}

void TimerOutMng::DelTask(TimerOutProc* _ptask)
{
    // ����ʱ�������г��� ɾ������
    //m_task_list.remove(_ptask);

     // Ĭ�Ͽ����ǵ� &
    for (auto& chi : m_timer_wheel)
    {
        for (auto task : chi)
        {
            if (task == _ptask)
            {
                chi.remove(_ptask);
                break;
            }
        }
    }
}
