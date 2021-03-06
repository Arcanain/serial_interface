#include "../include/SerialInterface.hpp"

#include <stdio.h>

SerialInterface::SerialInterface()
{
    printf("init SerialInterface");
}

SerialInterface::~SerialInterface()
{
    printf("deinit SerialInterface");
}

bool SerialInterface::update_stock_data()
{
    SdState port_state = 1;

    while ( port_state )
    {
        SdByte temp_read_data[ RECEIVE_DATA_LENGTH * 2 + 2 ]; //TODO: check data size
        SdResult result = read_from_port( temp_read_data, sizeof(temp_read_data), &port_state );
        if ( result == SD_ERROR ) return false;

        for ( int i = 0 ; i < result ; ++i ) {
            stock_data[oldest_data_point] = temp_read_data[i];
            oldest_data_point += 1;
            if ( oldest_data_point >= sizeof( stock_data ) ) oldest_data_point = 0;
        }
    }

    return true;
}

void SerialInterface::float_to_char( float value, SdByte *data )
{
    unsigned char* value_ptr = (unsigned char*)&value;

    for(int i = 0; i < sizeof(float); i++) {
        data[i] = value_ptr[i];
    }
}

void SerialInterface::int_to_char( int value, SdByte *data )
{
    unsigned char* value_ptr = (unsigned char*)&value;

    for(int i = 0; i < sizeof(int); i++) {
        data[i] = value_ptr[i];
    }
}

void SerialInterface::setup_serial_driver( char *port_name )
{
    read_failed_count = 0;
    reset_data();
    close_port();

    if ( open_port( port_name ) == SD_SUCCESS ) {
        SdByte testData[2];
        SdState port_state;
        SdResult result = read_from_port( testData, sizeof( testData ), &port_state );
        is_ready = ( result != -1 );
        return;
    }

    is_ready = false;
}

void SerialInterface::get_data( ReceiveData *data )
{
    if ( !update_stock_data() ) {
        read_failed_count++;
        if (read_failed_count == MAX_FAILED_COUNT) is_ready = false;
        return;
    } else {
        read_failed_count = 0;
    }

    bool is_in_data = false;
    SdByte unit_data[RECEIVE_DATA_LENGTH];
    int unit_data_point = 0;

    int p = 0;
    for ( int i = 0; i < sizeof( stock_data ); ++i ) {
        p = oldest_data_point + i;

        if ( p >= sizeof( stock_data ) ) p %= ( sizeof( stock_data ) );
        if ( is_in_data ) {
            if (unit_data_point >= RECEIVE_DATA_LENGTH) {
                break;
            }
            unit_data[unit_data_point] = stock_data[p];
            unit_data_point ++;
        }
        if ( stock_data[p] == 0x0a ) is_in_data = !is_in_data;
    }

    //TODO: recode for full data
    p = 0;

    data->count = *(int*)&unit_data[p];
    p += 4;

    data->time_stamp = *(float*)&unit_data[p];
    p += sizeof(float);

    data->right_input = *(float*)&unit_data[p];
    p += sizeof(float);

    data->left_input = *(float*)&unit_data[p];
    p += sizeof(float);

    data->right_ang_vel = *(float*)&unit_data[p];
    p += sizeof(float);

    data->left_ang_vel = *(float*)&unit_data[p];
    p += sizeof(float);

    data->right_ang = *(float*)&unit_data[p];
    p += sizeof(float);

    data->left_ang = *(float*)&unit_data[p];
    p += sizeof(float);
}

bool SerialInterface::send_data( SendData *data )
{
    SdResult result = SD_ERROR;
    SdByte byte_data[SEND_DATA_LENGTH + 1];

    int p = 0;

    byte_data[p] = 0x0a; // set header
    p++;

    //TODO: recode for full data
    float_to_char( data->right_vel_setpoint, &byte_data[p] );
    p += sizeof(float);

    float_to_char( data->left_vel_setpoint, &byte_data[p] );
    p += sizeof(float);

    result = write_to_port( byte_data, sizeof(byte_data) );

    return (result != SD_ERROR);
}

void SerialInterface::reset_data()
{
    for ( int i = 0; i < sizeof( stock_data ); i++ ) {
        stock_data[i] = 0;
    }
    oldest_data_point = 0;
}

bool SerialInterface::check_is_ready()
{
    return is_ready;
}