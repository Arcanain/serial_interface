#ifndef _SERIAL_INTERFACE_H_
#define _SERIAL_INTERFACE_H_

#define STATUS_INVALID      (-1)
#define STATUS_GOOD         0
#define RECEIVE_DATA_LENGTH 32
#define SEND_DATA_LENGTH    8
#define MAX_FAILED_COUNT    50

#include "SerialDriver.hpp"

typedef struct _ReceiveData
{
    int count;
    float time_stamp; // seconds
    // int status = STATUS_INVALID;
    float right_input;
    float left_input;
    float right_ang_vel;
    float left_ang_vel;
    float right_ang;
    float left_ang;
} ReceiveData;

typedef struct _SendData
{
    // int count;
    // float timeStamp;
    // int status;
    float right_vel_setpoint;
    float left_vel_setpoint;
} SendData;

class SerialInterface : public SerialDriver
{
private:
    SdByte stock_data[RECEIVE_DATA_LENGTH * 2];

    int oldest_data_point = 0;

    int read_failed_count;

    bool is_ready = false;

    bool update_stock_data();

    void float_to_char(float value, SdByte *data);

    void int_to_char(int value, SdByte *data);

public:
    SerialInterface();

    ~SerialInterface();

    void setup_serial_driver(char *port_name);

    void get_data(ReceiveData *data);

    bool send_data(SendData *data);

    void reset_data();

    bool check_is_ready();
};

#endif // _SERIAL_INTERFACE_H_