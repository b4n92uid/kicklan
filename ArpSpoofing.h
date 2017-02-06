#ifndef ARPSPOOFING_H
#define ARPSPOOFING_H

#include <QObject>

class ArpSpoofing : public QObject
{
    Q_OBJECT
public:
    explicit ArpSpoofing(QObject *parent = 0);

signals:

public slots:
    void doSpoofing();

private:
    bool m_running;
};

#endif // ARPSPOOFING_H
