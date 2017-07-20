#include "MainWindow.h"
#include <QApplication>
#include <QStyleFactory>
#include "StatsModel.h"

Q_DECLARE_METATYPE(Tins::NetworkInterface)
Q_DECLARE_METATYPE(Tins::IPv4Address)
Q_DECLARE_METATYPE(TraficStats)
Q_DECLARE_METATYPE(AppActivity)

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("Fusion"));

    qRegisterMetaType<Tins::NetworkInterface>();
    qRegisterMetaType<Tins::IPv4Address>();
    qRegisterMetaType<TraficStats>();
    qRegisterMetaType<AppActivity>();

    MainWindow w;
    w.init();
    w.show();
    w.scaneLan();

    return a.exec();
}
