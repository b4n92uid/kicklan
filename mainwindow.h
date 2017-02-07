#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTimer>
#include <QThread>
#include <QHostInfo>

#include <tins/tins.h>

#include "ArpSpoofing.h"
#include "StatsModel.h"
#include "SnifferWorker.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void init();

signals:
    void startSniffer(Tins::NetworkInterface nif);
    void stopSniffer();

public slots:
    void log(QtMsgType type, QString msg);

    void selectInterfaceMenu();
    void selectInterface(Tins::NetworkInterface nif);

    void toggleSpoofing(bool status);
    void toggleForward(bool status);

    void updateStatus();

    void parseStats(TraficStats ts);
    void parseActivity(AppActivity aa);

    void scaneLan();
    void scaneLanFound(Tins::IPv4Address host, QString hostname);

private:
    Ui::MainWindow *ui;

    std::vector<Tins::NetworkInterface> m_nifList;
    Tins::NetworkInterface m_nifDefault;

    QStandardItemModel* m_logModel;

    QTimer m_workerTimer;
    ArpSpoofing* m_spoofer;

    SnifferWorker* m_sniffer;

    StatsModel* m_statsModel;

    QThread m_workerThread;
};

#endif // MAINWINDOW_H
