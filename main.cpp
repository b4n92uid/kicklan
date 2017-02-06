#include "MainWindow.h"
#include <QApplication>
#include <QStyleFactory>
#include "StatsModel.h"

Q_DECLARE_METATYPE(Tins::NetworkInterface)
Q_DECLARE_METATYPE(TraficStats)
Q_DECLARE_METATYPE(AppActivity)

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("Fusion"));

    qRegisterMetaType<Tins::NetworkInterface>();
    qRegisterMetaType<TraficStats>();
    qRegisterMetaType<AppActivity>();

    MainWindow w;
    w.show();

    return a.exec();
}
