#include <zinx.h>
#include <iostream>
#include "StdInOutChannel.h"
#include "EchoRole.h"
#include "ZinxTCP.h"
#include "ZinxTimer.h"

/*����׼���� ���쵽��׼���*/
/*
Echo
	���� exit ʱ�������� �˳�����
	���� close �������ԣ� ֱ������ open ֮��������ԣ� �����ڶ�̬����
	���� date ������ǰ׺����	cleardate �ر�
*/
/*������׼���ͨ����*/
class TestStdout : public Ichannel
{
	// ͨ�� Ichannel �̳�
	virtual bool Init() override
	{
		return true;
	}
	virtual bool ReadFd(std::string& _input) override
	{
		return false;
	}
	//���︺�𽫽��յ��������
	virtual bool WriteFd(std::string& _output) override
	{
		std::cout << _output << std::endl;
		return true;
	}
	virtual void Fini() override
	{
	}
	virtual int GetFd() override
	{
		//��׼������ļ�������Ϊ1  STDOUT_FILENO
		// return 1;
		return STDOUT_FILENO;
	}
	virtual std::string GetChannelInfo() override
	{
		return "stdout";
	}
	virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override
	{
		return nullptr;
	}
} *poOut = new TestStdout();


/*2-д���ܴ�����*/
//����д��������
//AZinxHandler ��д������Ϣ�����ͻ�ȡ��һ�������ŷ���
class Echo : public AZinxHandler {
	// ͨ�� AZinxHandler �̳�
	/*��Ϣ����������������д�ú���ʵ����Ϣ�Ĵ���
	������Ҫһ�����ڼ�������ʱ��Ӧ�ô����µ���Ϣ���󣨶Ѷ��󣩲�����ָ��*/
	virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
	{
		/*���쵽��׼���*/

		//GET_REF2DATA ���� ��_oInput ת���� BytesMsg ������, д�� input
		GET_REF2DATA(BytesMsg, input, _oInput);

		 //����µĹ��� �յ�exit������˳�  �����������ֱ�����ж��Ļ� 
		//��Υ������ԭ��  ����������µĹ���
		//˼· ���ﴴ���˳��������ȹ���

		//std::cout << input.szData << std::endl;
		//����һ�����ͨ����  �������������׼���
		ZinxKernel::Zinx_SendOut(input.szData, *poOut);


		return nullptr;
	}

	/*��ȡ��һ�������ں�������������д�ú�����
	����ָ����ǰ�����ڽ�������һ��������ʲô��
	ͨ��������Ϣ���󣬿�����Բ�ͬ������зֱ���*/

	virtual AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override
	{
		return nullptr;
	}
}*poEcho = new Echo();

//exit �˳���Ϣ������ 
//Framework ���
class ExitFramework : public AZinxHandler
{
	// ͨ�� AZinxHandler �̳�
	virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
	{
		GET_REF2DATA(BytesMsg, obyte, _oInput); //����ĵڶ����� �����Լ�����������

		if (obyte.szData == "exit")
		{
			ZinxKernel::Zinx_Exit(); //ZinxKernel��ܵ��˳���
			return nullptr;
		}
		//����������һ�����ڴ��������
		return new BytesMsg(obyte);
	}
	virtual AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override
	{
		return poEcho;
	}
} *poExit = new ExitFramework();

//��Ϣ����ǰ׺ ��������
class AddDate : public AZinxHandler
{
	// ͨ�� AZinxHandler �̳�
	virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
	{
		GET_REF2DATA(BytesMsg, oBytes, _oInput);
		time_t tmp;
		time(&tmp);
		std::string szNew = std::string(ctime(&tmp)) + oBytes.szData;

		BytesMsg* pret = new BytesMsg(oBytes);
		pret->szData = szNew;
		return pret;
	}
	virtual AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override
	{
		return poEcho;
	}
} *poAddDate = new AddDate();



//���ݷַ��������� 
class CmdHandler : public AZinxHandler
{
	bool date_state = false;
	// ͨ�� AZinxHandler �̳�
	virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
	{
		/*�ж������Ƿ���open��close��ִ�в�ͬ����*/
		GET_REF2DATA(BytesMsg, oBytes, _oInput);

		if (oBytes.szData == "open")
		{
			ZinxKernel::Zinx_Add_Channel(*poOut);
			return nullptr;
		}
		else if (oBytes.szData == "close")
		{
			ZinxKernel::Zinx_Del_Channel(*poOut);
			return nullptr;
		}
		else if (oBytes.szData == "date")
		{
			date_state = true;
			return nullptr;
		}
		else if (oBytes.szData == "cleardate")
		{
			date_state = false;

		}

		return new BytesMsg(oBytes);
	}
	virtual AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override
	{
		GET_REF2DATA(BytesMsg, oBytes, _oNextMsg);
		if (oBytes.szData == "exit")
		{
			return poExit;
		}
		else {
			if (date_state == true)
			{
				return poAddDate;
			}
			else {
				return poEcho;
			}
			
		}
		
	}
	//int status = 0;
} *poCmd = new CmdHandler();








