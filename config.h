#ifndef CONFIG_H
#define CONFIG_H

#include <QDialog>
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

namespace Ui {
class Config;
}

class Config : public QDialog
{
    Q_OBJECT

public:
    explicit Config(QWidget *parent = nullptr);
    ~Config();

    QSerialPort *getSerialPort(); // <- método para acceder al puerto
signals:
    void config_Port(QSerialPort *serial); // señal que envía el puerto

    void conection_State(bool conected); // señal para indicar estado

private slots:
    void on_buttonOpenPort_clicked();
    void show_Available_Ports();

    void on_pushButton_clicked();

private:
    Ui::Config *ui;
    QSerialPort *serialPort;
};

#endif // CONFIG_H
