import QtQuick
import QtQuick.Controls
import TwoWayRender 1.0

ApplicationWindow {
    width: 900
    height: 700
    visible: true
    title: "TwoWayRender - CPU"

    // 渲染控制器（加载模型 + 渲染）
    RenderController {
        id: controller
    }

    // CPU 渲染视图
    CpuRenderView {
        id: cpuView
        anchors.fill: parent
        anchors.bottomMargin: 60
        controller: controller
    }

    // 底部控制栏
    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 60
        color: "#1e1e1e"

        Row {
            anchors.centerIn: parent
            spacing: 16

            Button {
                text: "Rotate +30"
                onClicked: controller.rotate(30)
            }

            Button {
                text: "Rotate -30"
                onClicked: controller.rotate(-30)
            }
        }
    }
}
