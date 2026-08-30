#ifndef CONTROLDOCKWIDGET_H
#define CONTROLDOCKWIDGET_H
#include <qmainwindow.h>
#include <memory>
#include "DlgControl.h"
#include "DrawAreaWidget.h"
class DrawAreaWidget;
class HexagonGridWidget;
class ControlDockWidgetCmd :public DialogCmd {

public:
	ControlDockWidgetCmd(std::shared_ptr<DrawAreaWidget> in_drawArea);
	void Activate(DlgControl* control) override;
	void init();
private:
	void GetColor(QColor color);
private:
	std::shared_ptr<DrawAreaWidget> m_DrawArea = nullptr;
	std::shared_ptr<HexagonGridWidget> m_hexa_color_dialog = nullptr;
};



#endif