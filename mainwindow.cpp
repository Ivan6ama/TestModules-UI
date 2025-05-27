#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "config.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    ui->tabWidget->setCurrentIndex(2);  // Muestra la primera pestaña

    ui->comboBoxData->addItem("ALIVE", 0xF0);
    ui->comboBoxData->addItem("UIMOTORS", 0x01);
    ui->comboBoxData->addItem("UIDISPLAY", 0x02);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_serialPort_Rx()
{
    unsigned char *incomingBuffer;
    int count;

    count = serialPort->bytesAvailable();

    if(count<=0)
        return;

    incomingBuffer = new unsigned char[count];

    serialPort->read((char *)incomingBuffer,count);

    for(int i =0 ; i < count; i++){
        switch (protocolState ){
        case HEADER_U:
            if (incomingBuffer[i] == 'U'){
                protocolState = HEADER_N;
            }
            break;
        case HEADER_N:
            if (incomingBuffer[i] == 'N')
                protocolState = HEADER_E;
            else{
                i--;
                protocolState = HEADER_U;
            }
            break;
        case HEADER_E:
            if (incomingBuffer[i] == 'E')
                protocolState = HEADER_R;
            else{
                i--;
                protocolState = HEADER_U;
            }
            break;
        case HEADER_R:
            if (incomingBuffer[i] == 'R')
                protocolState = NBYTES;
            else{
                i--;
                protocolState = HEADER_U;
            }
            break;
        case NBYTES:
            rxData.nBytes = incomingBuffer[i];
            protocolState = TOKEN;
            break;
        case TOKEN:
            if (incomingBuffer[i] == ':'){
                protocolState = PAYLOAD;
                rxData.cheksum = 'U'^'N'^'E'^'R'^ rxData.nBytes^':';
                rxData.payLoad[0] = rxData.nBytes;
                rxData.index = 1;
            }
            else{
                i--;
                protocolState = HEADER_U;
            }
            break;
        case PAYLOAD:
            if (rxData.nBytes > 1){
                rxData.payLoad[rxData.index++] = incomingBuffer[i];
                rxData.cheksum ^= incomingBuffer[i];
            }
            rxData.nBytes--;
            if(rxData.nBytes == 0){
                protocolState = HEADER_U;
                if(rxData.cheksum == incomingBuffer[i]){
                    decode_Payload(&rxData.payLoad[0]);
                }
            }
            break;
        default:
            protocolState = HEADER_U;
            break;
        }
    }
    delete [] incomingBuffer;
}

void MainWindow::send_Data_Serial(uint8_t ID)
{
    uint8_t bufTx[SIZEBUFRX], index, chks;

    index = 0;
    chks = 0;

    bufTx[index++] = 'U';
    bufTx[index++] = 'N';
    bufTx[index++] = 'E';
    bufTx[index++] = 'R';
    bufTx[index++] = 0x00;
    bufTx[index++] = ':';

    if(serialPort->isOpen()){
        switch(ID){
        case ALIVE:
            ui->plainTextTX->appendPlainText(">> Estas vivo ???");

            bufTx[index++] = ALIVE;

            bufTx[NBYTES] = 2;
            break;
        case FIRMWARE:
            ui->plainTextTX->appendPlainText(">> Cual es tu Firmware");

            bufTx[index++] = FIRMWARE;

            bufTx[NBYTES] = 2;
            break;
        case UIMOTORS:
            bufTx[index++] = UIMOTORS;

            lSpeed = ui->plainTextLeftMotor->toPlainText().toInt();
            rSpeed = ui->plainTextRightMotor->toPlainText().toInt();

            bufTx[index++] = lSpeed;
            bufTx[index++] = rSpeed;

            bufTx[NBYTES] = 4;
            break;
        case UIDISPLAY:
            break;
        default:
            break;
        }

        for(int a = 0 ; a < index ; a++){
            chks ^= bufTx[a];
        }

        bufTx[index] = chks;

        /*Mostrar los datos enviados en formato hexadecimal*/
        QString hexString;
        for(int i = 0; i < bufTx[NBYTES] + PAYLOAD; i++) {
            hexString += QString("%1 ").arg(bufTx[i], 2, 16, QChar('0')).toUpper();
        }
        ui->plainTextTX->appendPlainText("Enviando: " + hexString.trimmed());

        /*Envio de los datos al puerto serie*/
        if(serialPort->isWritable()){
            serialPort->write(reinterpret_cast<char *>(bufTx),bufTx[NBYTES] + PAYLOAD);
        }

        writeFlag = true;
    }
}

void MainWindow::decode_Payload(uint8_t *buf)
{
    switch (buf[1]){
    case ALIVE:
        ui->plainTextRX->appendPlainText("Estoy vivo");
        break;
    /*case ASENSORS:
        w.ui8[0] = buf[2];
        w.ui8[1] = buf[3];
        ui->plainTextRX->appendPlainText("Sensor Izquierda:" + QString("%1").arg(w.ui16[0], 2, 10, QChar('0')));

        w.ui8[0] = buf[4];
        w.ui8[1] = buf[5];
        ui->plainTextRX->appendPlainText("Sensor Central:" + QString("%1").arg(w.ui16[0], 2, 10, QChar('0')));

        w.ui8[0] = buf[6];
        w.ui8[1] = buf[7];
        ui->plainTextRX->appendPlainText("Sensor Derecha:" + QString("%1").arg(w.ui16[0], 2, 10, QChar('0')));
        break;*/
    case UIMOTORS:
        ui->plainTextRX->appendPlainText("Motores cargados");
        break;
    default:
        w.ui8[0] = buf[2];
        w.ui8[1] = buf[3];
        ui->plainTextRX->appendPlainText("Sensor Izquierda:" + QString("%1").arg(w.ui16[0], 2, 10, QChar('0')));
        ui->plainTextRX->appendPlainText("COMANDO DESCONOCIDO");
        break;
    }
}


void MainWindow::on_buttonOpenConfig_clicked()
{
    Config *configWindow = new Config(this);
    configWindow->show();

    // Conectás la señal ANTES de mostrar
    connect(configWindow, &Config::config_Port, this, [=](QSerialPort *puerto){
        this->serialPort = puerto;
        connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::on_serialPort_Rx);
    });
    connect(configWindow, &Config::conection_State, this, &MainWindow::update_Indicator);

    //configWindow->exec(); // modal
    //configWindow->deleteLater();
}

void MainWindow::update_Indicator(bool conected)
{
    if (conected) {
        ui->labelSerialConected->setStyleSheet("background-color: green; border-radius: 15px;");
    } else {
        ui->labelSerialConected->setStyleSheet("background-color: grey; border-radius: 15px;");
    }
}


void MainWindow::on_button_Clean_clicked()
{
    ui->plainTextRX->clear();
    ui->plainTextTX->clear();
}


void MainWindow::on_buttonDebugSendData_clicked()
{
    uint8_t id;

    id = ui->comboBoxData->currentData().toInt();
    send_Data_Serial(id);
}

