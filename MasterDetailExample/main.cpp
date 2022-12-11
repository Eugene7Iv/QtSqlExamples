#include "MasterDetailExample.h"
#include <QtWidgets/QApplication>
#include <QFile>
#include "database.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!createConnection())
        return EXIT_FAILURE;

    QFile albumDetails("albumdetails.xml");
    MasterDetailExample w("artists", "albums", &albumDetails);
    w.show();
    return a.exec();
}
