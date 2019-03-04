#include "test.h"
#include "ui_test.h"
#include "gdal.h"
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_srs_api.h"
#include "ogr_spatialref.h"
#include "cpl_string.h"
#include "ogr_api.h"
#include "PROJ/include/proj_api.h"
#include "main.h"
#include <QTextCodec>
#include<cstring>
#include<QtDebug>
#include<QFile>
#include<QFileInfo>
#include<assert.h>
#include<vector>
#include<stack>
#include<list>
using namespace std;

#define Gridecode 80 //分类后的建筑gid
#define PI 3.1415926535
QString inputfile,outputfile;
int AreaID;
ImgInfo imginfo;//图像信息类
vector<vector<int>>isChecked;//像素归属数组,1:有归属；0:无归属
list<Point>::iterator itor;//迭代器
list<Point>Point_Max;
list<Point>::iterator itor_1;//迭代器
stack<Point>convex;//存储凸包点
//函数
list<Point> AreaGrow(int seedx, int seedy);//种子生长为区域
double Distance(Point pb, Point pf);//距离函数
double PolarAngle(Point pb, Point pf);//算极角函数
int Direction(Point p0, Point p1, Point p2);//判断顺逆 1顺 2逆 3长 4短


test::test(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::test)
{
    ui->setupUi(this);
    ui->progressBar->setVisible(FALSE);
    ui->progresstitle->setText("准备就绪");
    int num[5]={100,500,1000,2000,5000};
    QString st;
    for(int i=0;i<5;i++){
        ui->minpixel->addItem(st.setNum(num[i]));
    }
    ui->minpixel->setCurrentText(st.setNum(1000));
}

test::~test()
{
    delete ui;
}

void test::on_inputButton_clicked()
{
    inputfile = QFileDialog :: getOpenFileName(this,tr("Open tif file"), NULL, "*.tif");

    //--打开文件成功
    if (!inputfile.isNull())
    {
        ui->inputfile->setText(inputfile);

        /*  ChooseAreaId(inputfile.toLocal8Bit().data());
        for(int i=0;i<areaid.size();i++){
            QString qst;
            ui->areaid->addItem(qst.setNum(areaid[i]));
        }*/
    }
    else    //---打开文件失败
    {
        QMessageBox ::information(NULL, NULL, "open file error");
    }
}

void test::on_outputButton_clicked()
{
    outputfile = QFileDialog ::getSaveFileName(this, tr("Save shp file"), NULL, "*.shp");

    //--打开文件成功
    if (!outputfile.isNull())
    {
        ui->outputfile->setText(outputfile);
    }
    else    //---打开文件失败
    {
        QMessageBox ::information(NULL, NULL, "save file error");
    }
}

