#include "CachedTableExample.h"
#include <QtWidgets/QApplication>
#include "connection.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!createConnection())
        return EXIT_FAILURE;

    CachedTableExample editor("person");
    editor.show();
    return a.exec();
}
