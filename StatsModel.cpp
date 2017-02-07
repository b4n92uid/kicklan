#include "StatsModel.h"

#include <QDebug>

StatsModel::StatsModel(QObject *parent) : QAbstractTableModel(parent)
{

}

int StatsModel::rowCount(const QModelIndex&) const
{
    return m_data.size();
}

int StatsModel::columnCount(const QModelIndex&) const
{
    return 6;
}

QVariant StatsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();

    if(orientation == Qt::Horizontal)
    {
        switch(section)
        {
        case 0: return tr("Name"); break;
        case 1: return tr("IP"); break;
        case 2: return tr("Traffic Type"); break;
        case 3: return tr("Down Rate"); break;
        case 4: return tr("Up Rate"); break;
        case 5: return tr("Activity"); break;
        }
    }

    return QVariant();
}

QVariant StatsModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DisplayRole)
    {
        if(index.row() >= m_data.size())
            return QVariant();

        UserStats* us = m_data[index.row()];

        switch(index.column())
        {
        case 0:
            return us->name;

        case 1:
            return us->ip;

        case 2:
            return us->trafficType;

        case 3:
            return humanBandwidth(us->downRate);

        case 4:
            return humanBandwidth(us->upRate);

        case 5: {
                if(!us->activity.isEmpty())
                {
                    AppActivity& ac = us->activity.last();
                    return ac.host;
                }
            }
            break;
        }

    }

    return QVariant();
}

UserStats* StatsModel::find(QString addr)
{
    foreach(UserStats* us, m_data)
    {
        if(us->ip == addr)
            return us;
    }

    return NULL;
}

void StatsModel::insert(UserStats *us)
{
    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    m_data << us;
    endInsertRows();
}

void StatsModel::refresh()
{
    emit dataChanged(QModelIndex(), QModelIndex());
}

QString StatsModel::humanBandwidth(int byte) const
{
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("bytes");

    while(byte >= 1024 && i.hasNext())
     {
        unit = i.next();
        byte /= 1024;
    }
    return QString().setNum(byte)+" "+unit;
}

UserStats::UserStats(QString addr) : downRate(0), upRate(0), m_downAcc(0), m_upAcc(0), m_dts(0), m_uts(0), m_tts(0)
{
    ip = addr;
}

void UserStats::recordUpload(int size)
{
    qint64 now = QDateTime::currentDateTime().toMSecsSinceEpoch();

    if((now - m_uts) > 1000)
    {
        m_uts = now;
        upRate = m_upAcc;
        m_upAcc = 0;
    }
    else
    {
        m_upAcc += size;
    }
}

void UserStats::recordDownload(int size)
{
    qint64 now = QDateTime::currentDateTime().toMSecsSinceEpoch();

    if((now - m_dts) > 1000)
    {
        m_dts = now;
        downRate = m_downAcc;
        m_downAcc = 0;
    }
    else
    {
        m_downAcc += size;
    }
}

void UserStats::recordTrafficType(int port)
{
    qint64 now = QDateTime::currentDateTime().toMSecsSinceEpoch();

    if(m_portTraffic.contains(port))
        m_portTraffic[port]++;
    else
        m_portTraffic[port] = 1;

    if((now - m_tts) > 4000)
    {
        using namespace std;

        m_tts = now;

        auto stdmap = m_portTraffic.toStdMap();

        auto max = std::max_element(stdmap.begin(), stdmap.end(),
                                    [](pair<const int, int>& p1, pair<const int, int>& p2) {
                    return p1 <p2;
                });

        this->trafficType = getTrafficTypeByPort(max->first);

        m_portTraffic.clear();
    }
}

QString UserStats::getTrafficTypeByPort(int port)
{
    switch(port) {
      case 80:
        return "Browsing";

      case 443:
        return "Browsing HTTPS";

      case 20:
      case 21:
        return "FTP";

      case 22:
        return "SSH";

      case 25:
      case 109:
      case 110:
        return "Mailing";

      case 5938:
        return "TeamViewer";

      default:
        return QString("Port %1").arg(port);
    }
}

QHostInfo UserStats::getHostByAddr(std::string ip)
{
    return getHostByAddr(QString::fromStdString(ip));
}

QHostInfo UserStats::getHostByAddr(QString ip)
{
    in_addr addr;
    addr.s_addr = inet_addr(ip.toLocal8Bit().data());

    hostent* host = gethostbyaddr((char*)&addr, 4, AF_INET);

    QHostInfo info;

    if(host)
    {
        info.setHostName(host->h_name);
    }
    else
    {
        DWORD dwError = WSAGetLastError();

        switch(dwError)
        {
        case WSAHOST_NOT_FOUND:
            info.setError(QHostInfo::HostNotFound);
            break;

        default:
            info.setError(QHostInfo::UnknownError);
        }
    }

    return info;
}
