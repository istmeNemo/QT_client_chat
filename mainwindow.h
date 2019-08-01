#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QStringListModel>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QDateTime>
#include <QInputDialog>
#include <QTcpSocket>
class QJsonDocument;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void HelloUDP();
private slots:


    void on_sendMbutton_clicked();

    void setIp();
    void readyRead();
    void dataSender(const QJsonObject &message,const QHostAddress,quint16 port);
    void addMessage(const QString);
    void listCreatorClient(const QJsonObject &message);
    void refresh();
    void on_pushButton_clicked();
    void on_listView_activated(const QModelIndex &index);
    void on_listView_clicked(const QModelIndex &index);
    void on_connectButton_clicked();

private:
    Ui::MainWindow *ui;
    QUdpSocket *socket;
    QStringListModel *model;
    QStringListModel *users;
    QStringList List;
    QStringList ListUsers;
    QString userName="bATMAN";
    QHostAddress localAddr=QHostAddress::LocalHost;
    QJsonArray listUsers;
    QHostAddress addressToSend=QHostAddress::LocalHost;
    quint16 portToSend=1234;
    QHostAddress addressTracker=QHostAddress::LocalHost;//server
    quint16 portTracker=26000;
    //void initSocket();
    //void readPendingDatagrams();
};

#endif // MAINWINDOW_H
