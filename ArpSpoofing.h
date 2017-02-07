#ifndef ARPSPOOFING_H
#define ARPSPOOFING_H

#include <QObject>
#include <QMap>

#include <tins/tins.h>

class ArpSpoofing : public QObject
{
    Q_OBJECT
public:
    explicit ArpSpoofing(QObject *parent = 0);

    Tins::NetworkInterface interface() const;
    void setInterface(const Tins::NetworkInterface &interface);

    Tins::IPv4Address gatewayIp() const;
    void setGatewayIp(const Tins::IPv4Address &gatewayIp);

    void enableIp(QString ip, bool enabled);
    void enableIp(Tins::IPv4Address ip, bool enabled);

signals:

public slots:
    void doSpoofing();

private:

private:
    Tins::IPv4Address m_gatewayIp;
    Tins::NetworkInterface m_interface;
    QMap<Tins::IPv4Address, bool> m_iplist;
};

#endif // ARPSPOOFING_H
