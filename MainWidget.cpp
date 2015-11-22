#include "MainWidget.h"
#include "ui_MainWidget.h"
#include <QScreen>
#include <QPainter>
#include <QApplication>
#include <QClipboard>
#include <QCursor>
#include "ScreenshotService.h"
#include "Region.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setMouseTracking(true);

    setGeometry(ScreenshotService::Instance()->GetScreenRect());

    m_zoomInWindow = new ZoomInWindow(this);

    m_viewCreating = false;
	m_regionPressed = false;

	m_frameView = new FrameView(this);
	m_frameView->hide();
    connect(m_frameView, SIGNAL(RegionHoverd(int)), this, SLOT(OnRegionHovered(int)));
    connect(m_frameView, SIGNAL(RegionPressed(int)), this, SLOT(OnRegionPressed(int)));
    connect(m_frameView, SIGNAL(ViewDblClicked()), this, SLOT(OnViewDblClicked()));
    
    for (int i = 0; i < 8; i++)
    {
        m_blocks[i] = new Block(i, this);
        connect(m_blocks[i], SIGNAL(BlockHovered(int)), this, SLOT(OnRegionHovered(int)));
        connect(m_blocks[i], SIGNAL(BlockPressed(int)), this, SLOT(OnRegionPressed(int)));
        
        m_blocks[i]->resize(5, 5);
        m_blocks[i]->hide();
    }
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.drawPixmap(rect(), ScreenshotService::Instance()->GetDarkPixmap());

    QWidget::paintEvent(event);
}

void MainWidget::mouseMoveEvent(QMouseEvent* event)
{
    QPoint curPos = event->pos();
    
    // 还未选定区域时
    if (!m_frameView->isVisible())
    {
        UpdateZoomInWindow(curPos);
    }
    
	if (m_viewCreating) // 正在选定区域
	{
        m_zoomInWindow->hide();
		m_frameView->show();
		m_frameView->setGeometry(MakeRect(curPos, m_startPos));
        MoveBlocks();
	}
    else if (m_regionPressed)   // 移动或拖拽已选定区域
    {
        QPoint activePos, pivotPos;
        
        if (m_pressedRegion == Region::LeftTop)
        {
            activePos = curPos;
            pivotPos = m_oldViewGeo.bottomRight();
        }
        else if (m_pressedRegion == Region::Top)
        {
            activePos = QPoint(m_oldViewGeo.left(), curPos.y());
            pivotPos = m_oldViewGeo.bottomRight();
        }
        else if (m_pressedRegion == Region::TopRight)
        {
            activePos = curPos;
            pivotPos = m_oldViewGeo.bottomLeft();
        }
        else if (m_pressedRegion == Region::Right)
        {
            activePos = QPoint(curPos.x(), m_oldViewGeo.top());
            pivotPos = m_oldViewGeo.bottomLeft();
        }
        else if (m_pressedRegion == Region::RightBottom)
        {
            activePos = curPos;
            pivotPos = m_oldViewGeo.topLeft();
        }
        else if (m_pressedRegion == Region::Bottom)
        {
            activePos = QPoint(m_oldViewGeo.right(), curPos.y());
            pivotPos = m_oldViewGeo.topLeft();
        }
        else if (m_pressedRegion == Region::LeftBottom)
        {
            activePos = curPos;
            pivotPos = m_oldViewGeo.topRight();
        }
        else if (m_pressedRegion == Region::Left)
        {
            activePos = QPoint(curPos.x(), m_oldViewGeo.bottom());
            pivotPos = m_oldViewGeo.topRight();
        }
        else if (m_pressedRegion == Region::Inner)
        {
            QRect newGeo = m_oldViewGeo;
            newGeo.translate(curPos - m_startPos);
            
            if (newGeo.left() < 0)
                newGeo.translate(-newGeo.left(), 0);
            else if (newGeo.right() > width())
                newGeo.translate(width() - newGeo.right(), 0);
            
            if (newGeo.top() < 0)
                newGeo.translate(0, -newGeo.top());
            else if (newGeo.bottom() > height())
                newGeo.translate(0, height() - newGeo.bottom());
            
            activePos = newGeo.topLeft();
            pivotPos = newGeo.bottomRight();
        }
        
        m_frameView->setGeometry(MakeRect(activePos, pivotPos));
        MoveBlocks();
    }

    QWidget::mouseMoveEvent(event);
}

