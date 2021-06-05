#ifndef _SERIAL_INTERFACE_H_
#define _SERIAL_INTERFACE_H_

#define STATUS_INVALID      (-1)
#define STATUS_GOOD         0
#define RECEIVE_DATA_LENGTH 32
#define SEND_DATA_LENGTH    8

#include "SerialDriver.hpp"

typedef struct _ReceiveData
{
    int count;
    float timeStamp; // seconds
    // int status = STATUS_INVALID;
    float rightInput;
    float leftInput;
    float rightAngVel;
    float leftAngVel;
    float rightAng;
    float leftAng;
} ReceiveData;

typedef struct _SendData
{
    // int count;
    // float timeStamp;
    // int status;
    float rightVelSetpoint;
    float leftVelSetpoint;
} SendData;

class SerialInterface : public SerialDriver
{
private:
    SdByte stockData[ RECEIVE_DATA_LENGTH*2 ];

    int oldestDataPoint = 0;

    bool is_ready;

    bool updateStockData();

    void float_to_char( float value, SdByte *data );

    void int_to_char( int value, SdByte *data );
public:
    SerialInterface();

    ~SerialInterface();
    bool setup_serial_driver( char *aPortName );

    bool get_data( ReceiveData *data );

    bool send_data( SendData *data );

    void reset_data();
    
    bool check_is_ready();
};

#endif // _SERIAL_INTERFACE_H_