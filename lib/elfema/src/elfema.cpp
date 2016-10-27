#include "elfema.h"
#include "ntp.h"

#include <TimeLib.h> 

ESP8266WebServer server(webserver_port);

/* clock stuff and display */
int state = modeA;
bool elfema_is_set = false;
int prevMinute = undefined; 
int display_hour = undefined;
int display_minute = undefined;
int DST = undefined;

void step_clock() {
    if (state == modeA) {
        state = modeB;
        Serial.println("Clock Mode A");
        digitalWrite(IO_1, LOW);
        digitalWrite(IO_4, HIGH);
    } else {
        state = modeA;
        Serial.println("Clock Mode B");
        digitalWrite(IO_3, LOW);
        digitalWrite(IO_2, HIGH);
    }
    delay(trigger_hold);
    no_switch_state();
}

void no_switch_state() {
    digitalWrite(IO_1, HIGH);
    digitalWrite(IO_4, LOW);

    digitalWrite(IO_3, HIGH);
    digitalWrite(IO_2, LOW); 
}

void set_clock_stop() {
    elfema_is_set = false;
    prevMinute = -1;

    display_hour = -1;
    display_minute = -1;
}

int DST_hour() {
    return (hour() + DST) % 24;
}

/*----- webserver code -------*/

void set_display_time() {
    int read_hour = -1;
    int read_minute = -1;

    if ( server.args() != 2 )
    {
        server.send(404, "text/plain", "wrong args\n");
        return;
    }

    for (uint8_t i=0; i<server.args(); i++)
    {
        String arg_name = server.argName(i);
        String arg_val = server.arg(i);

        if ( arg_name == "H" ) {
            read_hour = arg_val.toInt();
        } else if ( arg_name == "M" ) {
            read_minute = arg_val.toInt();
        } else {
            server.send(404, "text/plain", "wrong args\n");
            return; 
        }
    }

    if (( -1 < read_hour ) && ( read_hour < 24) && \
        ( -1 < read_minute ) && (read_minute < 60 ))
    {
        Serial.print("Current Display time is : ");
        Serial.print(read_hour); Serial.print(":"); Serial.print(read_minute);
        Serial.print("\n");

        if (( -1 != display_hour ) && \
            ( -1 != display_minute))
        {
            elfema_is_set = false;
        }
        display_hour = read_hour;
        display_minute = read_minute; 
        server.send(200, "text/plain", "ok got it!\n");
    } else {
        server.send(404, "text/plain", "wrong args\n");
    }
}

void set_clock_stop_command() {
    set_clock_stop();
    server.send(200, "text/plain", "ok clock is now stopped!\n");
}

void get_time_command() {
    String time_str = get_ntp_date_time(DST_hour(), minute(), second(), day(), month(), year());
    server.send(200, "text/plain", "ntp time and date: " + time_str + "\n");
}

bool manual_ntpquery() {
    time_t query_result = getNtpTime();
    if ( query_result != 0 ) {
        setTime(query_result);
        return true;
    } else {
        return false;
    }
}

void query_ntp_command() {
    String time_str;
    if ( manual_ntpquery() == true ) {
        time_str = get_ntp_date_time(DST_hour(), minute(), second(), day(), month(), year());
        server.send(200, "text/plain", "ntp query success\nntp time and date: " + time_str + "\n");
    } else {
        server.send(500, "text/plain", "ntp query failed\n");
    }
}

void handleNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET)?"GET":"POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i=0; i<server.args(); i++){
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

