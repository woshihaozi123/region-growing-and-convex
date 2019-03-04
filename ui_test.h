/********************************************************************************
** Form generated from reading UI file 'test.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEST_H
#define UI_TEST_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_test
{
public:
    QWidget *centralWidget;
    QPushButton *inputButton;
    QLineEdit *outputfile;
    QLabel *label;
    QLineEdit *inputfile;
    QPushButton *outputButton;
    QLabel *label_2;
    QPushButton *pushButton;
    QLabel *label_3;
    QProgressBar *progressBar;
    QLabel *progresstitle;
    QLineEdit *areaid;
    QLabel *label_4;
    QComboBox *minpixel;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *test)
    {
        if (test->objectName().isEmpty())
            test->setObjectName(QStringLiteral("test"));
        test->resize(430, 223);
        centralWidget = new QWidget(test);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        inputButton = new QPushButton(centralWidget);
        inputButton->setObjectName(QStringLiteral("inputButton"));
        inputButton->setGeometry(QRect(380, 33, 31, 23));
        outputfile = new QLineEdit(centralWidget);
        outputfile->setObjectName(QStringLiteral("outputfile"));
        outputfile->setGeometry(QRect(100, 65, 271, 20));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 40, 54, 12));
        inputfile = new QLineEdit(centralWidget);
        inputfile->setObjectName(QStringLiteral("inputfile"));
        inputfile->setGeometry(QRect(100, 35, 271, 20));
        outputButton = new QPushButton(centralWidget);
        outputButton->setObjectName(QStringLiteral("outputButton"));
        outputButton->setGeometry(QRect(380, 63, 31, 23));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(30, 70, 54, 12));
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(340, 160, 71, 23));
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(30, 100, 60, 12));
        progressBar = new QProgressBar(centralWidget);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setGeometry(QRect(100, 160, 231, 20));
        progressBar->setValue(24);
        progresstitle = new QLabel(centralWidget);
        progresstitle->setObjectName(QStringLiteral("progresstitle"));
        progresstitle->setGeometry(QRect(30, 160, 61, 20));
        QFont font;
        font.setFamily(QString::fromUtf8("\351\273\221\344\275\223"));
        font.setPointSize(8);
        progresstitle->setFont(font);
        progresstitle->setLayoutDirection(Qt::LeftToRight);
        areaid = new QLineEdit(centralWidget);
        areaid->setObjectName(QStringLiteral("areaid"));
        areaid->setGeometry(QRect(100, 95, 71, 20));
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(200, 100, 84, 12));
        minpixel = new QComboBox(centralWidget);
        minpixel->setObjectName(QStringLiteral("minpixel"));
        minpixel->setGeometry(QRect(300, 94, 71, 22));
        test->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(test);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 430, 23));
        test->setMenuBar(menuBar);
        mainToolBar = new QToolBar(test);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        test->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(test);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        test->setStatusBar(statusBar);

        retranslateUi(test);

        QMetaObject::connectSlotsByName(test);
    } // setupUi

    void retranslateUi(QMainWindow *test)
    {
        test->setWindowTitle(QApplication::translate("test", "\345\214\272\345\237\237\347\224\237\351\225\277+\345\207\270\345\214\205", Q_NULLPTR));
        inputButton->setText(QApplication::translate("test", "...", Q_NULLPTR));
        label->setText(QApplication::translate("test", "\346\211\223\345\274\200Tif", Q_NULLPTR));
        outputButton->setText(QApplication::translate("test", "...", Q_NULLPTR));
        label_2->setText(QApplication::translate("test", "\350\276\223\345\207\272Shp", Q_NULLPTR));
        pushButton->setText(QApplication::translate("test", "\346\211\247\350\241\214", Q_NULLPTR));
        label_3->setText(QApplication::translate("test", "\345\273\272\347\255\221\345\210\206\347\261\273\345\200\274", Q_NULLPTR));
        progresstitle->setText(QApplication::translate("test", "\345\207\206\345\244\207\345\260\261\347\273\252", Q_NULLPTR));
        areaid->setText(QApplication::translate("test", "80", Q_NULLPTR));
        label_4->setText(QApplication::translate("test", "\345\237\216\345\214\272\345\203\217\347\264\240\346\234\200\345\260\217\345\200\274", Q_NULLPTR));
        minpixel->setCurrentText(QString());
    } // retranslateUi

};

namespace Ui {
    class test: public Ui_test {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEST_H
