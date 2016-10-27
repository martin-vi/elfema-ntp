#include <elfema.h>
#include <ntp.h>
#include <dst.h>
 
#include <TimeLib.h> 
#include <ESP8266WiFi.h>

void setup() 
{
  pinMode(IO_1, OUTPUT);
  pinMode(IO_2, OUTPUT);
  pinMode(IO_3, OUTPUT);
  pinMode(IO_4, OUTPUT);
  no_switch_state();
  
  Serial.begin(9600);
  while (!Serial) ; 
  delay(250);
  Serial.println("TimeNTP Example");
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSW);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // init clock with two steps
  step_clock();
  step_clock();

  Serial.print("IP number assigned by DHCP is ");
  Serial.println(WiFi.localIP());
  
  server.on("/", [](){
    server.send(200, "text/plain", "hello world, efema wall clock here!\n");
  });
  server.on("/set", set_display_time);
  server.on("/stop", set_clock_stop_command);
  server.on("/time", get_time_command);
  server.on("/query_ntp", query_ntp_command);

  server.on("/step", [](){
    step_clock();
    server.send(200, "text/plain", "done\n");
  });

  server.onNotFound(handleNotFound);
  server.begin();

  // get inital time via ntp
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
  setSyncInterval(6*60*60); // every 6 hours
}

/*------- Clock code ---------*/
void set_clock_display() {
  if ( elfema_is_set ) { return; }
  
  if (( (DST_hour() % 12) == (display_hour % 12) ) &&  \
      ( minute() == display_minute )) {
    elfema_is_set = true;
    prevMinute = display_minute;
    return;
  }
  
  // increment clock
  display_minute = ( display_minute + 1 ) % 60;
  if ( display_minute == 0 ) {
    display_hour = ( display_hour + 1 ) % 24;
  }
  
  step_clock();
  delay(clock_set_sleep);
}


void loop()
{  
    server.handleClient();
    if (timeStatus() != timeNotSet) {
        if ( DST  == undefined ) {
            DST = get_DST(day(), month(), year());
        }

        /* +1 minute */
        if ( prevMinute != minute() && elfema_is_set ) {
            prevMinute = minute();
            step_clock();
        }

        /* set clock */
        if ( (!elfema_is_set)  && (display_hour != undefined) && (display_minute != undefined) ) {
            set_clock_display();
        }

        if ( minute() == 0 && second() == 0 )
        {
            if ( DST_hour() == 1 )
            {
                manual_ntpquery();
            }
            /* sprint forward, 2 to 3 */
            else if ( is_spring_forward(DST, DST_hour(), day(), month(), year()) )
            {
                DST = DST_SUMMER;
                elfema_is_set = false;
            }
            /* fall back, 3 to 2 */
            else if ( is_fall_back(DST, DST_hour(), day(), month(), year()) )
            {
                DST = DST_WINTER;
                /* wait 1h, clock is inactive */
                while ( DST_hour() < 3 ) { server.handleClient(); }
            }

        }
    }
}
