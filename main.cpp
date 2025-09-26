#include "timerapp.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TimerApp timerApp;
    timerApp.show();

    return app.exec();
}
