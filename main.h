#ifndef MAIN_H
#define MAIN_H
#include<iostream>
#include<iomanip>
#include<QtDebug>
#include<QObject>
#include <QTextCodec>

using namespace std;

class ImgInfo{
private:
    int Width;
    int Height;
    const char *FilePath;
    int BandCount;
    int BuildPixel;
    int TotalPixel;
    int Bit;
    int Depth;
    const char*ProjRef;
    double Posi_UL_x, Posi_UL_y;
    double Pixel_Width, Pixel_Height;
public:
    void SetImgInfo(int width, int height, const char *path, int bandnum, int buildpixel, int depth, int bit, const char *ref, double x, double y, double pw, double ph){
        this->Width = width;
        this->Height = height;
        this->FilePath = path;
        this->BandCount = bandnum;
        this->BuildPixel = buildpixel;
        this->TotalPixel = width*height;
        this->Depth = depth;//位深度
        this->Bit = bit;//字节数
        this->Posi_UL_x = x;
        this->Posi_UL_y = y;
        this->Pixel_Width = pw;
        this->Pixel_Height = ph;
        this->ProjRef = ref;
    }
    void PrintInfo(){
        //  QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK").toLocal8Bit().data());
        float level = BuildPixel*100.0 / TotalPixel;
        qDebug()<< "-------------------------------------------";
        qDebug()<<"图像路径:"<< FilePath;
        qDebug()<<"图像宽度:"  << Width ;
        qDebug()<<"图像长度:"  << Height ;
        qDebug()<<"  位深度:"  << Depth ;
        qDebug()<<"图像字节:"  << Bit ;
        qDebug()<<"波段总数:"  << BandCount ;
        qDebug()<<"建筑像素:"  << BuildPixel ;
        qDebug()<<"  总像素:"  << TotalPixel ;
        qDebug()<<"建筑占比:"  << level<< "%" ;
        qDebug()<<"左上坐标:"  << "("<< Posi_UL_x<< ","<< Posi_UL_y<< ")" ;
        qDebug()<<"横分辨率:"  << Pixel_Width ;
        qDebug()<<"纵分辨率:"  << Pixel_Height ;
        qDebug()<< "-------------------------------------------" ;
    }
    int GetWidth(){
        return this->Width;
    }
    int GetHeight(){
        return this->Height;
    }
    const char* GetRef(){
        return this->ProjRef;
    }
    double GetPx(){
        return this->Posi_UL_x;
    }
    double GetPy(){
        return this->Posi_UL_y;
    }
    double Getpw(){
        return this->Pixel_Width;
    }
    double Getph(){
        return this->Pixel_Height;
    }
};

class Point{
public:
    int x;
    int y;
    void Init(int xx, int yy){
        x = xx;
        y = yy;
    }
};

class PointF{
public:
    double x;
    double y;
    void Init(double xx, double yy){
        x = xx;
        y = yy;
    }
};
#endif // MAIN_H
