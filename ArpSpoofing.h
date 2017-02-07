#ifndef ARPSPOOFING_H
#define ARPSPOOFING_H

#include <QObject>

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

signals:

public slots:
    void doSpoofing();

private:

private:
    Tins::IPv4Address m_gatewayIp;
    Tins::NetworkInterface m_interface;
};

#endif // ARPSPOOFING_H
