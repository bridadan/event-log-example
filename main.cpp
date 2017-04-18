#include "mbed.h"
#include <stdio.h>
#include "SDBlockDevice.h"
#include "FATFileSystem.h"

SDBlockDevice sdBlockDevice(PTE3, PTE1, PTE2, PTE4);
FATFileSystem fs("fs");
InterruptIn button(SW2);

volatile bool clicked = false;

void button_clicked() {
    clicked = true;
}

int main()
{
    int error;
    printf("Mounting the filesystem\r\n");
    error = fs.mount(&sdBlockDevice);
    
    if (error) {
        printf("Filesystem mounting failed, reformatting the SD card\r\n");
        error = FATFileSystem::format(&sdBlockDevice);
        
        if (error) {
            printf("Failed to format the SD card. Exiting.\r\n");
            mbed_die();
        } else {
            printf("SD card formatted, retrying mount\r\n");
            error = fs.mount(&sdBlockDevice);
            
            if (error) {
                printf("Filesystem mounting failed after SD card format. Exiting.\r\n");
                mbed_die();
            }
        }
    }
    
    printf("Filesystem mounted\r\n");
    
    button.rise(&button_clicked);
    
    printf("waiting for interrupt\r\n");
    unsigned int counter = 0;
    char buff[30];
    while (true) {
        if (clicked == true) {
            clicked = false;
            printf("motion detected! %d\r\n", counter);
            time_t seconds = time(NULL);
            int stringLength = snprintf(buff, 30, "%u, %u\r\n", seconds, counter);
            FILE* fd = fopen("/fs/events.txt", "a");
            
            if (fd) {
                int writtenBytes = fwrite(buff, 1, stringLength, fd);
                
                if (writtenBytes != stringLength) {
                    printf("Expected to write %d bytes, but actually wrote %d bytes\r\n", stringLength, writtenBytes);
                }
                
                if (fclose(fd) < 0) {
                    printf("Failed to close '/fs/events.txt'\r\n");
                }
            } else {
                printf("Failed to open '/fs/events.txt'\r\n");
            }
            
            counter++;        
        }
   }
}