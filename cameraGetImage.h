#ifndef _IMG_PROCESS_H_

#define _IMG_PROCESS_H_

#include <iostream>

#include "./include/MvCameraControl.h"

typedef struct imgData
{
    int sourceLength;   //摄像头采集的原始数据
    unsigned char * sourceData;

    int imageLength;    //图像数据长度
    int imageSize;          //缓冲区总数据长度
    unsigned char * imageData;   //转换后的bmp图像数据
}imgData;

typedef struct cameraObeject
{
    void * handle; //相机句柄
    int deviceNumber;   //设备枚举号
    MVCC_INTVALUE param;        //相机参数
    MV_FRAME_OUT_INFO_EX frameInf;  //帧参数
}cameraObeject;


/*	@brief:	     打印相机参数
 *	@notice:    无
 *	@author:   江榕煜（2020.10.2）
 *	@param:    MV_CC_DEVICE_INFO：相机信息结构体
 *	@retv:	       正常运行or报错
**/
bool PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo);

/*	@brief:	            创建一个可读取的相机对象
 *	@notice:          相机设备号在传入的对象中设置
 *	@author:        江榕煜（2020.10.2）
 *	@param:         cameraObeject指针：相机对象
 *	@retv:	            创建成功or失败
**/
bool camera_deviceInit(cameraObeject * target);

/*	@brief:	        相机开始捕获图像流
 *	@notice:        读取图像前必须调用该函数
 *	@author:    江榕煜（2020.10.2）
 *	@param:     cameraObeject：相机对象
 *	@retv:	        启动成功or失败
**/
bool camera_beginCapture(cameraObeject * target);

/*	@brief:	        像素数据缓冲池初始化
 *	@notice:        需要在相机初始化和完成取流之后调用
 *	@author:    江榕煜（2020.10.2）
 *	@param:     
                target：相机对象
                pixBuf：图像源数据缓冲池对象
 *	@retv:	
**/
bool camera_pixBufferInit(cameraObeject * target,imgData * buffer);

/*	@brief:	        获取一帧图片
 *	@notice:        先调用过创建设备和启动流捕获
 *	@author:    江榕煜（2020.10.2）
 *	@param:     
                target：相机对象
                buffer：图像像素数据
 *	@retv:	        捕获成功or失败
**/
bool camera_getImage(cameraObeject * target,imgData * buffer);

/*	@brief:	        相机停止捕获图像流
 *	@notice:        销毁句柄前必须调用该函数
 *	@author:    江榕煜（2020.10.2）
 *	@param:     cameraObeject：相机对象
 *	@retv:	        停止成功or失败
**/
bool camera_endCapture(cameraObeject * target);

/*	@brief:	        关闭相机
 *	@notice:        
 *	@author:    江榕煜（2020.10.2）
 *	@param:     cameraObeject：相机对象
 *	@retv:	        关闭成功or失败
**/
bool camera_deviceDestroy(cameraObeject * target);

/*	@brief:	        释放图像数据缓存
 *	@notice:        适当使用该函数，保护数据和内存
 *	@author:    江榕煜（2020.10.2）
 *	@param:     
                buffer：图像数据内存指针
 *	@retv:	        释放成功or失败
**/
bool camera_pixBufferFree(imgData * buffer);

#endif // !_IMG_PROCESS_H_
