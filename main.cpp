#include "mbed.h"
#include <stdio.h>
#include "SDBlockDevice.h"
#include "FATFileSystem.h"
#include "EventLog/EventLog.h"
#include "EventLog/EventLogEvent.h"
#include "ntp-client/NTPClient.h"
#include "ESP8266Interface.h"

SDBlockDevice sdBlockDevice(PTE3, PTE1, PTE2, PTE4);
FATFileSystem fs("fs");
InterruptIn log_button(SW2);
InterruptIn ntp_button(SW3);
ESP8266Interface wifi(D1, D0);

volatile bool log_event = false;
volatile bool ntp_event = false;
bool timeValid = false;

void log_button_clicked() {
    log_event = true;
}

void ntp_button_clicked() {
    ntp_event = true;
}

int main()
{
    NTPClient ntp(&wifi);
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
    EventLog eventLog("/fs/events");
    
    log_button.fall(&log_button_clicked);
    ntp_button.fall(&ntp_button_clicked);
    
    printf("waiting for interrupt\r\n");
    unsigned int counter = 0;
    char buff[30];
    while (true) {
        if (log_event) {
            log_event = false;
            printf("motion detected! %d\r\n", counter);
            EventLogEvent event((int)counter);
            printf("Created event\r\n");
            event.setTimeValid(timeValid);
            eventLog.log(&event);
            printf("Logged Event\r\n");
            counter++;
            printf("Finished logging motion\r\n");
        }
        
        if (ntp_event) {
            ntp_event = false;
            printf("Attempting to set time via NTP\r\n");
            int result = wifi.connect("AndroidAP", "mbedchina2016", NSAPI_SECURITY_WPA_WPA2);
            if (result == 0) {
                printf("Connected to wifi\r\n");
            } else {
                printf("Failed to connect to wifi with error %d\r\n", result);
                continue;
            }
            
            time_t timestamp;
            
            for (int i = 0; i < 3; i++) {
                timestamp = ntp.get_timestamp();
                if (timestamp < 0) {
                    printf("An error occurred when getting the time. Code: %ld. Retrying\r\n", timestamp);
                } else {
                    printf("Got time\r\n");
                    set_time(timestamp);
                    timeValid = true;
                    break;
                }
            }
            
            wifi.disconnect();
            
            time(&timestamp);
            printf("Current time is %s\r\n", ctime(&timestamp));
        }
   }
}