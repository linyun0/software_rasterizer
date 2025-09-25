#ifndef CONTROLDOCKWIDGET_H
#define CONTROLDOCKWIDGET_H
#include <qmainwindow.h>
#include "DlgControl.h"
#include "DrawAreaWidget.h"
class DrawAreaWidget;
class ControlDockWidgetCmd :public DialogCmd {

public:
	ControlDockWidgetCmd(DrawAreaWidget*);
	void Activate(DlgControl* control) override;
	void init();
private:
	DrawAreaWidget* m_DrawArea;
};



#endif