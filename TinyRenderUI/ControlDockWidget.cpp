#include "ControlDockWidget.h"
#include <qdockwidget.h>
#include <qlayout.h>
#include <qpushbutton.h>

ControlDockWidget::ControlDockWidget(QWidget* parent, DialogCmd* cmd)
    :AppDialog(parent, cmd)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    // 创建滚动区域并设置为主部件
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true); // 允许内容扩展
    //(m_scrollArea);
    mainLayout->addWidget(m_scrollArea);
    // 创建滚动区域的容器部件
    QWidget* m_container = new QWidget;
    m_scrollArea->setWidget(m_container);

    // 主垂直布局
    QVBoxLayout* m_mainLayout = new QVBoxLayout(m_container);
    m_mainLayout->setAlignment(Qt::AlignTop);
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);

    QPushButton* SubjectButton1 = new QPushButton("LoadData", m_container);
    SubjectButton1->setObjectName("LoadDataPushButton");
    // connect(m_toggleButton, &QPushButton::clicked, this, &CustomDockWidget::toggleWidgetVisibility);
    m_mainLayout->addWidget(SubjectButton1);

    // 添加需要控制的Widget（示例）
    //QVBoxLayout* widgetLayout = new QVBoxLayout(InputWidget);
    //widgetLayout->setMargin(0);
    //widgetLayout->setSpacing(0);
    //widgetLayout->addWidget(new QPushButton("PushButton1"));
    //widgetLayout->addWidget(new QPushButton("PushButton2"));
    //widgetLayout->addWidget(new QPushButton("PushButton3"));
    //m_mainLayout->addWidget(InputWidget);

    // 添加伸缩弹簧保持顶部对齐
    m_mainLayout->addStretch();

    // 设置容器的最小宽度
    m_container->setMinimumWidth(300);

    // 禁用水平滚动条
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setMinimumWidth(300);

    m_NameToIdMap.insert({ "PlainTextEdit", TinyRender_LoadData_LinePlainText });
    m_NameToIdMap.insert({ "DrawLinesPushButton",TinyRender_LoadData_PushButton });
    m_NameToIdMap.insert({ "DrawTrianglePushButton",TinyRender_DrawTriangle_PushButton });
    m_NameToIdMap.insert({ "CNWCheckBox",TinyRender_LoadData_CheckBox });
    m_NameToIdMap.insert({ "LoadDataPushButton",TinyRender_LoadData });
    m_NameToIdMap.insert({ "InputDataWidget",TinyRender_LoadData_Widget });
}

void ControlDockWidget::setSize() {
    //QWidget* w = (QWidget*)(sender());
    //double width = (w->width())/10*4;
    //double height= w->height();
    // 
    //this->resize(width,height);
}

ControlDockWidget::~ControlDockWidget()
{
    if (m_scrollArea) {
        delete m_scrollArea;
    }

}
