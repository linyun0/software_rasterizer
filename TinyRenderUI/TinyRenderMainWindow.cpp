#include "TinyRenderMainWindow.h"
#include "DrawAreaWidget.h"
#include "ControlDockWidgetCmd.h"
#include <qdockwidget.h>

TinyRenderMainWindow::TinyRenderMainWindow(QWidget* parent):QMainWindow(parent)
{
	m_drawArea = new DrawAreaWidget(this);
	this->setCentralWidget(m_drawArea);
	

	cmd = new ControlDockWidgetCmd(m_drawArea);
	cmd->init();

	m_dockWidget = new QDockWidget(cmd->GetDialog());
	m_dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	this->addDockWidget(Qt::RightDockWidgetArea, m_dockWidget);
	m_dockWidget->setWidget(cmd->GetDialog());

	this->setMinimumSize(1000, 1000);
}

TinyRenderMainWindow::~TinyRenderMainWindow()
{
	if (m_drawArea) {
		delete m_drawArea;
	}
	if (cmd) {
		delete cmd;
	}
}
