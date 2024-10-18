#include <QApplication>
#include "Window.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setAttribute(Qt::AA_DontUseNativeMenuBar);
    Window w;
    w.show();
    return a.exec();
}
