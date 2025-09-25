#pragma once

#include <QDockWidget>
#include <qscrollarea.h>
#include "DlgControl.h"
class ControlDockWidget : public AppDialog
{
	

public:
	ControlDockWidget(QWidget* parent, DialogCmd* cmd = nullptr);
	~ControlDockWidget();
private slots:
	void setSize();

private:
	QScrollArea* m_scrollArea;
};
