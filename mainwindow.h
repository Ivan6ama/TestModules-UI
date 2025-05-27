#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QComboBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define     SIZEBUFRX   256
#define     SIZEBUFTX   256

typedef enum{
    ALIVE           = 0xF0,
    FIRMWARE 		= 0xF1,
    UIMOTORS        = 0X01,
    UIDISPLAY       = 0x02,
}_eCMD;

typedef enum{
    HEADER_U,
    HEADER_N,
    HEADER_E,
    HEADER_R,
    NBYTES,
    TOKEN,
    PAYLOAD
}_eProtocol;

typedef union{
    uint8_t     ui8[4];
    int8_t      i8[4];
    uint16_t    ui16[2];
    int16_t     i16[2];
    uint32_t    ui32;
    int32_t     i32;
}_uWork;

typedef struct{
    uint8_t timeOut;
    uint8_t cheksum;
    uint8_t payLoad[256];
    uint8_t nBytes;
    uint8_t index;
}_sData;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_serialPort_Rx();
    void send_Data_Serial(uint8_t ID);
    void decode_Payload(uint8_t *buf);


    void update_Indicator(bool conected);

    void on_buttonOpenConfig_clicked();
    void on_button_Clean_clicked();
    void on_buttonDebugSendData_clicked();

private:
    Ui::MainWindow *ui;

    /* General purpose variables ----------------------------------------------------------------------*/
    _uWork                  w;


    /* Serial communication variables -----------------------------------------------------------------*/
    QSerialPort             *serialPort = nullptr;
    _sData                  rxData;
    _eCMD                   ID;
    _eProtocol              protocolState;
    uint8_t                 writeFlag;
    uint8_t                 serialCommand;
    int8_t                  lSpeed;
    int8_t                  rSpeed;
};
#endif // MAINWINDOW_H
