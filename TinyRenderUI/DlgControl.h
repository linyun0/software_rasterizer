#ifndef DLGCONTROL_H
#define DLGCONTROL_H
#include <qwidget.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <unordered_map>
#include <qplaintextedit.h>
#define TinyRender_LoadData 0
#define TinyRender_LoadData_LinePlainText   1 
#define TinyRender_LoadData_CheckBox        2 
#define TinyRender_LoadData_PushButton      3
#define TinyRender_LoadData_Widget      4
#define TinyRender_DrawTriangle_PushButton  5
class AppDialog;

class DlgControl {
public:
	DlgControl();
public:
	virtual int GetId();
	virtual bool isShow();
	virtual QString GetString();
	virtual bool GetState();

	virtual void Hide();
	virtual void Show();
	virtual void SetCheckState(bool state);
protected:
	int m_id;
};

class PushButtonControl :public DlgControl
{
public:
	PushButtonControl(QPushButton* btn, int id);
private:
	QPushButton* m_obj;
};

class CheckBoxControl :public DlgControl {
public:
	CheckBoxControl(QCheckBox* checkBox, int id);
	virtual bool GetState() override;
	virtual void SetCheckState(bool state) override;
private:
	QCheckBox* m_obj;
};
class PlainTextEditControl :public DlgControl {

public:
	PlainTextEditControl(QPlainTextEdit* checkBox, int id);
	virtual QString GetString() override;
private:
	QPlainTextEdit* m_obj;
};

class WidgetControl :public DlgControl {
public:
	WidgetControl(QWidget* widget, int id);
	bool isShow() override;
	virtual void Hide() override;
	virtual void Show() override;
private:
	QWidget* m_obj;
};

class DialogCmd {
public:
	DialogCmd();
	~DialogCmd();
protected:
	AppDialog* dlgBox = nullptr;
public:
	virtual void Activate(DlgControl* control);
	virtual void init();
	AppDialog* GetDialog();
};

class AppDialog :public QWidget {
	
public:
	AppDialog(QWidget* parent, DialogCmd* cmd);
private:
	void BuildConnection();
	void BreakConnection();
public:
	DlgControl* GetControl(int id);
	DlgControl* GetControl(QString name);
protected:
	std::unordered_map<QString, int> m_NameToIdMap;
private:
	std::unordered_map<int, QString> m_IdToNameMap;
	std::unordered_map<QString, DlgControl*> m_NameToControl;
	bool CreateControl(const QString& name);
	DialogCmd* m_cmd;
	virtual void showEvent(QShowEvent* event) override;
private slots:
	virtual void OnPushButtonClicked();
	virtual void OnCheckBoxStateChanged(int);

};






#endif 