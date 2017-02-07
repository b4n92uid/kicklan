#ifndef LANSCANNERTASK_H
#define LANSCANNERTASK_H

#include <QRunnable>
#include <QHostInfo>

#include <tins/tins.h>

class ArpRequest : public QObject, public QRunnable
{
    Q_OBJECT

public:
    ArpRequest(Tins::NetworkInterface nif, Tins::IPv4Address target);

signals:
    void hostStatus(Tins::IPv4Address, QString, bool);

protected:
    void run();

private:
    Tins::NetworkInterface m_nif;
    Tins::IPv4Address m_target;
};

class LanScannerTask : public QObject, public QRunnable
{
    Q_OBJECT

public:
    LanScannerTask(Tins::NetworkInterface nif);

public slots:
    void reply(Tins::IPv4Address addr, QString name, bool status);

protected:
    void run();

signals:
    void finished();
    void hostFound(Tins::IPv4Address, QString);

private:
    Tins::NetworkInterface m_nif;
    int m_rangeSize;

    QMap<QString, bool> m_status;
};

#endif // LANSCANNERTASK_H
