#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <poll.h>
#include "uart.h"

// Helper to set Jetson terminal to raw mode so we catch keys instantly
void set_conio_terminal_mode(struct termios *orig) {
    struct termios new_termios;
    tcgetattr(0, orig);
    new_termios = *orig;
    cfmakeraw(&new_termios);
    tcsetattr(0, TCSANOW, &new_termios);
}

void reset_terminal_mode(struct termios *orig) {
    tcsetattr(0, TCSANOW, orig);
}

int main() {
    struct termios orig_termios;
    int uart_fd = InitUart(B115200);
    
    if (uart_fd < 0) return 1;

    printf("Starting Chat. Type locally to send. Press Ctrl+C to quit.\n\r");

    // Set local keyboard to raw mode
    set_conio_terminal_mode(&orig_termios);

    struct pollfd fds[2];
    fds[0].fd = 0;          // Standard Input (Keyboard)
    fds[0].events = POLLIN;
    fds[1].fd = uart_fd;    // UART Port
    fds[1].events = POLLIN;

    char buf[256];
    while (1) {
        // Wait for either the keyboard or the UART to have data
        if (poll(fds, 2, -1) > 0) {
            
            // 1. If Keyboard has data -> Write to UART
            if (fds[0].revents & POLLIN) {
                int n = read(0, buf, sizeof(buf));
                if (n > 0) {
                    // Check for Ctrl+C (ASCII 3) to exit raw mode gracefully
                    if (buf[0] == 3) break; 
                    write(uart_fd, buf, n);
                }
            }

            // 2. If UART has data -> Write to Screen
            if (fds[1].revents & POLLIN) {
                int n = read(uart_fd, buf, sizeof(buf));
                if (n > 0) {
                    write(1, buf, n); // Write directly to stdout
                }
            }
        }
    }

    reset_terminal_mode(&orig_termios);
    close(uart_fd);
    return 0;
}
