#include "WidgetMapperExample.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WidgetMapperExample w;
    w.show();
    return a.exec();
}