QRect MainWidget::MakeRect(const QPoint& p1, const QPoint& p2)
{
    QPoint topLeft(qMin(p1.x(), p2.x()), qMin(p1.y(), p2.y()));
    QPoint bottomRight(qMax(p1.x(), p2.x()), qMax(p1.y(), p2.y()));
    return QRect(topLeft, bottomRight);
}

void MainWidget::mousePressEvent(QMouseEvent* event)
{
    m_startPos = event->pos();
    
    if (!m_frameView->isVisible())
    {
        m_viewCreating = true;
    }    
    else if (m_regionPressed)
    {
        m_oldViewGeo = m_frameView->geometry();
    }
    
	QWidget::mousePressEvent(event);
}

void MainWidget::mouseReleaseEvent(QMouseEvent* event)
{
	m_viewCreating = false;
    m_regionPressed = false;
    m_frameView->CutIn9Parts();
    
	QWidget::mouseReleaseEvent(event);
}

void MainWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        QApplication::quit();
    }
    
    QWidget::keyPressEvent(event);
}

void MainWidget::OnRegionHovered(int region)
{
    switch (region)
    {
    case -1:
        setCursor(Qt::ArrowCursor);
        break;
    case Region::Top:
    case Region::Bottom:
        setCursor(Qt::SizeVerCursor);
        break;
    case Region::TopRight:
    case Region::LeftBottom:
        setCursor(Qt::SizeBDiagCursor);
        break;
    case Region::Left:
    case Region::Right:
        setCursor(Qt::SizeHorCursor);
        break;
    case Region::RightBottom:
    case Region::LeftTop:
        setCursor(Qt::SizeFDiagCursor);
        break;
    case Region::Inner:
        setCursor(Qt::SizeAllCursor);
        break;
    default:
        break;
    }
}

void MainWidget::OnRegionPressed(int region)
{
    m_regionPressed = true;
    m_pressedRegion = region;
}

void MainWidget::OnViewDblClicked()
{
    QPixmap grabbedPixmap = ScreenshotService::Instance()->GetScreenshotPixmap()
            .copy(m_frameView->geometry());
    
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setPixmap(grabbedPixmap);
    QApplication::quit();
}

void MainWidget::UpdateZoomInWindow(const QPoint& curPos)
{
    QRect screenRect = ScreenshotService::Instance()->GetScreenRect();
    
    int x, y;
    if (curPos.x() + m_zoomInWindow->width() + 10 > screenRect.width())
        x = curPos.x() - m_zoomInWindow->width() - 10;
    else
        x = curPos.x() + 10;

    if (curPos.y() + m_zoomInWindow->height() + 10 > screenRect.height())
        y = curPos.y() - m_zoomInWindow->height() - 10;
    else
        y = curPos.y() + 10;
    
    m_zoomInWindow->move(x, y);
    m_zoomInWindow->Focus(curPos);
    m_zoomInWindow->Update();
    
    if (!m_zoomInWindow->isVisible())
        m_zoomInWindow->show();
}

void MainWidget::MoveBlocks()
{
    QRect geo = m_frameView->geometry();
    geo.translate(-2, -2);
    m_blocks[Region::LeftTop]->move(geo.topLeft());
    m_blocks[Region::TopRight]->move(geo.topRight());
    m_blocks[Region::RightBottom]->move(geo.bottomRight());
    m_blocks[Region::LeftBottom]->move(geo.bottomLeft());
    m_blocks[Region::Top]->move((geo.topLeft() + geo.topRight()) / 2);
    m_blocks[Region::Right]->move((geo.topRight() + geo.bottomRight()) / 2);
    m_blocks[Region::Bottom]->move((geo.bottomRight() + geo.bottomLeft()) / 2);
    m_blocks[Region::Left]->move((geo.bottomLeft() + geo.topLeft()) / 2);
    
    for (int i = 0; i < 8; i++)
    {
        m_blocks[i]->show();
    }
}

void MainWidget::DoScreenshot()
{
    ScreenshotService::Instance()->Screenshot();
    show();
    UpdateZoomInWindow(QCursor::pos());
}

