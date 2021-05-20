#pragma execution_character_set("utf-8")

//#include "CircuitBoardInspection.h"
#include "WindowClass.h"
#include <QtWidgets/QApplication>

#include "et199_64.h"

// 验证加密阶段
// 密码:tongxint704，需要重新使用DrvSet初始化-客户号/ATR

void VerifyEncryption() {
	DWORD dwRet;
	DWORD dwCount = 0;	// 锁枚举
	ET_CONTEXT *pContext;

	// 枚举ET199并返回数量
	dwRet = ETEnum(NULL, &dwCount);
	if (dwRet != ET_E_INSUFFICIENT_BUFFER && dwRet) {
		// QMessageBox::critical(NULL, "错误", "找不到指定的加密锁！[ERROR CODE: 1]", QMessageBox::Yes);
		QMessageBox::critical(NULL, "错误", "找不到指定的加密锁！", QMessageBox::Yes);
		exit(0);
	}
	//根据数量分配内存用于存储CONTEXT结构数组
	pContext = new ET_CONTEXT[dwCount];
	//清0缓冲区
	memset(pContext, 0, sizeof(ET_CONTEXT)*dwCount);
	//使用分配的缓冲区重新进行枚举
	dwRet = ETEnum(pContext, &dwCount);
	if (dwRet) {
		// QMessageBox::critical(NULL, "错误", "找不到指定的加密锁！[ERROR CODE: 2]", QMessageBox::Yes);
		QMessageBox::critical(NULL, "错误", "找不到指定的加密锁！", QMessageBox::Yes);
		delete[] pContext;
		exit(0);
	}
	printf("Find ET199 %d\n", dwCount);
	if (dwCount < 1) {
		// QMessageBox::critical(NULL, "错误", "找不到指定的加密锁！[ERROR CODE: 3]", QMessageBox::Yes);
		QMessageBox::critical(NULL, "错误", "找不到指定的加密锁！", QMessageBox::Yes);
		delete[] pContext;
		exit(0);
	}
	//打开ET199
	dwRet = ETOpen(&pContext[0]);
	if (dwRet) {
		QMessageBox::critical(NULL, "错误", "加密锁打开异常！", QMessageBox::Yes);
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
	if (pContext[0].dwCustomer != 0xAAC25F0C) // 这是根据密码生成的客户号
	{
		QMessageBox::critical(NULL, "错误", "加密锁验证失败！", QMessageBox::Yes);
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
	// 锁验证
	VerifyEncryption();

	// 初始化相机
	MyCamera *myCamera = new MyCamera();
	if (!myCamera->cameraInit()) {
		QMessageBox::warning(NULL, "警告", "相机初始化失败！", QMessageBox::Yes);
		exit(0);
	}
	// 初始化数据库
	MysqlSetting *mysql_conn = new MysqlSetting();
	if (!mysql_conn->mysqlInit()) {
		QMessageBox::warning(NULL, "警告", "数据库初始化失败！", QMessageBox::Yes);
		exit(0);
	}
	//// 初始化串口
	//RS232Communication *rs232 = new RS232Communication();
	//if (!rs232->RS232Init()) {
	//	QMessageBox::warning(NULL, "警告", "串口初始化失败！", QMessageBox::Yes);
	//	exit(0);
	//}
	
	QMessageBox::information(NULL, "提示", "初始化成功！", QMessageBox::Yes);
	
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