//////////////////////////////////////////////////////////////////////////////////////
void test::Readimg(int buildvalue,const char *filepath){ //选择建筑id
    ui->progressBar->setRange(0,100);
    ui->progressBar->setValue(0);
    ui->progressBar->setVisible(TRUE);
    ui->progresstitle->setText("读取tif影像");
    vector<vector<unsigned long int>>img;//存储图像像素值
    //注册
    GDALAllRegister();
    //以防中文名不能正常读取
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
    GDALDataset *m_pDataset = (GDALDataset*)GDALOpen(filepath, GA_ReadOnly);
    if (m_pDataset == NULL)
    {
        qDebug() <<"指定的文件不能打开!";
    }
    int BandNum = m_pDataset->GetRasterCount();

    //获取数据宽度和高度，（带黑框的大图的）
    int dataWidth = m_pDataset->GetRasterXSize();
    int dataHeight = m_pDataset->GetRasterYSize();

    img.resize(dataHeight, vector<unsigned long int>(dataWidth, 0));
    isChecked.resize(dataHeight, vector<int>(dataWidth, 1));

    /** projRef将会存储test.tif中的坐标信息 **/
    const char *projRef = m_pDataset->GetProjectionRef();
    /** adfGeoTransform存储一些基准点的坐标和每一个pixel表示的长度等 **/
    double adfGeoTransform[6];
    m_pDataset->GetGeoTransform(adfGeoTransform);
    qDebug() << projRef ;

    int bufHeight = dataHeight;
    int bufWidth = dataWidth;
    int bit = GDALGetDataTypeSize(m_pDataset->GetRasterBand(1)->GetRasterDataType()) / 8;//字节数
    int depth = GDALGetDataTypeSize(m_pDataset->GetRasterBand(1)->GetRasterDataType());//位深度

    int totalpixel;
    qDebug()<< "开始读取图像...";
    if (BandNum == 1){
        //获取第1波段的波段指针，参数就是表示第几波段的意思
        GDALRasterBand *pBand = m_pDataset->GetRasterBand(1);
        unsigned long int* m_data = new unsigned long int[bufHeight*bufWidth];
        CPLErr err = pBand->RasterIO(GF_Read, 0, 0, dataWidth, dataHeight, m_data, bufWidth, bufHeight, GDT_UInt32, 0, 0);

        for (int i = 0; i < dataHeight; i++){
            for (int j = 0; j < dataWidth; j++){
                img[i][j] = m_data[i*dataWidth + j];
                if (img[i][j] == buildvalue){
                    isChecked[i][j] = 0;
                    totalpixel++;
                }
            }
            ui->progressBar->setValue((int)(i*20/dataHeight));//更新进度条
        }
        delete[]m_data;
        m_data = NULL;
    }
    else if (BandNum == 3){
        unsigned long int** m_data = new unsigned long int*[3];
        for (int i = 0; i < 3; i++){
            m_data[i] = new unsigned long int[bufHeight*bufWidth];
        }
        for (int i = 0; i < 3; i++){
            //获取第1波段的波段指针，参数就是表示第几波段的意思
            GDALRasterBand *pBand = m_pDataset->GetRasterBand(i + 1);
            CPLErr err = pBand->RasterIO(GF_Read, 0, 0, dataWidth, dataHeight, m_data[i], bufWidth, bufHeight, GDT_UInt32, 0, 0);
        }
        for (int i = 0; i < dataHeight; i++){
            for (int j = 0; j < dataWidth; j++){
                img[i][j] = (m_data[0][i*dataWidth + j] * 3 + m_data[1][i*dataWidth + j] * 6 + m_data[2][i*dataWidth + j] + 5) / 10;
                if (img[i][j] == buildvalue){
                    isChecked[i][j] = 0;
                    totalpixel++;
                }
            }
             ui->progressBar->setValue((int)(i*20/dataHeight));//更新进度条20
        }
        for (int i = 0; i < 3; i++){
            delete[] m_data[i];
        }
        delete[]m_data;
        m_data = NULL;
    }

    string ref = projRef;//gdalclose()后，projRef改变，所以要存到string中
    const char *re = ref.data();
    img.clear();
    qDebug()<< "读取完毕";
    imginfo.SetImgInfo(dataWidth, dataHeight, filepath, BandNum, totalpixel, depth, bit, re, adfGeoTransform[0], adfGeoTransform[3], adfGeoTransform[1], adfGeoTransform[5]);

    ui->progresstitle->setText("准备就绪");
    ui->progressBar->setVisible(FALSE);
    //GDALClose(m_pDataset);
}

