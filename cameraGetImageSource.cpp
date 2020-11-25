#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "./include/MvCameraControl.h"
#include "cameraGetImage.h"

/*	@brief:	     打印枚举到的相机信息
 *	@notice:    无
 *	@author:   江榕煜（2020.10.2）
 *	@param:    MV_CC_DEVICE_INFO：相机信息结构体
 *	@retv:	       正常运行or报错
**/
bool PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo)
{
    if (NULL == pstMVDevInfo)
    {
        printf("The Pointer of pstMVDevInfo is NULL!\n");
        return false;
    }
    if (pstMVDevInfo->nTLayerType == MV_GIGE_DEVICE)
    {
        int nIp1 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
        int nIp2 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
        int nIp3 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
        int nIp4 = (pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);

        // ch:打印当前相机ip和用户自定义名字 | en:print current ip and user defined name
        printf("Device Model Name: %s\n", pstMVDevInfo->SpecialInfo.stGigEInfo.chModelName);
        printf("CurrentIp: %d.%d.%d.%d\n" , nIp1, nIp2, nIp3, nIp4);
        printf("UserDefinedName: %s\n\n" , pstMVDevInfo->SpecialInfo.stGigEInfo.chUserDefinedName);
    }
    else if (pstMVDevInfo->nTLayerType == MV_USB_DEVICE)
    {
        printf("Device Model Name: %s\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chModelName);
        printf("UserDefinedName: %s\n\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName);
    }
    else
    {
        printf("Not support.\n");
    }

    return true;
}

/*	@brief:	            创建一个可读取的相机对象
 *	@notice:          相机设备号在传入的对象中设置
 *	@author:        江榕煜（2020.10.2）
 *	@param:         cameraObeject指针：相机对象
 *	@retv:	            创建成功or失败
**/
bool camera_deviceInit(cameraObeject * target)
{
     int nRet = MV_OK;

        MV_CC_DEVICE_INFO_LIST stDeviceList;
        memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

        // 枚举设备
        // enum device
        nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
        if (MV_OK != nRet)
        {
            std::cout<<"MV_CC_EnumDevices fail! nRet :"<<nRet<<std::endl;
            return false;
        }
        if (stDeviceList.nDeviceNum > 0)
        {
            std::cout<<"设备枚举成功，信息如下："<<std::endl;
            for (unsigned int i = 0; i < stDeviceList.nDeviceNum; i++)
            {
                std::cout<< "device: "<<i<<std::endl;
                MV_CC_DEVICE_INFO* pDeviceInfo = stDeviceList.pDeviceInfo[i];
                if (NULL == pDeviceInfo)
                {
                    return false;
                } 
                PrintDeviceInfo(pDeviceInfo);            
            } 
            std::cout<<"设备枚举结束"<<std::endl; 
        } 
        else
        {
            std::cout<<"Find No Devices!"<<std::endl;
            return false;
        }

        //打印开启的相机信息
        if(stDeviceList.nDeviceNum >= target->deviceNumber)
        {
            std::cout<<"打开相机(number): "<<target->deviceNumber<<std::endl;
            MV_CC_DEVICE_INFO* pDeviceInfo = stDeviceList.pDeviceInfo[target->deviceNumber];
            PrintDeviceInfo(pDeviceInfo);   
        }
        else
        {
            std::cout<<"open Device Numer Error!"<<std::endl;
            return false;
        }
        

        // 选择设备并创建句柄
        // select device and create handle
        std::cout<<"创建设备句柄"<<std::endl;
        nRet = MV_CC_CreateHandle(&(target->handle), 
                        stDeviceList.pDeviceInfo[target->deviceNumber]);
        if (MV_OK != nRet)
        {
            std::cout<<"MV_CC_CreateHandle fail! nRet:"<<nRet<<std::endl;
           return false;
        }

        // 打开设备
        // open device
        std::cout<<"打开设备"<<std::endl;
        nRet = MV_CC_OpenDevice(target->handle);
        if (MV_OK != nRet)
        {
            std::cout<<"MV_CC_OpenDevice fail! nRet :"<<nRet<<std::endl;
            return false;
        }

        // 设置枚举
        nRet = MV_CC_SetEnumValue(target->handle, "TriggerMode", 0);
        if (MV_OK != nRet)
        {
            printf("MV_CC_SetTriggerMode fail! nRet [%x]\n", nRet);
            return false;
        }

        // ch:获取数据包大小 | en:Get payload size
        memset(&(target->param), 0, sizeof(MVCC_INTVALUE));
        nRet = MV_CC_GetIntValue(target->handle, "PayloadSize", &(target->param));
        if (MV_OK != nRet)
        {
            printf("Get PayloadSize fail! nRet [0x%x]\n", nRet);
            // break;
            return false;
        }

        return true;
}

/*	@brief:	        相机开始捕获图像流
 *	@notice:        读取图像前必须调用该函数
 *	@author:    江榕煜（2020.10.2）
 *	@param:     cameraObeject：相机对象
 *	@retv:	        启动成功or失败
**/
bool camera_beginCapture(cameraObeject * target)
{
    // 启动流
    int nRet = MV_CC_StartGrabbing(target->handle);
    if (MV_OK != nRet)
    {
        printf("MV_CC_StartGrabbing fail! nRet [%x]\n", nRet);
        return false;
    }

    return true;
}

/*	@brief:	        像素数据缓冲池初始化
 *	@notice:        需要在相机初始化和完成取流之后调用
 *	@author:    江榕煜（2020.10.2）
 *	@param:     
                target：相机对象
                pixBuf：图像源数据缓冲池对象
 *	@retv:	
**/
bool camera_pixBufferInit(cameraObeject * target,imgData * buffer)
{
    buffer->sourceLength = target->param.nCurValue;
    buffer->sourceData = new unsigned char [buffer->sourceLength];
    if(buffer->sourceData == NULL)
    {
        std::cout<<"Create buffer Error!"<<std::endl;
        return false;
    }
    int nRet = MV_CC_GetOneFrameTimeout(target->handle, 
                                        (unsigned char *)buffer->sourceData,
                                        buffer->sourceLength,
                                        &(target->frameInf),
                                        1000);
    buffer->imageSize = target->frameInf.nWidth * target->frameInf.nHeight * 4 + 2048;
    buffer->imageData = new unsigned char [buffer->imageSize];
    if(buffer->imageData == NULL)
    {
        std::cout<<"Create buffer Error!"<<std::endl;
        return false;
    }
    return true;
}

/*	@brief:	        获取一帧图片
 *	@notice:        先调用过创建设备和启动流捕获
 *	@author:    江榕煜（2020.10.2）
 *	@param:     
                target：相机对象
                buffer：图像像素数据
 *	@retv:	        捕获成功or失败
**/
bool camera_getImage(cameraObeject * target,imgData * buffer)
{
    int nRet = MV_CC_GetOneFrameTimeout(target->handle, 
                                                                                            (unsigned char *)buffer->sourceData,
                                                                                            buffer->sourceLength,
                                                                                            &(target->frameInf),
                                                                                            1000);
    if(nRet != MV_OK)
    {
        std::cout<<"get image Error!"<<std::endl;
        return false;
    }

    // 填充存图参数
    // fill in the parameters of save image
    MV_SAVE_IMAGE_PARAM_EX stSaveParam;
    memset(&stSaveParam, 0, sizeof(MV_SAVE_IMAGE_PARAM_EX));

    // 从上到下依次是：输出图片格式，输入数据的像素格式，图像宽，
    // 图像高，输入数据缓存，输出图片缓存，提供的输出缓冲区大小，JPG编码质量
    stSaveParam.enImageType = MV_Image_Bmp; 
    stSaveParam.enPixelType = target->frameInf.enPixelType; 
    stSaveParam.nWidth      = target->frameInf.nWidth; 
    stSaveParam.nHeight     = target->frameInf.nHeight; 
    stSaveParam.pData       = (unsigned char *)buffer->sourceData;
    stSaveParam.nDataLen    = buffer->sourceLength;
    stSaveParam.pImageBuffer = (unsigned char *)buffer->imageData;
    stSaveParam.nBufferSize = buffer->imageSize;
    stSaveParam.nJpgQuality = 80;

    nRet = MV_CC_SaveImageEx2(target->handle, &stSaveParam);
    if(MV_OK != nRet)
    {
        printf("failed in MV_CC_SaveImage,nRet[%x]\n", nRet);
        return false;
    }
    buffer->imageLength = stSaveParam.nImageLen;

    return true;
}

/*	@brief:	        相机停止捕获图像流
 *	@notice:        销毁句柄前必须调用该函数
 *	@author:    江榕煜（2020.10.2）
 *	@param:     cameraObeject：相机对象
 *	@retv:	        停止成功or失败
**/
bool camera_endCapture(cameraObeject * target)
{
    int nRet = MV_CC_StopGrabbing(target->handle);
    if (MV_OK != nRet)
    {
        printf("MV_CC_StopGrabbing fail! nRet [%x]\n", nRet);
        return false;
    }
    return true;
}

/*	@brief:	        关闭相机
 *	@notice:        
 *	@author:    江榕煜（2020.10.2）
 *	@param:     cameraObeject：相机对象
 *	@retv:	        关闭成功or失败
**/
bool camera_deviceDestroy(cameraObeject * target)
{
    // 销毁句柄
    // destroy handle
    int nRet = MV_CC_DestroyHandle(target->handle);
    if (MV_OK != nRet)
    {
        printf("MV_CC_DestroyHandle fail! nRet [%x]\n", nRet);
        return false;
    }
    return true;
}

/*	@brief:	        释放图像数据缓存
 *	@notice:        适当使用该函数，保护数据和内存
 *	@author:    江榕煜（2020.10.2）
 *	@param:     
                buffer：图像数据内存指针
 *	@retv:	        释放成功or失败
**/
bool camera_pixBufferFree(imgData * buffer)
{
    delete [] buffer->imageData;
    delete [] buffer->sourceData;

    return true;
}
