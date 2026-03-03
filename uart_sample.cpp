#include <stdio.h>
#include <unistd.h>
#include "uart.h"

int main() {
    // 1. Initialize the UART port at 115200 baud
    int fd = InitUart(B115200);
    
    if (fd < 0) {
        printf("Failed to initialize UART.\n");
        return 1;
    }

    // 2. Send initial data
    const char *msg = "Hello from Jetson!\n";
    int bytes_sent = write(fd, msg[0], 1);
    
    if (bytes_sent < 0) {
        perror("Failed to write to UART");
    } else {
        printf("Sent %d bytes to computer.\n", bytes_sent);
    }

    // 3. Listen to what the computer says (infinite loop)
    printf("Listening for incoming data... (Press Ctrl+C to stop)\n");
    
    char read_buf[256];
    while (1) {
        // This will block until at least 1 byte arrives
        int n = read(fd, &read_buf, sizeof(read_buf) - 1);
        
        if (n > 0) {
            read_buf[n] = '\0'; // Null-terminate the string
            printf("Received: %s", read_buf);
            fflush(stdout); // Ensure it prints immediately
        } else if (n < 0) {
            perror("Error reading");
            break;
        }
    }

    close(fd);
    return 0;
}
