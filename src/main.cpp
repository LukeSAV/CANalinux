#include <QPushButton>
#include <QApplication>
#include <QMenu>
#include <QComboBox>
#include <QStringList>
#include "mainwindow.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    MainWindow* main_window = new MainWindow();
    main_window->show();
    return app.exec();
}
