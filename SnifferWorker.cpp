#include "SnifferWorker.h"

#include <QDebug>
#include <QCoreApplication>
#include <QRegularExpression>

#include <functional>
#include <iostream>

using namespace Tins;
using namespace std;

using Tins::TCPIP::Stream;
using Tins::TCPIP::StreamFollower;

SnifferWorker::SnifferWorker(QObject *parent) : QObject(parent)
{
    m_running = false;
    m_sniffer = NULL;

    using std::placeholders::_1;
    m_follower.new_stream_callback(std::bind(&SnifferWorker::processNewConnection, this, _1));
}

void SnifferWorker::startSniffer(NetworkInterface nif)
{
    qDebug() << "startSniffer";

    SnifferConfiguration config;
    config.set_promisc_mode(true);
    config.set_immediate_mode(true);
    config.set_filter("tcp");

    m_running = true;

    m_sniffer = new Sniffer(nif.name(), config);
    m_sniffer->sniff_loop(make_sniffer_handler(this, &SnifferWorker::processPacket));
}

void SnifferWorker::stopSniffer()
{
    qDebug() << "stopSniffer";

    m_running = false;
    delete m_sniffer, m_sniffer = NULL;
}

bool SnifferWorker::processPacket(PDU &pdu)
{
    try {
        IP& ip = pdu.rfind_pdu<IP>();
        TCP& tcp = ip.rfind_pdu<TCP>();

        TraficStats ts;
        ts.srcIsPrivate = ip.src_addr().is_private();
        ts.ipSrc= QString::fromStdString(ip.src_addr().to_string());
        ts.ipDst= QString::fromStdString(ip.dst_addr().to_string());
        ts.size = pdu.size();
        ts.portDst = tcp.dport();

        m_follower.process_packet(pdu);

        emit stats(ts);

    } catch (pdu_not_found& e) {
        qWarning() << e.what();
    }

    QCoreApplication::processEvents(QEventLoop::AllEvents, 1);

    return m_running;
}

void SnifferWorker::processNewConnection(Stream &stream)
{
    using std::placeholders::_1;

    stream.client_data_callback(std::bind(&SnifferWorker::processClientData, this, _1));
    stream.server_data_callback(std::bind(&SnifferWorker::processServerData, this, _1));
    // Don't automatically cleanup the stream's data, as we'll manage
    // the buffer ourselves and let it grow until we see a full request
    // and response
    stream.auto_cleanup_payloads(false);

}

void SnifferWorker::processServerData(TCPIP::Stream &stream)
{
//    Stream::payload_type payload = stream.server_payload();
//    QString content = QString::fromLocal8Bit((const char*)payload.data());
}

void SnifferWorker::processClientData(TCPIP::Stream &stream)
{
    Stream::payload_type payload = stream.client_payload();
    QString content = QString::fromLocal8Bit((const char*)payload.data());

    QRegularExpression matchHost("GET (.+) HTTP\\/1\\.1\\r\\nHost: (.+)\\r\\n");

    AppActivity aa;

    aa.ip = QString::fromStdString(stream.client_addr_v4().to_string());
    aa.port = stream.client_port();

    {
        QRegularExpressionMatch match = matchHost.match(content);

        if(match.hasMatch())
        {
            aa.path = match.captured(1);
            aa.host = match.captured(2);
        }
    }

    aa.date = QDateTime::currentDateTime();

    emit activity(aa);
}
