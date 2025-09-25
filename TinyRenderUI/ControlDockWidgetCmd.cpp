#include "ControlDocKWidgetCmd.h" 
#include <qmessagebox.h>
#include "ControlDockWidget.h"
#include "DrawAreaWidget.h"
ControlDockWidgetCmd::ControlDockWidgetCmd(DrawAreaWidget* drawArea)
{
	m_DrawArea= drawArea;
}

void ControlDockWidgetCmd::Activate(DlgControl* control)
{
	int id = control->GetId();

	switch (id) {
	case TinyRender_LoadData_CheckBox:
	{
		int i = 1;

		break;
	}
	case TinyRender_LoadData_PushButton:
	{
		QString val;
		bool addNewWindow;
		control = dlgBox->GetControl(TinyRender_LoadData_CheckBox);
		addNewWindow = control->GetState();
		if (addNewWindow) {
			//m_mainWindow->AddChildWindow();
			control->SetCheckState(false);
		}
		//DrawAreaWidget* drawArea = m_mainWindow->GetActivatedDrawArea();
		control = dlgBox->GetControl(TinyRender_LoadData_LinePlainText);
		val = control->GetString();
		if (val.isEmpty()) {
			QMessageBox::warning(dlgBox, "", " the value can not be empty!");
			return;
		}
		//std::vector<vec3> Lines = DataLoader::LoadLinesData(val, drawArea->width(), drawArea->height());
		//drawArea->DrawLines(Lines);

		break;
	}
	case TinyRender_LoadData_LinePlainText:
	{

		break;
	}
	case TinyRender_LoadData:
	{
		control = dlgBox->GetControl(TinyRender_LoadData_Widget);
		if (control->isShow()) {
			control->Hide();
		}
		else {
			control->Show();
		}
		break;
	}
	case TinyRender_DrawTriangle_PushButton:
	{
		QString val;
		bool addNewWindow;
		control = dlgBox->GetControl(TinyRender_LoadData_CheckBox);
		addNewWindow = control->GetState();
		if (addNewWindow) {
			//m_mainWindow->AddChildWindow();
			control->SetCheckState(false);
		}
	//	DrawAreaWidget* drawArea = m_mainWindow->GetActivatedDrawArea();
		control = dlgBox->GetControl(TinyRender_LoadData_LinePlainText);
		val = control->GetString();
		if (val.isEmpty()) {
			QMessageBox::warning(dlgBox, "", " the value can not be empty!");
			return;
		}
	//	std::vector<vec3> Lines = DataLoader::LoadLinesData(val, drawArea->width(), drawArea->height());
		//drawArea->DrawTriangles(Lines);

		break;
	}
	default:
	{

	}

	}



}

void ControlDockWidgetCmd::init()
{
	if (dlgBox == nullptr) {
		dlgBox = new ControlDockWidget(m_DrawArea->parentWidget(), this);
	}
	dlgBox->show();
}