///*3-дͨ����*/
//�ܵ���
//������Ϣ��?
//Ichannel ͨ���࣬�����Ի����������࣬�ṩ����ϵͳ���õ������շ�����
//һ��أ��û�Ӧ�ø��ݴ�����ļ�����ϢԴ����ͬ��
//����ͨ����������ѡ�ú��ʵ�ʵ���ࣨ�Ѿ��ṩ��ͨ�������ࣩ�����ϵͳ���ļ�IO
class TestStdin : public Ichannel
{
	// ͨ�� Ichannel �̳�
	/*ͨ����ʼ��������Ӧ����д�������ʵ�ִ��ļ���һ���Ĳ�������
	�ú�������ͨ��������ӵ�zinxkernelʱ������
	Ҳ���� ��ʼ��  Ҫ�����Ϊtrue*/
	virtual bool Init() override
	{
		return true;
	}
	/*��ȡ���ݣ� ������Ӧ�ø���Ŀ���ļ���ͬ����д�����������ʵ�ֽ�fd�е����ݶ�ȡ������_string��
	�ú�����������Դ����Ӧ���ļ������ݵ���ʱ������
	ReadFd  Ϊ ��ȡ��Ϣ Ҳ����������ĵص�*/
	virtual bool ReadFd(std::string& _input) override
	{
		std::cin >> _input;
		return true;
	}
	/*д�����ݣ� ������Ӧ�ý�����д���Ĳ���ͨ����д�ú���ʵ��
	�ú������ڵ���zinxkernel::sendout�����󱻵��ã�ͨ������ֱ�ӵ��ö���ͨ����·���õķ�ѹ���Ƶ��ã�*/
	virtual bool WriteFd(std::string& _output) override
	{
		return false;
	}
	/*ͨ��ȥ��ʼ����������Ӧ���ڸú������������Դ
	�ú�������ͨ�������zinxkernel��ժ��ʱ����
	��Դ����  */
	virtual void Fini() override
	{
	}
	/*��ȡ�ļ������������� ������Ӧ���ڸú������ص�ǰ���ĵ��ļ���
	һ��أ�������Ӧ�����������ж�������������¼��������¼��ǰIO���õ��ļ����ڴ˺�����ֻ�Ƿ��ظ�����
	��ȡ��Ϣ��λ�� �������Ǵ������Ϣ�ص�����Ļ �����ļ���������0  Ҳ���� STDIN_FILENO*/
	virtual int GetFd() override
	{
		//return 0; //���Ҳ����
		return STDIN_FILENO;
	}
	/*��ȡͨ����Ϣ�����������߿����ڸú����з��ظ�ͨ����ص�һЩ�����ַ���������������Һ͹���*/
	virtual std::string GetChannelInfo() override
	{
		return "stdin";
	}
	/*��ȡ��һ�������ڣ�������Ӧ����д�ú�����ָ����һ��������
	һ��أ�������Ӧ���ڸú�������һ��Э��������������ȡ�����ֽ���
	�������  �Ǿ��� ��ȡ������Ϣ��  ������Ҫȥ�Ǹ�������  */
	virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override
	{
		return poCmd;
	}
};


class timerhello : public TimerOutProc
{
	// ͨ�� TimerOutProc �̳�
	virtual void Proc() override
	{
		auto pchannel = ZinxKernel::Zinx_GetChannel_ByInfo("stdout");
		std::string output = "hello world";
		ZinxKernel::Zinx_SendOut(output, *pchannel);
		TimerOutMng::GetInstance().DelTask(this);
	}
	virtual int GetTimeSec() override
	{
		return 3;
	}
};



class timerbye : public TimerOutProc
{
	// ͨ�� TimerOutProc �̳�
	virtual void Proc() override
	{
		auto pchannel = ZinxKernel::Zinx_GetChannel_ByInfo("stdout");
		std::string output = "bye";
		ZinxKernel::Zinx_SendOut(output, *pchannel);
	}
	virtual int GetTimeSec() override
	{
		return 5;
	}
};




int main()
{
	/*1 - ��ʼ�����*/
	//��ʼ�����
	ZinxKernel::ZinxKernelInit();

	TimerOutMng::GetInstance().AddTask(new timerhello());
	TimerOutMng::GetInstance().AddTask(new timerbye());

	/*4-��ͨ��������ӵ����*/
	//�ܵ���ʼ��
	//TestStdin* poStdin = new TestStdin();
	//��ӹܵ�
	//ZinxKernel::Zinx_Add_Channel(*poStdin);
	//ZinxKernel::Zinx_Add_Channel(*poOut);
	ZinxKernel::Zinx_Add_Channel(*(new StdInChannel()));
	ZinxKernel::Zinx_Add_Channel(*(new StdOutChannel()));
	ZinxKernel::Zinx_Add_Channel(*(new ZinxTCPListen(51111, new myFact())));
	ZinxKernel::Zinx_Add_Role(*(new EchoRole()));
	ZinxKernel::Zinx_Add_Role(*(new DatePreRole()));
	ZinxKernel::Zinx_Add_Role(*(new OutputCtrl()));
	ZinxKernel::Zinx_Add_Channel(*(new ZinxTimerChannel()));//������



	/*5-���п��*/
	ZinxKernel::Zinx_Run();


	//�����Դ�Ļ��� Ҳ���ǽ������
	ZinxKernel::ZinxKernelFini();
	return 0;
}

