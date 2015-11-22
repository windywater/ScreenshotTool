#ifndef MAINTOOL_H
#define MAINTOOL_H

#include <QToolBar>
#include <QCloseEvent>
#include <QTimer>

class MainTool : public QToolBar
{
    Q_OBJECT
public:
    MainTool(QWidget* parent = 0);
    ~MainTool();
    
protected:
    virtual void closeEvent(QCloseEvent* event);
    
signals:
    void beginScreenshot();
    
protected slots:
    void OnScreenshotActionTriggered();
    void OnHiddenMonitorTimeout();
    
protected:
    QTimer* m_hiddenMonitor;
};

#endif // MAINTOOL_H
