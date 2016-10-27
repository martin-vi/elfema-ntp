#include <ESP8266WebServer.h>

/* clock stuff */
static int IO_1 = 2;
static int IO_2 = 13;
static int IO_3 = 12;
static int IO_4 = 14;
static int modeA = 0;
static int modeB = 1;
static int undefined = -1;
static int trigger_hold = 300;

/* clock display */
static int clock_set_sleep = 500;

/* global variables */
extern int state;
extern bool elfema_is_set;
extern int prevMinute; 
extern int display_hour;
extern int display_minute;
extern int DST;

/* functions */
void step_clock();
bool manual_ntpquery();
void no_switch_state();
void set_clock_stop();
int DST_hour();

/* webserver */
static int webserver_port = 80;
extern ESP8266WebServer server;

void set_display_time();
void set_clock_stop_command();
void get_time_command();
void query_ntp_command();
void set_clock_stop();
void handleNotFound();