void test::WriteShp(const char *inputfilepath, const char *outputfilepath){
    qDebug()  << "开始输出边界.shp" ;
    //注册
    GDALAllRegister();
    //以防中文名不能正常读取
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
    GDALDataset *m_pDataset = (GDALDataset*)GDALOpen(inputfilepath, GA_ReadOnly);
    const char *projRef = m_pDataset->GetProjectionRef();

    /** 过程2 **/
    OGRSpatialReference fRef, tRef;
    char *tmp = NULL;
    /** 获得projRef的一份拷贝 **/
    /** 由于projRef是const char*,下面的一个函数不接受，所以需要转换成非const **/
    tmp = (char *)malloc(strlen(projRef) + 1);
    strcpy_s(tmp, strlen(projRef) + 1, projRef);


    /** 设置原始的坐标参数，和test.tif一致 **/
    fRef.importFromWkt(&tmp);
    /** 设置转换后的坐标 **/
    tRef.SetWellKnownGeogCS("WGS84");

    list<PointF>Pwgs84;
    list<PointF>::iterator itor_2;
    int num = convex.size();
    /** 下面进行坐标转换，到此为止都不需要proj，但是下面的内容如果不安装proj将会无法编译 **/
    OGRCoordinateTransformation *coordTrans;
    coordTrans = OGRCreateCoordinateTransformation(&fRef, &tRef);
    for (int i = 0; i < num; i++){
        double x = imginfo.GetPx() + convex.top().x*imginfo.Getpw();
        double y = imginfo.GetPy() + convex.top().y*imginfo.Getph();
        coordTrans->Transform(1, &x, &y);
        PointF p;
        p.Init(x, y);
        Pwgs84.push_back(p);
        convex.pop();
    }

    GDALClose(m_pDataset);

    OGRRegisterAll();//注册所有的驱动
    //创建ESRI shp文件
    const char *pszDriverName = "ESRI Shapefile";
    //调用对Shape文件读写的Driver
    CPLSetConfigOption("SHAPE_ENCODING", "");
    OGRSFDriver *poDriver ;
    poDriver= OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName);
    if (poDriver == NULL)
    {
        qDebug()  << pszDriverName << "驱动不可用！" ;
        return;
    }
    //创建数据源
    OGRDataSource *poDs = poDriver->CreateDataSource(outputfilepath, NULL);
    if (poDs == NULL)
    {
        qDebug()  << "DataSource Creation Error" ;
        return;
    }
    //创建图层Layer
    /*	string outShapName = outputfilepath;
        string layerName = outShapName.substr(0, outShapName.length() - 4);*/
    //layerName.c_str()表示将string转为char *类型
    //参数说明:新图层名称，坐标系，图层的几何类型，创建选项，与驱动有关
    OGRLayer *poLayer = poDs->CreateLayer(outputfilepath, &tRef, wkbLineString, NULL);
    if (poLayer == NULL)
    {
        qDebug() << "Layer Creation Failed" ;
        OGRDataSource::DestroyDataSource(poDs);
        return;
    }
    //创建一个feature
    OGRFeature *poFeature;
    poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());//GetLayerDefn()获取当前图层的属性表结构

    //给属性表中我们刚创建的列赋值
    OGRLineString or;
    /** 这时候x,y已经转化为经纬度啦 **/
    itor_2 = Pwgs84.begin();
    int mm=0;
    while (itor_2 != Pwgs84.end()){
        or.addPoint((*itor_2).x, (*itor_2).y);
        itor_2++;
        ui->progressBar->setValue((int)(mm*20/Pwgs84.size()+80));//100
        mm++;
    }
    poFeature->SetGeometry(&or);
    //or.addPoint(10.6, 45.6);
    if (poLayer->CreateFeature(poFeature) != OGRERR_NONE)
    {
        qDebug()  << "Failed to create feature in shapefile.\n" ;
        exit(1);
    }
    OGRFeature::DestroyFeature(poFeature);
    OGRDataSource::DestroyDataSource(poDs);
    qDebug()  << "输出边界完毕." ;
}

list<Point> AreaGrow(int seedx, int seedy){
    stack<Point> pointstack;//存储待生长的种子
    list<Point> pointlist;//存储区域生长范围内的像素点

    //四邻域
    int dx[4] = { -1, 0, 1, 0 };
    int dy[4] = { 0, -1, 0, 1 };

    int current_x, current_y;//当前正在处理的中心像素点
    int lin_x, lin_y;//邻域待判断像素点

    Point p;
    p.Init(seedx, seedy);
    pointstack.push(p);
    pointlist.push_back(p);

    while (pointstack.size() > 0){
        current_x = pointstack.top().x;
        current_y = pointstack.top().y;
        pointstack.pop();
        isChecked[current_y][current_x] = 1;

        for (int k = 0; k < 4; k++){
            lin_x = current_x + dx[k];
            lin_y = current_y + dy[k];
            //判断是否超界
            if ((lin_x < imginfo.GetWidth() && lin_x >= 0) && (lin_y < imginfo.GetHeight() && lin_y >= 0)){
                if (isChecked[lin_y][lin_x] == 0){
                    Point p;
                    p.Init(lin_x, lin_y);
                    pointstack.push(p);
                    pointlist.push_back(p);
                    isChecked[lin_y][lin_x] = 1;
                }
            }
        }
    }
    return pointlist;
}

void test::SeedGrowth(int minpixelcount){
    list<Point>point_c;
    vector<list<Point>>Points;
    qDebug() << "开始区域生长...";
    //按顺序找种子点
    for (int i = 0; i < imginfo.GetHeight(); i++){
        for (int j = 0; j < imginfo.GetWidth(); j++){
            if (isChecked[i][j] != 0){
                continue;
            }
            //1或2
            point_c = AreaGrow(j, i);//植入种子
            if (point_c.size() > minpixelcount){
                Points.push_back(point_c);
            }
        }
        ui->progressBar->setValue((int)(i*30/imginfo.GetHeight()+20));//50
    }
    point_c.clear();
    qDebug()  << "生长结束.";
    qDebug() << "生长区域数量:" << Points.size() ;
    //找出生长区域面积最大的一块
    int areamax = 0;
    int areamark = 0;
    for (int i = 0; i < Points.size(); i++){
        if (Points[i].size()>areamax){
            areamax = Points[i].size();
            areamark = i;
        }
        ui->progressBar->setValue((int)(i*10/Points.size()+50));//60
    }
    qDebug()  << "最大区域的面积:" << areamax ;
    //抽稀
    itor = Points[areamark].begin();
    while (itor != Points[areamark].end()){
        Point_Max.push_back(*itor);
        for (int i = 0; i < 30; i++){
            if (itor == Points[areamark].end()){
                break;
            }
            itor++;
        }
    }
    Points.clear();
    qDebug() << "抽稀的面积:" << Point_Max.size() ;
    isChecked.clear();
}

