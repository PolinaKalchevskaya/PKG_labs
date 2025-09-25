#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Настройка стиля приложения
    app.setApplicationName("Image Info Analyzer");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("Vogella Experiment");

    MainWindow window;
    window.show();

    return app.exec();
}
