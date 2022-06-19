#include <zinx.h>
#include <iostream>

/*����׼���� ���쵽��׼���*/
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

		std::cout << input.szData << std::endl;

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
		return poEcho;
	}
};
int main()
{
	/*1 - ��ʼ�����*/
	//��ʼ�����
	ZinxKernel::ZinxKernelInit();


	/*4-��ͨ��������ӵ����*/
	//�ܵ���ʼ��
	TestStdin* poStdin = new TestStdin();
	//��ӹܵ�
	ZinxKernel::Zinx_Add_Channel(*poStdin);
	//ZinxKernel::Zinx_Add_Channel(*poOut);


	/*5-���п��*/
	ZinxKernel::Zinx_Run();


	//�����Դ�Ļ��� Ҳ���ǽ������
	ZinxKernel::ZinxKernelFini();
	return 0;
}