int Direction(Point p0, Point p1, Point p2){//判断顺逆 1逆 2顺 3长 4短
    int result;
    int chaji;
    int P1x, P1y, P2x, P2y;
    P1x = p1.x - p0.x;
    P1y = p1.y - p0.y;
    P2x = p2.x - p0.x;
    P2y = p2.y - p0.y;
    chaji = P1y*P2x - P1x*P2y;//p0_p2在p0_p1顺时针还是逆时针:P2*P1
    if (chaji > 0){//逆
        result = 1;
    }
    else if (chaji < 0){//顺
        result = 2;
    }
    else{
        if (Distance(p0, p1) < Distance(p0, p2)){
            result = 3;
        }
        else
        {
            result = 4;
        }
    }
    return result;
}

double Distance(Point pb, Point pf){
    int bx, by, fx, fy;
    double distance;
    bx = pb.x;
    by = pb.y;
    fx = pf.x;
    fy = pf.y;
    distance = sqrt(pow((bx - fx), 2) + pow((by - fy), 2));
    return distance;
}

double PolarAngle(Point pb, Point pf){
    int bx, by, fx, fy;
    double theta;
    double polar;
    bx = pb.x;
    by = pb.y;
    fx = pf.x;
    fy = pf.y;
    if (by == fy){
        if (fx > bx){
            polar = 270.0;
        }
        else if (fx < bx)
        {
            polar = 90.0;
        }
    }
    else if (bx == fx){
        if (fy > by){
            polar = 180.0;
        }
        else if (fy < by){
            polar = 0;
        }
    }
    else{
        theta = atan(abs((double)(bx - fx) / (double)(by - fy)))*180.0 / PI;
        if (fx < bx&& fy < by){//0-90
            polar = theta;
        }
        else if (fx < bx&& fy > by){//90-180
            polar = 180.0 - theta;
        }
        else if (fx > bx&& fy > by){//180-270
            polar = 180.0 + theta;
        }
        else if (fx > bx&& fy < by){//270-360
            polar = 360.0 - theta;
        }
    }
    return polar;
}

