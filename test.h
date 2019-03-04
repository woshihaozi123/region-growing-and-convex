#ifndef TEST_H
#define TEST_H
#include<QFileDialog>
#include<QFile>
#include<QMessageBox>
#include <QMainWindow>

namespace Ui {
class test;
}

class test : public QMainWindow
{
    Q_OBJECT

public:
    explicit test(QWidget *parent = 0);
    ~test();

private slots:
    void on_inputButton_clicked();
    void on_outputButton_clicked();
    void on_pushButton_clicked();
    void Readimg(int buildvalue,const char *filepath);//选择建筑id
    void Convex();//凸包算法
    void SeedGrowth(int minpixelcount);//区域生长
    void WriteShp(const char *inputfilepath, const char *outputfilepath);//写shp
private:
    Ui::test *ui;
};

#endif // TEST_H
