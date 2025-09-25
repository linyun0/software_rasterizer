#pragma once
#include <QWidget>
#include <qmainwindow.h>

class DrawAreaWidget;
class ControlDockWidgetCmd;
class DialogCmd;
class  TinyRenderMainWindow:public QMainWindow
{
public:
	TinyRenderMainWindow(QWidget* parent=nullptr);
	~TinyRenderMainWindow();
private:
	DrawAreaWidget* m_drawArea = nullptr;
	DialogCmd* cmd = nullptr;
	QDockWidget* m_dockWidget = nullptr;

};
