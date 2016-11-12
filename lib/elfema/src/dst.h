const int SUNDAY=0;
const int MARCH=3;
const int OCTOBER=10;

const int DST_WINTER=0;
const int DST_SUMMER=1;

const int DST_fall_back_hour=3;
const int DST_spring_forward_hour=2;

char dow(int y, char m, char d);
char NthDate(int year, char month, char DOW, char NthWeek);

void set_DST(int day, int month);
int last_sunday_of_month(int month, int year);

int get_DST(int day, int month, int year);
bool is_spring_forward(int DST, int hour, int day, int month, int year);
bool is_fall_back(int DST, int hour, int day, int month, int year);
