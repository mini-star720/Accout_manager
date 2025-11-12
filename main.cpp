#include <QApplication>
#include <QStyleFactory>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 设置应用程序样式
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    
    // 设置应用程序信息
    QApplication::setApplicationName("智能记账本");
    QApplication::setApplicationDisplayName("智能记账本系统");
    QApplication::setApplicationVersion("1.0.0");
    
    MainWindow w;
    w.show();
    
    return a.exec();
}