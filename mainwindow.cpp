#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QDebug>
#include <QDateTime>
#include <QTimer>

using namespace Tins;
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_statsModel = new StatsModel(this);
    ui->statsView->setModel(m_statsModel);

    m_logModel = new QStandardItemModel(this);
    ui->logView->setModel(m_logModel);

    connect(ui->kicklan, SIGNAL(clicked(bool)), this, SLOT(toggleSpoofing(bool)));
    connect(ui->stopForward, SIGNAL(clicked(bool)), this, SLOT(toggleForward(bool)));

    m_spoofing = new ArpSpoofing(this);
    connect(&m_workerTimer, SIGNAL(timeout()), m_spoofing, SLOT(doSpoofing()));

    m_sniffer = new SnifferWorker;
    connect(this, &MainWindow::startSniffer, m_sniffer, &SnifferWorker::startSniffer);
    connect(this, &MainWindow::stopSniffer, m_sniffer, &SnifferWorker::stopSniffer);

    connect(m_sniffer, &SnifferWorker::emitStats, this, &MainWindow::parseStats);
    connect(m_sniffer, &SnifferWorker::emitActivity, this, &MainWindow::parseActivity);

    m_sniffer->moveToThread(&m_workerThread);

    m_workerThread.start();

    init();
}

MainWindow::~MainWindow()
{
    m_workerTimer.stop();

    emit stopSniffer();
    m_sniffer->deleteLater();

    m_workerThread.quit();
    m_workerThread.wait();

    delete ui;
}

void MainWindow::init()
{
    log(QtDebugMsg, QString("Start at %1").arg(QDateTime::currentDateTime().toString("yyyy/MM/dd HH:mm:ss")));

    log(QtDebugMsg, "KickLan initializing...");

    log(QtDebugMsg, "Fetch available interfaces");

    selectInterface(NetworkInterface::default_interface());

    QMenu* menuInterfaces = new QMenu(tr("Interfaces"), this);
    QActionGroup* group = new QActionGroup(menuInterfaces);

    m_nifList = NetworkInterface::all();
    foreach(NetworkInterface nif, m_nifList)
    {
        QString nif_name = QString::fromStdWString(nif.friendly_name());

        log(QtDebugMsg, QString("Detected `%1` interface").arg(nif_name));

        QAction* nif_action = new QAction(nif_name, menuInterfaces);
        nif_action->setCheckable(true);
        nif_action->setActionGroup(group);
        nif_action->setData(nif.id());

        connect(nif_action, SIGNAL(triggered(bool)), this, SLOT(selectInterfaceMenu(bool)));

        if(nif == m_nifDefault)
            nif_action->setChecked(true);

        menuInterfaces->addAction(nif_action);
    }

    ui->menubar->addMenu(menuInterfaces);
}

void MainWindow::log(QtMsgType type, QString msg)
{
    qDebug() << msg;

    QStandardItem* item = new QStandardItem(msg);

    switch(type) {
        case QtDebugMsg:
        break;

        case QtInfoMsg:
        item->setIcon(QIcon(":/icons/assets/info.png").pixmap(QSize(16, 16)));
        item->setBackground(QBrush(QColor(153,217,234), Qt::Dense4Pattern));
        break;

        case QtWarningMsg:
        item->setIcon(QIcon(":/icons/assets/warning.png").pixmap(QSize(16, 16)));
        item->setBackground(QBrush(QColor(255,200,16), Qt::Dense4Pattern));
        break;

        case QtCriticalMsg:
        item->setIcon(QIcon(":/icons/assets/critical.png").pixmap(QSize(16, 16)));
        item->setBackground(QBrush(QColor(255,200,200), Qt::Dense4Pattern));
        break;
    }

    m_logModel->appendRow(item);
}

void MainWindow::selectInterfaceMenu(bool status)
{
    int index = qobject_cast<QAction*>(sender())->data().toInt();

    NetworkInterface nif = NetworkInterface::from_index(index);
    selectInterface(nif);
}

void MainWindow::selectInterface(NetworkInterface nif)
{
    log(QtInfoMsg, QString("Select `%1`").arg(QString::fromStdWString(nif.friendly_name())));

    m_nifDefault = nif;

    updateStatus();
}

void MainWindow::toggleSpoofing(bool status)
{
    if(status)
    {
        log(QtInfoMsg, "Start monitoring");

        m_workerTimer.start(ui->rate->value());
        ui->rate->setEnabled(false);

        emit startSniffer(m_nifDefault);
    }
    else
    {
        m_workerTimer.stop();
        ui->rate->setEnabled(true);

        emit stopSniffer();

        log(QtInfoMsg, "End monitoring");
    }
}

void MainWindow::toggleForward(bool status)
{

}

void MainWindow::updateStatus()
{
    QString statustpl(
                "Inteface: <b>%1</b><br />"
                "IP: <b>%2</b><br />"
                "MAC: <b>%3</b>"
    );

    statustpl = statustpl
            .arg(QString::fromStdWString(m_nifDefault.friendly_name()))
            .arg(QString::fromStdString(m_nifDefault.ipv4_address().to_string()))
            .arg(QString::fromStdString(m_nifDefault.hw_address().to_string()));

    ui->status->setText(statustpl);
}

void MainWindow::parseStats(TraficStats ts)
{
    if(ts.srcIsPrivate)
    {
        UserStats* srcus = m_statsModel->find(ts.ipSrc);

        if(srcus)
        {
            srcus->recordUpload(ts.size);
            srcus->recordTrafficType(ts.portDst);

            m_statsModel->refresh();
        }

        else
        {
            srcus = new UserStats(ts.ipSrc);

            srcus->name = UserStats::getHostByAddr(ts.ipSrc);
            srcus->recordDownload(ts.size);
            srcus->recordTrafficType(ts.portDst);

            m_statsModel->insert(srcus);
        }
    }

    // Record download badnwidth
    UserStats* dstus = m_statsModel->find(ts.ipDst);

    if(dstus)
    {
        dstus->recordDownload(ts.size);
        m_statsModel->refresh();
    }

}

void MainWindow::parseActivity(AppActivity aa)
{
    UserStats* us = m_statsModel->find(aa.ip);

    us->activity << aa;
}