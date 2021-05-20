#pragma execution_character_set("utf-8")

//#include "CircuitBoardInspection.h"
#include "WindowClass.h"
#include <QtWidgets/QApplication>

#include "et199_64.h"

// ��֤���ܽ׶�
// ����:tongxint704����Ҫ����ʹ��DrvSet��ʼ��-�ͻ���/ATR

void VerifyEncryption() {
	DWORD dwRet;
	DWORD dwCount = 0;	// ��ö��
	ET_CONTEXT *pContext;

	// ö��ET199����������
	dwRet = ETEnum(NULL, &dwCount);
	if (dwRet != ET_E_INSUFFICIENT_BUFFER && dwRet) {
		// QMessageBox::critical(NULL, "����", "�Ҳ���ָ���ļ�������[ERROR CODE: 1]", QMessageBox::Yes);
		QMessageBox::critical(NULL, "����", "�Ҳ���ָ���ļ�������", QMessageBox::Yes);
		exit(0);
	}
	//�������������ڴ����ڴ洢CONTEXT�ṹ����
	pContext = new ET_CONTEXT[dwCount];
	//��0������
	memset(pContext, 0, sizeof(ET_CONTEXT)*dwCount);
	//ʹ�÷���Ļ��������½���ö��
	dwRet = ETEnum(pContext, &dwCount);
	if (dwRet) {
		// QMessageBox::critical(NULL, "����", "�Ҳ���ָ���ļ�������[ERROR CODE: 2]", QMessageBox::Yes);
		QMessageBox::critical(NULL, "����", "�Ҳ���ָ���ļ�������", QMessageBox::Yes);
		delete[] pContext;
		exit(0);
	}
	printf("Find ET199 %d\n", dwCount);
	if (dwCount < 1) {
		// QMessageBox::critical(NULL, "����", "�Ҳ���ָ���ļ�������[ERROR CODE: 3]", QMessageBox::Yes);
		QMessageBox::critical(NULL, "����", "�Ҳ���ָ���ļ�������", QMessageBox::Yes);
		delete[] pContext;
		exit(0);
	}
	//��ET199
	dwRet = ETOpen(&pContext[0]);
	if (dwRet) {
		QMessageBox::critical(NULL, "����", "���������쳣��", QMessageBox::Yes);
		ETClose(&pContext[0]);
		delete[] pContext;
		exit(0);
	}
	//printf("Open ET199: \n");
	//printf("COS Version: %d.%02d\n", pContext[0].dwVersion >> 8 & 0xff, pContext[0].dwVersion & 0xff);
	//printf("HardSerial: ");
	//for (int i = 0; i<8; i++) printf("%02X", pContext[0].bID[i]);
	//printf("\n");
	printf("ATR:%s\n", pContext[0].bAtr);
	printf("Customer:%X\n", pContext[0].dwCustomer);
	if (pContext[0].dwCustomer != 0xAAC25F0C) // ���Ǹ����������ɵĿͻ���
	{
		QMessageBox::critical(NULL, "����", "��������֤ʧ�ܣ�", QMessageBox::Yes);
		ETClose(&pContext[0]);
		delete[] pContext;
		exit(0);
	}
	ETClose(&pContext[0]);
	delete[] pContext;
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	// ����֤
	VerifyEncryption();

	// ��ʼ�����
	MyCamera *myCamera = new MyCamera();
	if (!myCamera->cameraInit()) {
		QMessageBox::warning(NULL, "����", "�����ʼ��ʧ�ܣ�", QMessageBox::Yes);
		exit(0);
	}
	// ��ʼ�����ݿ�
	MysqlSetting *mysql_conn = new MysqlSetting();
	if (!mysql_conn->mysqlInit()) {
		QMessageBox::warning(NULL, "����", "���ݿ��ʼ��ʧ�ܣ�", QMessageBox::Yes);
		exit(0);
	}
	//// ��ʼ������
	//RS232Communication *rs232 = new RS232Communication();
	//if (!rs232->RS232Init()) {
	//	QMessageBox::warning(NULL, "����", "���ڳ�ʼ��ʧ�ܣ�", QMessageBox::Yes);
	//	exit(0);
	//}
	
	QMessageBox::information(NULL, "��ʾ", "��ʼ���ɹ���", QMessageBox::Yes);
	
	//CircuitBoardInspection w(myCamera);
	WindowClass w(myCamera, mysql_conn);
    w.show();
    a.exec();

	myCamera->cameraClose();
	std::cout << "Delete My camera." << std::endl;
	delete myCamera;

	std::cout << "Delete My mysql_conn." << std::endl;
	delete mysql_conn;

	//rs232->RS232Close();
	//std::cout << "Delete My rs232." << std::endl;
	//delete rs232;

	return 1;
}
