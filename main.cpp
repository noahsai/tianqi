#include "tianqi.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    tianqi w;
    w.show();

    return a.exec();
}
