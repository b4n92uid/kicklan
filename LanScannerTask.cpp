#include "LanScannerTask.h"
#include <QThreadPool>
#include <QThread>
#include "StatsModel.h"

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

    m_rangeSize = 0;
    m_status.clear();

    for(const IPv4Address& addr : range)
    {
        ArpRequest* arpr = new ArpRequest(m_nif, addr);
        connect(arpr, &ArpRequest::hostStatus, this, &LanScannerTask::reply, Qt::QueuedConnection);

        QThreadPool::globalInstance()->start(arpr);

        m_rangeSize++;
    }
}

void LanScannerTask::reply(IPv4Address addr, QString name, bool status)
{
    m_status[addr.to_string().data()] = status;

    if(status)
    {
        emit hostFound(addr, name);
    }

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
    QHostInfo host = UserStats::getHostByAddr(m_target.to_string());

    if(host.error() == QHostInfo::NoError)
        emit hostStatus(m_target, host.hostName(), true);
    else
        emit hostStatus(m_target, QString(), false);

}
