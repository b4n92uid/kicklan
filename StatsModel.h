#ifndef STATSMODEL_H
#define STATSMODEL_H

#include <QList>
#include <QAbstractTableModel>

#include <tins/tins.h>

#include <QDateTime>

struct AppActivity {
    QString ip;
    int port;
    QString host;
    QString path;
    QDateTime date;
};

class TraficStats
{
public:
    QString ipSrc;
    QString ipDst;
    int size;
    bool srcIsPrivate;
    int portDst;
};

class UserStats
{
public:
    UserStats(QString addr);

    void recordUpload(int size);
    void recordDownload(int size);
    void recordTrafficType(int port);

    QString name;

    QString trafficType;

    QString ip;

    int downRate, upRate;

    QList<AppActivity> activity;

    static QString getTrafficTypeByPort(int port);

    static QString getHostByAddr(QString ip);

    typedef QMap<int, int> PortTraffic;

private:
    int m_downAcc, m_upAcc;
    qint64 m_dts, m_uts, m_tts;
    PortTraffic m_portTraffic;
};

class StatsModel : public QAbstractTableModel
{
public:
    StatsModel(QObject* parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    UserStats *find(QString addr);

    void insert(UserStats* us);
    void refresh();

private:
    QString humanBandwidth(int byte) const;

private:
    QList<UserStats*> m_data;
};

#endif // STATSMODEL_H
