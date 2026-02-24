#include <QtWidgets/QApplication>
#include "chonkylauncher.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName("ChonkyStation Launcher");
    app.setApplicationVersion("1.0.8");
    app.setOrganizationName("ChonkyLauncher");
    
    ChonkyLauncher launcher;
    launcher.show();
    
    return app.exec();
}
