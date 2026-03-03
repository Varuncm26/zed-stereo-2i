#include "uart.h"

int InitUart(speed_t baud){
        //int serial_port=open("/dev/serial0",O_RDWR);
        int serial_port = open("/dev/ttyTHS1",O_RDWR);
        if (serial_port < 0) {
                printf("Error %i from open: %s\n", errno,strerror(errno));
                return -1;
        }
        struct termios tty;
        if(tcgetattr(serial_port, &tty) !=0) {
                printf("Error %i from tcgetattr: %s\n", errno,strerror(errno));
                return -1;
        }//tcgetattr() gets the parameters associated with the object referred by fd
        tty.c_cflag &= ~CSTOPB; //Clear the stop bit, only one stop bit is used in communication
        tty.c_cflag &= ~CSIZE; //Clear all bits that set the data size
        tty.c_cflag |=CS8; //8 bits per byte
        tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
        tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
        tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)

        //Local Modes (c_Iflag)
        tty.c_lflag &= ~ICANON; //Disable canonical mode
        tty.c_lflag &= ~ECHO; // Disable echo
        tty.c_lflag &= ~ECHOE; // Disable erasure
        tty.c_lflag &= ~ECHONL; // Disable new-line echo
        tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP

        //Input flags
        tty.c_iflag &= ~(IXON | IXOFF | IXANY); //Turn off s/w flow control
        tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special hand$
        //Output Modes c_oflag
        tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
        //tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

        cfsetispeed(&tty,baud);
        cfsetospeed(&tty,baud);

        //Blocking Read
        //
	tty.c_cc[VMIN] = 1; //wait till atleast 1 byte is available
        tty.c_cc[VTIME] = 0; //disable tmer after first byte recieved

        //Save tty settings , also checking for error
        if(tcsetattr(serial_port,TCSANOW,&tty) !=0) {
                printf("Error %i from tcsetattr: %s\n",errno,strerror(errno));
                return -1;
        }
        //return 0; //Success
        printf("UART SERIAL_FD %d\n",serial_port);
        return serial_port;
}

