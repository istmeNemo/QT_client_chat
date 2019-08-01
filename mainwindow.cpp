#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QHostAddress>
#include <QDateTime>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setIp();

    socket = new QUdpSocket(this);
    socket -> bind(localAddr,1234);
    connect(socket, SIGNAL(readyRead()),this, SLOT(readyRead()));
    model = new QStringListModel(this);//dodanie do listy
     List<<"Message from: ";
    model->setStringList(List);
    ui->chatView->setModel(model);
    users=new QStringListModel(this);
    ListUsers<<"Users:";


    users->setStringList(ListUsers);
    ui->listView->setModel(users);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_sendMbutton_clicked()//must add to which user/ip send
{
    QString text =ui->lineEdit->text();
    if(text.isEmpty())//prevent sending empty message
        return;
    QJsonObject message;
    message["type"] = QStringLiteral("message");
    message["username"] =userName;
    message["text"]=text;
    ui->lineEdit->clear();
    addMessage("ME:"+text);
    dataSender(message,addressToSend,portToSend);//change to user /*****remeber***
    qDebug()<<"LOCAL IP IS:........."<<localAddr;
}

void MainWindow::setIp()
{
    QTcpSocket socketTest;

    socketTest.connectToHost("8.8.8.8", 53); // google DNS, or something else reliable
    if (socketTest.waitForConnected()) {
        qDebug()
            << "local IPv4 address for Internet connectivity is"
            << socketTest.localAddress();
    } else {
        qWarning()
            << "could not determine local IPv4 address:"
            << socketTest.errorString();
    }
    localAddr=socketTest.localAddress();
}



void MainWindow::HelloUDP()
{
    QJsonObject message;
    message["type"] = QStringLiteral("connect");
    message["username"] = userName ;
    message["text"]="Hello serwer";
    socket->writeDatagram( QJsonDocument(message).toJson(QJsonDocument::Compact),addressTracker,portTracker);
}

void MainWindow::readyRead()
{
    QByteArray buffer;


    buffer.resize(socket->pendingDatagramSize());//ustawienie jako pierwszy pakiet przychodzacy udp wielkosc

    QHostAddress sender;
    quint16 senderPort;

    socket->readDatagram(buffer.data(), buffer.size(),&sender, &senderPort);

    QJsonObject docObj = (QJsonDocument::fromJson(buffer)).object();//convert to json data
    const QJsonValue typeVal = docObj.value(QLatin1String("type"));

    if (typeVal.isNull() || !typeVal.isString())
        return; // not serialized data


    QString typeMessage = typeVal.toString();
    qDebug() << "How work typeMessage" << typeMessage;





    if(typeMessage.compare(QLatin1String("message"), Qt::CaseInsensitive) == 0)
    {
        QString text = docObj.value("username").toString()+": "+docObj.value(QLatin1String("text")).toString();
        addMessage(text);
        qDebug() << "its called"<<text;
    }

    if(typeMessage.compare(QLatin1String("userslist"), Qt::CaseInsensitive) == 0)//server
    {
        qDebug() << "server answer to list";
        listCreatorClient(docObj);
    }
    if(typeMessage.compare(QLatin1String("refresh"), Qt::CaseInsensitive) == 0)
    {
        refresh();
    }
    qDebug() << "Message from: " << sender.toString();
    qDebug() << "Message port: " << senderPort;
    qDebug() << "Message: " << buffer;
}



void MainWindow::dataSender(const QJsonObject &message, const QHostAddress addr, quint16 port)
{
    socket->writeDatagram(QJsonDocument(message).toJson(QJsonDocument::Compact),addr,port);
}

void MainWindow::addMessage(const QString message)
{
    int row = model->rowCount();

        // Enable add one or more rows
        model->insertRows(row,1);

        // Get the row for Edit mode
        QModelIndex index = model->index(row);

        // Enable item selection and put it edit mode
        ui->chatView->setCurrentIndex(index);
        model->setData(model->index(row, 0), message);
}



void MainWindow::listCreatorClient(const QJsonObject &message)//client function
{
    int j=0;
    const QString ipValue = message.value("ip").toString();
    qDebug()<<"what ip"<<ipValue;

    while(j<listUsers.count())
    {
        if(listUsers[j].toObject().value("ip").toString().compare(ipValue,Qt::CaseInsensitive)==0)
        {
          break;
        }
            j++;
    }
    qDebug()<<"WARTOS WIELKOSCI LIST:"<<listUsers.count();
    qDebug()<<"WARTOS J"<<j;



    if(j<listUsers.count())
    {listUsers.replace(j,message);
    qDebug()<<"replace ip";}
    else {
        listUsers.append(message);
        qDebug()<<"append";
    }
    qDebug()<<"display LISTUSERS: "<<listUsers;


}

void MainWindow::on_pushButton_clicked()//display list user
{
     QJsonObject temp;
     temp["type"]=QStringLiteral("list");
     dataSender(temp,addressTracker,portTracker);
}
void MainWindow::refresh()
{
    qDebug()<<"Display listUser:"<<listUsers;
    users->setStringList(ListUsers);
    int i =0;
    while(i<listUsers.count())
    {
    int row = users->rowCount();

        // Enable add one or more rows
        users->insertRows(row,1);

        // Get the row for Edit mode
        QModelIndex index = users->index(row);

        // Enable item selection and put it edit mode
        ui->listView->setCurrentIndex(index);
        users->setData(users->index(row, 0), listUsers[i].toObject().value("username").toString());
        i++;
    }
}
void MainWindow::on_listView_activated(const QModelIndex &index)
{

    qDebug()<<"return number of index:::"<<index;
}

void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    if(index.row()==0)
        return;
    addressToSend.setAddress(listUsers[index.row()-1].toObject().value("ip").toString());
    qDebug()<<"return number of index CLICKED:::"<<index.row();
}



void MainWindow::on_connectButton_clicked()
{
    const QString hostAddress = QInputDialog::getText(
        this
        , tr("Chose Server")
        , tr("Server Address")
        , QLineEdit::Normal
        , QStringLiteral("127.0.0.1")
    );

    if (hostAddress.isEmpty())
        return; // the user pressed cancel or typed nothing
    // disable the connect button to prevent the user clicking it again
     const QString newUsername = QInputDialog::getText(this, tr("Chose Username"), tr("Username"));
    if (newUsername.isEmpty())
        return;
    userName=newUsername;
    addressTracker.setAddress(hostAddress);
    HelloUDP();
    //ui->connectButton->setEnabled(false);
    ui->lineEdit->setEnabled(true);
}
