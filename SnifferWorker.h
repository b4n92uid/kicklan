#ifndef SNIFFERWORKER_H
#define SNIFFERWORKER_H

#include <QObject>
#include <QDateTime>

#include <tins/tins.h>
#include <tins/tcp_ip/stream_follower.h>

#include "StatsModel.h"

class SnifferWorker : public QObject
{
    Q_OBJECT

public:
    SnifferWorker(QObject* parent = 0);

    bool processPacket(Tins::PDU& pdu);

    void processNewConnection(Tins::TCPIP::Stream& stream);
    void processServerData(Tins::TCPIP::Stream& stream);
    void processClientData(Tins::TCPIP::Stream& stream);

signals:
    void stats(TraficStats ts);
    void activity(AppActivity aa);

public slots:
    void startSniffer(Tins::NetworkInterface nif);
    void stopSniffer();

private:
    Tins::Sniffer* m_sniffer;
    Tins::TCPIP::StreamFollower m_follower;
    bool m_running;
};

#endif // SNIFFERWORKER_H
