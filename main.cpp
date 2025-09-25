#include<iostream>
#include <assimp/Importer.hpp>
#include "TinyRenderSrc/DataLoader.h"
#include <QWidget>
#include <QtWidgets/QApplication>
#include "TinyRenderUI/TinyRenderMainWindow.h"

int main(int argc, char* argv[])
{

	QApplication a(argc, argv);
	TinyRenderMainWindow mainWindow;
    mainWindow.show();
	return a.exec();
}
