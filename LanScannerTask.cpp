#include "LanScannerTask.h"
#include <QThreadPool>

using namespace Tins;

LanScannerTask::LanScannerTask(Tins::NetworkInterface nif)
{
    m_nif = nif;
    m_rangeSize = 0;
}

void LanScannerTask::run()
{
    IPv4Address gateway;
    Tins::Utils::gateway_from_ip(m_nif.ipv4_address(), gateway);

    IPv4Range range = IPv4Range::from_mask(gateway, m_nif.ipv4_mask());

    for(const IPv4Address& addr : range)
    {
        ArpRequest* arpr = new ArpRequest(m_nif, addr);
        connect(arpr, &ArpRequest::hostStatus, this, &LanScannerTask::reply);

        QThreadPool::globalInstance()->start(arpr);

        m_rangeSize++;
    }
}

void LanScannerTask::reply(IPv4Address addr, bool status)
{
    m_status[addr.to_string().data()] = status;

    qDebug() << "REPLY" << addr.to_string().data() << status;

    if(status)
        emit hostFound(addr);

    if(m_status.count() >= m_rangeSize)
        emit finished();
}

ArpRequest::ArpRequest(NetworkInterface nif, IPv4Address target)
{
    m_nif = nif;
    m_target = target;
}

void ArpRequest::run()
{
    qDebug() << "TEST" << m_target.to_string().data();

    try {
        PacketSender sender;
        Utils::resolve_hwaddr(m_nif, m_target, sender);

        emit hostStatus(m_target, true);

    } catch (std::exception&) {
        emit hostStatus(m_target, false);


    }
}
