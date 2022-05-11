#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "simplecrypt.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connectedToHost = false;
    readCfgFile();
    readKeyFile();

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

    readKeyFile();
    stringKey = hexToString(key);
    printMessage("<font color=Green>Connected to server: </font>" + serverIPAdress);
    printMessage("<font color=Yellow>Actual key: </font>" + stringKey);
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
    qDebug() << key << "key";
    return cryptMessage;
}


QString MainWindow::decryptText(QString message)
{
    SimpleCrypt crypto;
    crypto.setKey(key);
    QString decryptMessage = crypto.decryptToString(message);
    qDebug() << key << "key";
    qDebug() << decryptMessage;
    return decryptMessage;

}


void MainWindow::readCfgFile()
{
    QFile file("config.cfg");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        printMessage("<font color=\"Red\">No cfg file in program dicertory!</font>");
        qDebug() << "Error";
        return;
    }
    else
    {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream in(&file);
           while (!in.atEnd())
           {
                  serverIPAdress = in.readLine();
                  qDebug() << "Read cfg file" << serverIPAdress;
           }
    }
}

void MainWindow::readKeyFile()
{
    QFile file("key.cfg");
    if (!file.open(QIODevice::ReadOnly | QIODevice::ReadOnly))
    {
        printMessage("<font color=\"Red\">No key file in program dicertory!</font>");
        qDebug() << "Error";
        return;
    }
    else
    {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream in(&file);
           while (!in.atEnd())
           {
                  QString string_key = in.readLine();
                  key = stringToHex(string_key);
                  qDebug() << "Read key file" << string_key;
                  qDebug() << key;
           }
    }
}

quint64 MainWindow::stringToHex(QString password)
{
    quint64 key = 0;
    bool ok;
    key = password.toULongLong(&ok, 10);
    if(!ok)
    {
        qDebug() << "conversion falied!";
    }


    return key;
}

QString MainWindow::hexToString(quint64 key)
{
    QString string_key = QString::number(key);

    return string_key;
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


void MainWindow::on_actionServer_triggered()
{
    bool ok;
    QString IPAdress;
    IPAdress = QInputDialog::getText(this, "Server IP Adress", "IP or hostname", QLineEdit::Normal, "127.0.0.1", &ok);
    if (ok == true)
    {
        serverIPAdress = IPAdress;
        QFile file("config.cfg");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "Error";
            return;
        }
        else
        {
            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QTextStream out(&file); out << serverIPAdress;
            file.close();
        }
    }
    else
    {
        serverIPAdress = "";

        QMessageBox messagebox;
        messagebox.setText("<font color=\"Red\">Enter IP adress!</font>");
        messagebox.exec();
    }

    qDebug() << serverIPAdress;
    qDebug() << IPAdress;

}


void MainWindow::on_actionpasword_to_cryp_message_triggered()
{
    readKeyFile();
    stringKey = hexToString(key);
    bool ok;
    QString password = QInputDialog::getText(this, "Password", "Write onw password", QLineEdit::Normal, stringKey, &ok);
    if (ok == true)
    {
        QFile file("key.cfg");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "Error";
            return;
        }
        else
        {
            file.open(QIODevice::WriteOnly | QIODevice::Text);


            QTextStream out(&file); out << password;
            file.close();
            readKeyFile();
            stringKey = hexToString(key);
            printMessage("<font color=Yellow>Actual key: </font>" + stringKey);
        }

    }
    else
    {
        QMessageBox messagebox;
        messagebox.setText("<font color=\"Red\">Enter password!</font>");
        messagebox.exec();
    }

}

