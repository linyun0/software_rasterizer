#include "DlgControl.h"
#include <qpushbutton.h>
#include <qcheckbox.h>


AppDialog::AppDialog(QWidget* parent, DialogCmd* cmd) :QWidget(parent), m_cmd(cmd)
{

}

void AppDialog::BuildConnection()
{
	for (QWidget* child : this->findChildren<QWidget*>())
	{
		QString objectName = child->objectName();
		if (objectName.contains("PushButton")) {
			QPushButton* btn = (QPushButton*)child;
			connect(btn, &QPushButton::clicked, this,&AppDialog::OnPushButtonClicked);
		}
		else if (objectName.contains("CheckBox")) {
			QCheckBox* box = (QCheckBox*)(child);
			connect(box, SIGNAL(stateChanged(int)), this, SLOT(OnCheckBoxStateChanged(int)));
		}
	}
}

void AppDialog::BreakConnection()
{

}

DlgControl* AppDialog::GetControl(int id)
{
	if (m_IdToNameMap.find(id) == m_IdToNameMap.end()) {
		return nullptr;
	};

	QString name = m_IdToNameMap[id];

	if (m_NameToControl.find(name) != m_NameToControl.end()) {
		return m_NameToControl[name];
	};

	if (CreateControl(name))
	{
		return m_NameToControl[name];
	};

	Q_ASSERT_X(false, "", ("No" + name + " Control").toLocal8Bit());
	return nullptr;
}

DlgControl* AppDialog::GetControl(QString name)
{
	if (m_NameToControl.find(name) != m_NameToControl.end()) {
		return m_NameToControl[name];
	};

	if (CreateControl(name))
	{
		return m_NameToControl[name];
	};

	return nullptr;
}

bool AppDialog::CreateControl(const QString& name)
{
	QWidget* childWidget = this->findChild<QWidget*>(name);
	bool ans = false;
	int  id = m_NameToIdMap[name];
	if (name.contains("PushButton")) {
		m_NameToControl[name] = (new PushButtonControl((QPushButton*)childWidget, id));
		ans = true;
	}
	else if (name.contains("CheckBox")) {
		m_NameToControl[name] = (new CheckBoxControl((QCheckBox*)childWidget, id));
		ans = true;
	}
	else if (name.contains("Widget")) {
		m_NameToControl[name] = (new WidgetControl(childWidget, id));
		ans = true;
	}
	else if (name.contains("PlainTextEdit")) {
		m_NameToControl[name] = (new PlainTextEditControl((QPlainTextEdit*)childWidget, id));
		ans = true;
	}

	return ans;
}

void AppDialog::showEvent(QShowEvent* event)
{
	BuildConnection();
	for (auto it : m_NameToIdMap) {
		m_IdToNameMap[it.second] = it.first;
	}
	QWidget::showEvent(event);
}

void AppDialog::OnCheckBoxStateChanged(int)
{
	QString name = sender()->objectName();
	//	m_cmd->Activate(GetControl(name));
}
void AppDialog::OnPushButtonClicked()
{
	QString name = sender()->objectName();
	m_cmd->Activate(GetControl(name));
}

DialogCmd::DialogCmd()
{
}

DialogCmd::~DialogCmd()
{
	if (dlgBox) {
		delete dlgBox;
	}
}

void DialogCmd::Activate(DlgControl* control)
{
}

void DialogCmd::init()
{
}

AppDialog* DialogCmd::GetDialog()
{
	if (dlgBox) {
		return dlgBox;
	}
}

DlgControl::DlgControl()
{
}

int DlgControl::GetId()
{
	return m_id;
}

bool DlgControl::isShow()
{
	return false;
}

QString DlgControl::GetString()
{
	return QString();
}

bool DlgControl::GetState()
{
	return false;
}

void DlgControl::Hide()
{
}

void DlgControl::Show()
{
}

void DlgControl::SetCheckState(bool state)
{
}

PushButtonControl::PushButtonControl(QPushButton* btn, int id) :m_obj(btn)
{
	m_id = id;
}

CheckBoxControl::CheckBoxControl(QCheckBox* checkBox, int id) :m_obj(checkBox)
{
	m_id = id;
}

bool CheckBoxControl::GetState()
{
	return m_obj->isChecked();
}

void CheckBoxControl::SetCheckState(bool state)
{
	m_obj->setCheckState(state ? Qt::Checked : Qt::Unchecked);
}

WidgetControl::WidgetControl(QWidget* widget, int id) :m_obj(widget)
{
}

bool WidgetControl::isShow()
{
	return m_obj->isVisible();
}

void WidgetControl::Hide()
{
	m_obj->hide();
}

void WidgetControl::Show()
{
	m_obj->show();
}

PlainTextEditControl::PlainTextEditControl(QPlainTextEdit* plainText, int id) :m_obj(plainText)
{
}

QString PlainTextEditControl::GetString()
{

	return m_obj->toPlainText();
}