void test::Convex(){
    qDebug() << "生成凸包..." ;
    itor_1 = Point_Max.begin();
    convex.push((*itor_1));
    Point_Max.erase(itor_1++);
    //找出y最小的
    while (itor_1 != Point_Max.end()){
        if ((*itor_1).y < convex.top().y){
            Point_Max.push_front(convex.top());
            convex.pop();
            convex.push((*itor_1));
            Point_Max.erase(itor_1++);
        }
        else if ((*itor_1).y == convex.top().y)
        {
            if ((*itor_1).x < convex.top().x){
                Point_Max.push_front(convex.top());
                convex.pop();
                convex.push((*itor_1));
                Point_Max.erase(itor_1++);
            }
            else{
                itor_1++;
            }
        }
        else{
            itor_1++;
        }
    }

    list<Point>Point_Max_Re;//排序后
    ///循环
    Point p1 = convex.top();//起始点，栈底
    int mmmm=0;
    int size=Point_Max.size();
    for (itor_1 = Point_Max.begin(); itor_1 != Point_Max.end();){
        double angle = PolarAngle(p1, (*itor_1));
        double distance = Distance(p1, (*itor_1));
        Point_Max_Re.push_back((*itor_1));
        Point pc = *itor_1;//临时
        Point_Max.erase(itor_1++);
        while (itor_1 != Point_Max.end()){//若小于最小极角，则插入新链，删除旧节点
            if (PolarAngle(p1, (*itor_1)) < angle){
                angle = PolarAngle(p1, (*itor_1));
                distance = Distance(p1, (*itor_1));
                Point_Max_Re.pop_back();
                Point_Max_Re.push_back((*itor_1));
                Point_Max.push_front(pc);
                pc = *itor_1;
                Point_Max.erase(itor_1++);
            }
            else if (abs(PolarAngle(p1, (*itor_1)) - angle) < 0.000001){
                if (Distance(p1, (*itor_1)) > distance){
                    distance = Distance(p1, (*itor_1));
                    Point_Max_Re.pop_back();
                    Point_Max_Re.push_back((*itor_1));
                    Point_Max.push_front(pc);
                    pc = *itor_1;
                    Point_Max.erase(itor_1++);
                }
                else{
                    itor_1++;
                }
            }
            else{
                itor_1++;
            }
        }
        itor_1 = Point_Max.begin();
        ui->progressBar->setValue((int)(mmmm*10/size+60));//80
        mmmm++;
    }
    Point_Max_Re.push_back(p1);
    convex.push(*Point_Max_Re.begin());
    Point_Max_Re.pop_front();
    convex.push(*Point_Max_Re.begin());
    Point_Max_Re.pop_front();

    int nu=0;
    int si=Point_Max_Re.size();
    itor_1 = Point_Max_Re.begin();
    while (itor_1 != Point_Max_Re.end()){
        //取栈顶的2个元素,以及链首的元素
        Point pi_0, pi_1, pi_2;
        pi_1 = convex.top();
        convex.pop();
        pi_0 = convex.top();
        convex.push(pi_1);
        itor_1 = Point_Max_Re.begin();
        pi_2 = *itor_1;//判断点
        Point_Max_Re.erase(itor_1++);

        //判断pi_0-pi_2在pi_0-pi_1的顺时针还是逆时针
        int dir = Direction(pi_0, pi_1, pi_2);
        if (dir == 1){//逆
            convex.push(pi_2);
        }
        else if (dir == 2){//顺,在删除中间节点的情况下，需要往回判断一次
            convex.pop();
            while (dir == 2 || dir == 3){
                pi_1 = convex.top();
                convex.pop();
                pi_0 = convex.top();
                convex.push(pi_1);
                dir = Direction(pi_0, pi_1, pi_2);
                if (dir == 2 || dir == 3){
                    convex.pop();
                    if (convex.size() == 2){
                        break;
                    }
                }
            }
            convex.push(pi_2);
        }
        else if (dir == 3){//长,在删除中间节点的情况下，需要往回判断一次
            convex.pop();
            while (dir == 2 || dir == 3){
                pi_1 = convex.top();
                convex.pop();
                pi_0 = convex.top();
                convex.push(pi_1);
                dir = Direction(pi_0, pi_1, pi_2);
                if (dir == 2 || dir == 3){
                    convex.pop();
                    if (convex.size() == 2){
                        break;
                    }
                }
            }
            convex.push(pi_2);
        }
        else if (dir == 4){//短

        }
         ui->progressBar->setValue((int)(nu*10/si+70));//80
         nu++;
    }
    Point_Max_Re.clear();
    Point_Max.clear();
    qDebug()<<"生成凸包完毕.";
}
///////////////////////////////////////////////////////////////////////////////////////////


void test::on_pushButton_clicked()
{
    if(ui->inputfile->text().isEmpty()||ui->outputfile->text().isEmpty()||ui->areaid->text().isEmpty()){
        QMessageBox::information(NULL,NULL,"please no blank");
    }else{
        inputfile=ui->inputfile->text();
        outputfile=ui->outputfile->text();
        if(!QFile::exists(inputfile)){
            QMessageBox::information(NULL,NULL,"wrong input file");
        }else{
            if(QFile::exists(outputfile)){
                QFileInfo fileinfo(outputfile);
                QString name=fileinfo.baseName();
                QString path=fileinfo.path();
                QString filena=path+"/"+name;
                QString shp=filena+".shp";
                QString dbf=filena+".dbf";
                QString prj=filena+".prj";
                QString shx=filena+".shx";
                QFile::remove(shp);
                QFile::remove(dbf);
                QFile::remove(prj);
                QFile::remove(shx);
            }
        if(inputfile.contains("/")){
            inputfile=inputfile.replace("/","\\",Qt::CaseInsensitive);
            //QDir::toNativeSeparators(inputfile)
        }
        if(outputfile.contains("/")){
            outputfile=outputfile.replace("/","\\",Qt::CaseInsensitive);
        }
        AreaID=ui->areaid->text().toInt();
        Readimg(AreaID,inputfile.toLocal8Bit().data());
        //   QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));///可不写
        //imginfo.PrintInfo();
        ui->progresstitle->setText("区域生长..");
        ui->progressBar->setVisible(TRUE);
        ui->progressBar->setValue(0);
        int minpixel=ui->minpixel->currentText().toInt();
        SeedGrowth(minpixel);//区域生长最少像素
        ui->progresstitle->setText("凸壳..");
        Convex();
        ui->progresstitle->setText("写入shp");
        WriteShp(inputfile.toLocal8Bit().data(), outputfile.toLocal8Bit().data());
        ui->progressBar->setVisible(FALSE);
        ui->progresstitle->setText("完毕");
        }
    }
}
