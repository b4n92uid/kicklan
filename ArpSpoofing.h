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

    void init(const Tins::NetworkInterface &interface);

    void enableIp(QString ip, bool enabled);
    void enableIp(Tins::IPv4Address ip, bool enabled);

signals:

public slots:
    void doSpoofing();

private:

private:
    typedef Tins::IPv4Address Logical;
    typedef Tins::EthernetII::address_type Hardware;

    Logical m_gatewayIp;
    Hardware m_gatewayHw;

    Tins::NetworkInterface m_interface;
    QMap<Logical, bool> m_iplist;
    QMap<Logical, Hardware> m_iphw;

    Tins::PacketSender m_sender;
};

#endif // ARPSPOOFING_H
