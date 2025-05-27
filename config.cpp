#include "config.h"
#include "ui_config.h"

Config::Config(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Config),
    serialPort(new QSerialPort(this))
{
    ui->setupUi(this);

    ui->buttonOpenPort->setText("OPEN");

    ui->plainTextBaudRate->appendPlainText("115200");

    show_Available_Ports();

    ui->labelOnOff->setFixedSize(40, 40);
    ui->labelOnOff->setStyleSheet("background-color: grey; border-radius: 20px;");
}

Config::~Config()
{
    delete ui;
}

void Config::on_buttonOpenPort_clicked()
{
    QString textoBaudrate = ui->plainTextBaudRate->toPlainText();
    bool ok;
    int baudRate = textoBaudrate.toInt(&ok);

    if(serialPort->isOpen()){
        serialPort->close();
        ui->buttonOpenPort->setText("OPEN");

        ui->labelOnOff->setFixedSize(40, 40);
        ui->labelOnOff->setStyleSheet("background-color: grey; border-radius: 20px;");
    }else{
        ui->labelOnOff->setFixedSize(40, 40);
        ui->labelOnOff->setStyleSheet("background-color: green; border-radius: 20px;");

        serialPort->setPortName(ui->comboBoxPorts->currentText());
        serialPort->setBaudRate(baudRate);
        if(serialPort->open(QSerialPort::ReadWrite)){
            emit config_Port(serialPort); // señal con el puerto
            emit conection_State(true); // puerto conectado
            //this->accept(); // cierra la ventana
            ui->buttonOpenPort->setText("CLOSE");
        }else {
            emit conection_State(false); // puerto conectado
            QMessageBox::critical(this, "Error", "No se pudo abrir el puerto serie.");
        }
    }
}

QSerialPort *Config::getSerialPort() {
    return serialPort;
}

void Config::show_Available_Ports()
{
    ui->comboBoxPorts->clear(); // Limpia el comboBox antes de agregar nuevos elementos

    const auto puertos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &puerto : puertos) {
        QString nombrePuerto = puerto.portName();  // Ej: COM3 o ttyUSB0
        ui->comboBoxPorts->addItem(nombrePuerto);  // Agrega el nombre como valor interno
    }

    if (ui->comboBoxPorts->count() == 0) {
        ui->comboBoxPorts->addItem("NO PORTS", "");
    }
}


void Config::on_pushButton_clicked()
{
    show_Available_Ports();
}

