#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_Connect_clicked();
    void on_pushButton_Send_clicked();

    void socketConnected();
    void socketDisconnected();
    void socketReadyRead();

private:
    Ui::MainWindow *ui;
    bool connectedToHost;
    QTcpSocket* socket;

    void printMessage(QString);
};
#endif // MAINWINDOW_H
