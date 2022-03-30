#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connectedToHost = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_Connect_clicked()
{
    if(!connectedToHost)
    {
        socket = new QTcpSocket();

        connect(socket, &QTcpSocket::connected, this, &MainWindow::socketConnected);
        connect(socket, &QTcpSocket::disconnected, this, &MainWindow::socketDisconnected);
        connect(socket, &QTcpSocket::readyRead, this, &MainWindow::socketReadyRead);


        socket->connectToHost("127.0.0.1", 8001);
    }
    else
    {
        QString name = ui->lineEdit_nameInput->text();
        socket->write("<font color=\"Orange\">" + name.toUtf8() + " has left the chat room.</font>");

        socket->disconnectFromHost();
    }
}


void MainWindow::socketConnected()
{
    qDebug() << "Connect to server.";

    printMessage("<font color=Green>Connected to server.</font>");

    QString name = ui->lineEdit_nameInput->text();
    socket->write("<font color=\"Purple\">" + name.toUtf8() + " has joined the chat room.</font>");

    ui->pushButton_Connect->setText("Disconnect");

    connectedToHost = true;
}


void MainWindow::socketDisconnected()
{
    qDebug() << "Disconnected from serwer";

    printMessage("<font color=\"Red\">Disconnected from server.</font>");
    ui->pushButton_Connect->setText("Connect");

    connectedToHost = false;
}


void MainWindow::socketReadyRead()
{
    ui->textBrowser_ChatDisplay->append(socket->readAll());
}


void MainWindow::printMessage(QString message)
{
    ui->textBrowser_ChatDisplay->append(message);
}


void MainWindow::on_pushButton_Send_clicked()
{
    QString name = ui->lineEdit_nameInput->text();
    QString message = ui->lineEdit_messegeInput->text();
    socket->write("<font color=\"Blue\">" + name.toUtf8() + "</font>: " + message.toUtf8());

    ui->lineEdit_messegeInput->clear();
}

