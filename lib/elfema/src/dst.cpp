#include "dst.h"

#include <stdio.h>

/* daylight saving code
 *
 * credits: 
 *  http://hackaday.com/2012/07/16/automatic-daylight-savings-time-compensation-for-your-clock-projects/
 *
 * */

/*--------------------------------------------------------------------------
  FUNC: 6/11/11 - Returns day of week for any given date
  PARAMS: year, month, date
  RETURNS: day of week (0-7 is Sun-Sat)
  NOTES: Sakamoto's Algorithm
    http://en.wikipedia.org/wiki/Calculating_the_day_of_the_week#Sakamoto.27s_algorithm
    Altered to use char when possible to save microcontroller ram
--------------------------------------------------------------------------*/
char dow(int y, char m, char d) {
   static char t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
   y -= m < 3;
   return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}

/*--------------------------------------------------------------------------
  FUNC: 6/11/11 - Returns the date for Nth day of month. For instance,
    it will return the numeric date for the 2nd Sunday of April
  PARAMS: year, month, day of week, Nth occurence of that day in that month
  RETURNS: date
  NOTES: There is no error checking for invalid inputs.
--------------------------------------------------------------------------*/
char NthDate(int year, char month, char DOW, char NthWeek) {
  char targetDate = 1;
  char firstDOW = dow(year,month,targetDate);
  while (firstDOW != DOW){
    firstDOW = (firstDOW+1)%7;
    targetDate++;
  }
  //Adjust for weeks
  targetDate += (NthWeek-1)*7;
  return targetDate;
}

int last_sunday_of_month(int month, int year) {
    int last_oct_sun_day = NthDate(year, month, SUNDAY, 5);
    if ( last_oct_sun_day > 31 ) {
        last_oct_sun_day = NthDate(year, month, SUNDAY, 4);
    }
    return last_oct_sun_day;
}

int get_DST(int day, int month, int year) {
    int DST_spring_forward_date = last_sunday_of_month(MARCH, year);
    if ( month < MARCH ) {
        return DST_WINTER;
    }
    if ( month == MARCH && day < DST_spring_forward_date ) {
        return DST_WINTER;
    } else if ( month < OCTOBER ) {
        return DST_SUMMER;
    }

    int DST_fall_back_date = last_sunday_of_month(OCTOBER, year);
    if ( month == OCTOBER && day < DST_fall_back_date ) {
        return DST_SUMMER;
    } else {
        return DST_WINTER;
    }
}

bool is_spring_forward(int DST, int hour, int day, int month, int year) {
    return ( hour == 2 && \
             DST != get_DST(day, month, year) );
}

bool is_fall_back(int DST, int hour, int day, int month, int year) {
    return ( hour == 3 && \
             DST != get_DST(day, month, year) );
}

#ifndef TEST
int main(void) {
    char ok[]   = "Test success\n";
    char fail[] = "Test fail\n"; 

    /* 2015 */ 
    printf( is_spring_forward(DST_WINTER, 2, 20, MARCH, 2015) ? fail : ok   );
    printf( is_spring_forward(DST_WINTER, 1, 29, MARCH, 2015) ? fail : ok   );
    printf( is_spring_forward(DST_WINTER, 2, 29, MARCH, 2015) ? ok   : fail );
    printf( is_spring_forward(DST_SUMMER, 2, 29, MARCH, 2015) ? fail : ok   );

    printf( is_fall_back(DST_SUMMER, 3, 24, OCTOBER, 2015) ? fail : ok   );
    printf( is_fall_back(DST_SUMMER, 2, 25, OCTOBER, 2015) ? fail : ok   );
    printf( is_fall_back(DST_SUMMER, 3, 25, OCTOBER, 2015) ? ok   : fail );
    printf( is_fall_back(DST_WINTER, 3, 25, OCTOBER, 2015) ? fail : ok   );
    printf( is_fall_back(DST_WINTER, 3, 26, OCTOBER, 2015) ? fail : ok   );

    /* 2016 */
    printf( is_spring_forward(DST_WINTER, 2, 20, MARCH, 2016) ? fail : ok   );
    printf( is_spring_forward(DST_WINTER, 1, 27, MARCH, 2016) ? fail : ok   );
    printf( is_spring_forward(DST_WINTER, 2, 27, MARCH, 2016) ? ok   : fail );
    printf( is_spring_forward(DST_SUMMER, 2, 27, MARCH, 2016) ? fail : ok   );
    printf( is_spring_forward(DST_SUMMER, 2, 28, MARCH, 2016) ? fail : ok   );

    printf( is_fall_back(DST_SUMMER, 3, 29, OCTOBER, 2016) ? fail : ok   );
    printf( is_fall_back(DST_SUMMER, 2, 30, OCTOBER, 2016) ? fail : ok   );
    printf( is_fall_back(DST_SUMMER, 3, 30, OCTOBER, 2016) ? ok   : fail );
    printf( is_fall_back(DST_WINTER, 3, 30, OCTOBER, 2016) ? fail : ok   );
    printf( is_fall_back(DST_WINTER, 3, 31, OCTOBER, 2016) ? fail : ok   );
    
}
#endif
