#include "ntp.h"

#include <stdio.h>
#include <TimeLib.h> 

WiFiUDP Udp;

char * formatTimeDigits (int num)
{
    static char strOut[2];
    if (num >= 0 && num < 10) {
        sprintf(strOut, "%02d", num);
    } else {
        sprintf(strOut, "%1d", num);
    }
    return strOut;
}

String get_ntp_date_time(int hour, int minute, int second, int day, int month, int year)
{
    String display_time = "";
    display_time += formatTimeDigits(hour);
    display_time += ":";
    display_time += formatTimeDigits(minute);
    display_time += ":";
    display_time += formatTimeDigits(second);

    display_time += " ";
    display_time += formatTimeDigits(day);
    display_time += ".";
    display_time += formatTimeDigits(month);
    display_time += ".";
    display_time += year;

    return display_time;
}

/* query ntp server
 * 
 * credits: 
 *  https://github.com/PaulStoffregen/Time/blob/master/examples/TimeNTP_ESP8266WiFi/TimeNTP_ESP8266WiFi.ino
 */

time_t getNtpTime() {
    byte packetBuffer[NTP_PACKET_SIZE];
    IPAddress ntp_server_ip = IPAddress();
    ntp_server_ip.fromString(NTP_IP);

    Udp.begin(localPort);
    /*
     * Bug, upd sending on port 4097 
     * http://www.esp8266.com/viewtopic.php?f=29&t=2390&start=4
     * https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/WiFiUdp.cpp
     * http://esp8266.github.io/Arduino/versions/2.0.0-rc2/doc/libraries.html
     */

    while (Udp.parsePacket() > 0); // discard any previously received packets

    sendNTPpacket(ntp_server_ip);
    uint32_t beginWait = millis();
    while (millis() - beginWait < 1500) {
        int size = Udp.parsePacket();
        if (size >= NTP_PACKET_SIZE) {
            Udp.read(packetBuffer, ntp_server_ip);  // read packet into the buffer
            unsigned long secsSince1900;
            // convert four bytes starting at location 40 to a long integer
            secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
            secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
            secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
            secsSince1900 |= (unsigned long)packetBuffer[43];
            return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
        }
    }
    return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address) {
    byte packetBuffer[NTP_PACKET_SIZE];

    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12]  = 49;
    packetBuffer[13]  = 0x4E;
    packetBuffer[14]  = 49;
    packetBuffer[15]  = 52;
    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:                 
    Udp.beginPacket(address, 123); //NTP requests are to port 123
    Udp.write(packetBuffer, NTP_PACKET_SIZE);
    Udp.endPacket();
}

