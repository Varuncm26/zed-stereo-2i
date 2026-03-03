#include <stdio.h>
#include <pthread.h>
#include <termios.h>
#include "uart.h"

int serial_fd1;
pthread_mutex_t uart_mutex = PTHREAD_MUTEX_INITIALIZER;

// Function to put terminal in raw mode so getchar() is instant
void set_raw_mode(struct termios *orig) {
    struct termios raw;
    tcgetattr(STDIN_FILENO, orig);
    raw = *orig;
    raw.c_lflag &= ~(ICANON | ECHO); // Disable buffering and local echo
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

void reset_mode(struct termios *orig) {
    tcsetattr(STDIN_FILENO, TCSANOW, orig);
}

int CreatePacket(char key, unsigned char *buffer) {
    buffer[0] = 0xAA;      // Start byte
    buffer[1] = (unsigned char)key; // Key pressed
    buffer[2] = 0x55;      // End byte
    return 3; 
}

void *send_thread(void *arg) {
    unsigned char txBuffer[10];
    int len;
    char key;

    printf("UART Active. Press any key to send packet... (Ctrl+C to exit)\n\r");

    while (1) {
        key = getchar(); // Now returns immediately due to raw mode

        pthread_mutex_lock(&uart_mutex);
        len = CreatePacket(key, txBuffer);
        int write_status = write(serial_fd1, txBuffer, len);

        if (write_status > 0) {
            // Use \r for clean printing in raw mode
            printf("Sent Packet: [0xAA] [%c] [0x55]\n\r", key);
        } else {
            perror("UART write failed");
        }
        pthread_mutex_unlock(&uart_mutex);
    }
    return NULL;
}

int main() {
    struct termios orig_termios;
    pthread_t tx_thread;

    // 1. Use your existing InitUart function
    serial_fd1 = InitUart(B115200); 
    if (serial_fd1 < 0) {
        return -1;
    }

    // 2. Set Jetson terminal to raw mode for instant keypresses
    set_raw_mode(&orig_termios);

    // 3. Start the thread
    pthread_create(&tx_thread, NULL, send_thread, NULL);
    pthread_join(tx_thread, NULL);

    // Cleanup
    reset_mode(&orig_termios);
    close(serial_fd1);
    return 0;
}
