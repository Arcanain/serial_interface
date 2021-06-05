
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

bool SerialInterface::setup_serial_driver( char *port_name )
{
    reset_data();
    close_port();
    if ( open_port( port_name ) == SD_SUCCESS ) {
        SdByte testData[2];
        SdState portState;
        SdResult result = read_from_port( testData, sizeof( testData ), &portState );
        return ( result != -1 );
    }

    return false;
}

