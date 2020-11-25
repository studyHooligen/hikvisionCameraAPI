#include "../cameraGetImage.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <stdlib.h>

using namespace std;

int main(void)
{
    imgData imgNow;
    cameraObeject cameraLocal;
    cameraLocal.deviceNumber = 0;

    //相机初始化
    if(!camera_deviceInit(&cameraLocal))
    {
        std::cout<<"相机初始化错误"<<std::endl;
        return -1;
    }
    //使能视频流捕获
    if(!camera_beginCapture(&cameraLocal))
    {
        std::cout<<"使能视频流捕获错误"<<std::endl;
        return -1;
    }
    
    //初始化图像数据缓冲池
    if(!camera_pixBufferInit(&cameraLocal,&imgNow))
    {
        std::cout<<"缓冲池初始化错误"<<std::endl;
        return -1;
    }

    int imgID = 1;
    string imgPath = "./imageGet/";
    cout<<"存储路径  "<<imgPath<<endl;
    string savePath;
    FILE * imgFile = NULL;
    while (true)
    {
        cout<<"按一次按键存一张图片"<<endl;
        getchar();
        if(!camera_getImage(&cameraLocal,&imgNow))
        {
            std::cout<<"获取图像出错！！！"<<std::endl;
            break;
        }

        savePath = imgPath + std::to_string(imgID)+".bmp";
        imgFile = fopen(savePath.data(),"wb");
        if(imgFile == NULL)
        {
            std::cout<<"打开/创建图片失败"<<std::endl;
            break;
        }
        cout<<"存储图像： "<<savePath<<endl;
        fwrite(imgNow.imageData,
                sizeof(unsigned char),
                imgNow.imageLength,
                imgFile);
        fclose(imgFile);
        imgID++;
    }

    camera_endCapture(&cameraLocal);
    camera_deviceDestroy(&cameraLocal);
    camera_pixBufferFree(&imgNow);
    return 0;
    
}