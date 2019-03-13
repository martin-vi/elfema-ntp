#include <ESP8266WebServer.h>
#include <WiFiUdp.h>

const int NTP_PACKET_SIZE = 48;         // NTP time is in the first 48 bytes of message
static unsigned int localPort = 8888;   // local port to listen for UDP packets
static const int timeZone = 1;          // Central European Time

extern WiFiUDP Udp;

char * formatTimeDigits(int num);
String get_ntp_date_time(int hour, int minute, int second, int day, int month, int year);
time_t getNtpTime(char *ntp_server_ip);
void sendNTPpacket(IPAddress &address);
