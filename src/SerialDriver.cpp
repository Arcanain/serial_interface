#include "../include/SerialDriver.hpp"

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

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
            port_config.c_cflag = = CS8 | CLOCAL | CREAD;
            
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

}

SdResult SerialDriver::write_to_port( SdByte *data, int length )
{

}

SdResult SerialDriver::read_from_port( SdByte *data, int max_length, SdState *state )
{

}