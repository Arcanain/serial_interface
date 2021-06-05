#include "../include/SerialDriver.hpp"

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>

SerialDriver::SerialDriver()
{
}

SerialDriver::~SerialDriver()
{
}

// reference URL: http://www.02.246.ne.jp/~torutk/cxx/serial/posix_serial.html,
//                https://linuxjm.osdn.jp/html/LDP_man-pages/man3/termios.3.html

SdResult SerialDriver::open_port( char *port_name )
{
    char path_to_port[SD_MAX_PORT_NAME_LEN + 6]; // +6 for "/dev/%s"

    sprintf( path_to_port, "/dev/%s", port_name );
    handle = open( path_to_port, O_RDWR | O_NOCTTY );

    if ( handle >= 0 )
    {
        /*
        struct termios {
            tcflag_t        c_iflag;        / input modes /
            tcflag_t        c_oflag;        / output modes /
            tcflag_t        c_cflag;        / control modes /
            tcflag_t        c_lflag;        / line discipline modes /
            cc_t            c_cc[NCCS];     / control chars /
        };
        */
        struct termios port_config;

        if ( !tcgetattr( handle, &port_config ) )
        {
            port_config.c_cflag = CS8 | CLOCAL | CREAD;
            
            switch( SD_SERIAL_SPEED )
            {
            case 9600:
                port_config.c_cflag |= B9600;
                break;
            case 19200:
                port_config.c_cflag |= B19200;
                break;
            case 38400:
                port_config.c_cflag |= B38400;
                break;
            case 57600:
                port_config.c_cflag |= B57600;
                break;
            default:
                port_config.c_cflag |= B115200;
                break;
			}
			port_config.c_iflag = INPCK;
			port_config.c_oflag = 0;
			port_config.c_lflag = 0;
			port_config.c_cc[VMIN] = 0;
			port_config.c_cc[VTIME] = 0;

			if ( !tcsetattr( handle, TCSANOW, &port_config ) )
			{
		    	return SD_SUCCESS;
			}
        }
    	close( handle );
    }

    return SD_ERROR;

}
     
void SerialDriver::close_port()
{
	if ( handle != SD_INVALID_HANDLE )
    {
        close( handle );
    }
    handle = SD_INVALID_HANDLE;
}

SdResult SerialDriver::write_to_port( SdByte *data, int length )
{
	if ( tcflush( handle, TCIFLUSH ) )
    {
    	return SD_ERROR;
    }
    return write( handle, data, length );
}

// reference URL: https://software.fujitsu.com/jp/manual/manualfiles/M050000/B1WN4911/01/idmgr05/idmgr378.htm,

SdResult SerialDriver::read_from_port( SdByte *data, int max_length, SdState *state )
{
	/*
	struct　timeval　{
　　　　long　tv_sec;  //[s]
　　　　long　tv_usec; //[s]
　   };
	*/
	struct timeval timeout;
	fd_set         fds;
    SdResult       total_read_length = 0;
    int            interval_timeout_ms = 20000000/SD_SERIAL_SPEED;

	timeout.tv_sec  = 1;
    timeout.tv_usec = 0;

	FD_ZERO( &fds );
    FD_SET( handle, &fds );

	int select_result = select( handle + 1, &fds, NULL, NULL, &timeout );
    if ( select_result <= 0 )
    {
        printf("\n SerialDriver.cpp select() has error %d \n", errno);
    	return SD_ERROR;
    }
    
	//TODO: write a comment, why 2000? search interval time!
    interval_timeout_ms = interval_timeout_ms < 2000 ? 2000 : interval_timeout_ms;
    
	int count = 0;
    int maxCount = 100;
    while( total_read_length < max_length )
    {
        if ( count > maxCount ) return SD_ERROR;

        timeout.tv_sec = 0;
        timeout.tv_usec = interval_timeout_ms;

        FD_ZERO( &fds );
        FD_SET( handle, &fds );

        *state = select( handle + 1, &fds, NULL, NULL, &timeout );

        if ( *state < 0 )
        {
            printf("\n SerialDriver.cpp invalid state %d \n", *state);
        	return SD_ERROR;
        }
        else if ( *state == 0 )
        {
        	return total_read_length;
        }

        int read_length;

    	read_length = read( handle, data + total_read_length, max_length-total_read_length );

    	if ( read_length < 0 )
    	{
            printf("\n SerialDriver.cpp read faled total_read_length: %d \n", total_read_length);
    		return SD_ERROR;
    	}

    	total_read_length += read_length;
        ++count;
    }

    return total_read_length;
}