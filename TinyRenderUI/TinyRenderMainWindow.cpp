#include "TinyRenderMainWindow.h"
#include "DrawAreaWidget.h"
#include "ControlDockWidgetCmd.h"
#include <qdockwidget.h>

TinyRenderMainWindow::TinyRenderMainWindow(QWidget* parent):QMainWindow(parent)
{
	m_drawArea = std::make_shared<DrawAreaWidget>(this);
	this->setCentralWidget(m_drawArea.get());
	

	cmd = std::make_shared<ControlDockWidgetCmd>(m_drawArea.get());
	cmd->init();

	m_dockWidget = std::make_shared<QDockWidget>(cmd->GetDialog());
	m_dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	this->addDockWidget(Qt::RightDockWidgetArea, m_dockWidget.get());
	m_dockWidget->setWidget(cmd->GetDialog());

	this->setMinimumSize(1000, 1000);
}

TinyRenderMainWindow::~TinyRenderMainWindow()
{

}
