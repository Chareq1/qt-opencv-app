#include "mojeobrazki.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MojeObrazki w;
    w.show();
    return a.exec();
}
