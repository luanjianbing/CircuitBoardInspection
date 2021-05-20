# CircuitBoardInspection/单工位视觉检查系统
CircuitBoardInspection Version 1.0@20210520

版本:1.0 上传日期:20210520

# View
![image](https://github.com/luanjianbing/CircuitBoardInspection/blob/main/gitImg/4.png)
![image](https://github.com/luanjianbing/CircuitBoardInspection/blob/main/gitImg/3.png)

# Precautions
1、系统一开始会检测验证加密锁(ET199)用户号Customer，该用户号由自定义密码(tongxint704)生成种子转化为16进制码，所以需要在一开始的时候使用DrvSet初始化加密锁的客户号与ATR(用于验证原密码)

2、初始化Camera，未检测到Camera退出系统，型号MV-CE200-10UC(HIKROBOT)

3、初始化数据库、串口(用于外部信号给出)

4、相机Image格式RGB8，打开AutoExposure\AutoGain\EnableGamma

5、建立Model时，需要首先标定Color范围，保证能够生成合适的Mask，具体可以在调试的时候观察mask文件夹下的图像情况，及时修正

6、根据连接板线路情况需要创建自己的Model，分别给出每个Block的Cate、Color和DetectionType

7、建立对应的Model后，在使用时需要首先Load指定模型

8、触发方式为ExternalSignal和MainView上的软件触发按钮

9、调试支持单线程方便看Cmd信息，实际运行支持4线程，初测提高约50%

10、解决方案中包含Setup安装包，更新时需要更新安装包Version

# MoreOver
Not Allowed to be Applied!
