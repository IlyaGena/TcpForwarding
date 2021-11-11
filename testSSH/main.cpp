#include <QCoreApplication>

#include "Test.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Test test(argv[1]);

    return a.exec();
}
