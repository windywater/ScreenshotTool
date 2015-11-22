#include "MainTool.h"
#include <QApplication>
#include <QAction>
#include "ScreenshotService.h"

MainTool::MainTool(QWidget* parent)
    : QToolBar(parent)
{
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
    
    QAction* screenshotAction = addAction("screenshot");
    connect(screenshotAction, SIGNAL(triggered()), this, SLOT(OnScreenshotActionTriggered()));
    
    m_hiddenMonitor = new QTimer(this);
    m_hiddenMonitor->setInterval(50);
    connect(m_hiddenMonitor, SIGNAL(timeout()), this, SLOT(OnHiddenMonitorTimeout()));
}

MainTool::~MainTool()
{
    
}

void MainTool::OnScreenshotActionTriggered()
{
    hide();
    m_hiddenMonitor->start();
}

void MainTool::closeEvent(QCloseEvent* event)
{
    QApplication::quit();
    QToolBar::closeEvent(event);
}

void MainTool::OnHiddenMonitorTimeout()
{
    if (!isVisible())
    {
        m_hiddenMonitor->stop();
        emit beginScreenshot();
    }
}
