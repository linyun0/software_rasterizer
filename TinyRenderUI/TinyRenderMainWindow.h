#pragma once
#include <QWidget>
#include <qmainwindow.h>
#include <QHBoxLayout>
#include <memory>
class DrawAreaWidget;
class ControlDockWidgetCmd;
class DialogCmd;
class  TinyRenderMainWindow:public QMainWindow
{
public:
	TinyRenderMainWindow(QWidget* parent=nullptr);
	~TinyRenderMainWindow();
private:
	std::shared_ptr<DrawAreaWidget> m_drawArea = nullptr;
	std::shared_ptr<DialogCmd> cmd = nullptr;
	std::shared_ptr<QDockWidget> m_dockWidget = nullptr;

	
};
