#include "timerapp.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Check if system tray is available
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qWarning() << "System tray not available on this system";
    }

    TimerApp timerApp;
    timerApp.show();

    return app.exec();
}
