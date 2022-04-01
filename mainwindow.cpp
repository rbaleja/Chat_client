#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "simplecrypt.h"

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
    if(ui->lineEdit_nameInput->text().isEmpty())
    {

        QMessageBox messagebox;
        messagebox.setText("<font color=Red>Enter your name.</font>");
        messagebox.exec();

    }
    else
    {
        if(!connectedToHost)
        {
            socket = new QTcpSocket();

            connect(socket, &QTcpSocket::connected, this, &MainWindow::socketConnected);
            connect(socket, &QTcpSocket::disconnected, this, &MainWindow::socketDisconnected);
            connect(socket, &QTcpSocket::readyRead, this, &MainWindow::socketReadyRead);

            socket->connectToHost(serverIPAdress, 8001);
        }
        else
        {
            QString name = ui->lineEdit_nameInput->text();
            ui->textBrowser_ChatDisplay->append("<font color=\"Orange\">" + name.toUtf8() + " has left the chat room.</font>: ");

            socket->disconnectFromHost();
        }

    }
}


void MainWindow::socketConnected()
{
    qDebug() << "Connect to server.";

    printMessage("<font color=Green>Connected to server.</font>");

    QString name = ui->lineEdit_nameInput->text();
    ui->textBrowser_ChatDisplay->append("<font color=\"Purple\">" + name.toUtf8() + " has joined the chat room.</font>");;
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
    QString cryptMessage(socket->readAll());
    QString decryptMessage = decryptText(cryptMessage);
    ui->textBrowser_ChatDisplay->append(decryptMessage);

}


QString MainWindow::cryptText(QString message)
{
    SimpleCrypt crypto;
    crypto.setKey(key);
    QString cryptMessage = crypto.encryptToString(message);
    qDebug() << cryptMessage;
    return cryptMessage;
}


QString MainWindow::decryptText(QString message)
{
    SimpleCrypt crypto;
    crypto.setKey(key);
    QString decryptMessage = crypto.decryptToString(message);

    return decryptMessage;
}


void MainWindow::printMessage(QString message)
{
    ui->textBrowser_ChatDisplay->append(message);
}


void MainWindow::on_pushButton_Send_clicked()
{
    QString name = ui->lineEdit_nameInput->text();
    QString message = ui->lineEdit_messegeInput->text();
    if (ui->lineEdit_messegeInput->text().isEmpty())
    {
        QMessageBox messagebox;
        messagebox.setText("<font color=\"Red\">Enter your message.</font>");
        messagebox.exec();
    }
    else
    {
        QString messageToCrypt = "<font color=\"Blue\">" + name.toUtf8() + "</font>: " + message.toUtf8();
        QString cryptMessage = cryptText(messageToCrypt);

        socket->write(cryptMessage.toUtf8());

        ui->textBrowser_ChatDisplay->append("<font color=\"Red\">Encrypted message sent.</font>");
        ui->lineEdit_messegeInput->clear();
    }
}


void MainWindow::on_lineEdit_messegeInput_returnPressed()
{
    ui->pushButton_Send->click();
}


void MainWindow::on_lineEdit_nameInput_returnPressed()
{
    ui->pushButton_Connect->click();
}

