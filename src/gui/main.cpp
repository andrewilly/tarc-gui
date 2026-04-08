#include <QApplication>
#include <QIcon>
#include <QFontDatabase>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName("TARC Archiver");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("TARC");
    
    // Carica stile
    QFile styleFile(":/resources/styles/darkstyle.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString style = QLatin1String(styleFile.readAll());
        app.setStyleSheet(style);
    }
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
