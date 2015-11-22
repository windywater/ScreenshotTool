#include "MainWidget.h"
#include <QApplication>
#include <QToolBar>
#include "ScreenshotService.h"
#include "MainTool.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ScreenshotService::Instance()->Screenshot();

    MainTool mainTool;
    mainTool.show();
    mainTool.move(100, 100);
    
    MainWidget w;
    //w.show();
    
    QObject::connect(&mainTool, SIGNAL(beginScreenshot()), &w, SLOT(DoScreenshot()));

    return a.exec();
}
