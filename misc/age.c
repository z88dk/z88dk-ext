

// Compute someone's age in days (add one for your exact "age")
// If you have a RTC you can add -DCLOCK to insert only your birth date


// zcc +zx -lndos -lm -create-app biorythms.c
// zcc +cpm -subtype=px8 -lm -create-app  -DCLOCK biorythms.c


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>


	struct tm t1;
	struct tm t2;

    static char buf[0x200];


int ReadDate(int *day,int *month,int *year)
{
    *buf=0;

    fgets(buf,sizeof(buf),stdin);

    return sscanf(buf,"%d/%d/%d",day,month,year)!=3;
}

int LeapYear(int year)
{
    return !(year%400)||(!(year%4)&&(year%100));
}

int ValiDate(int day,int month,int year)
{
    static int days[]={31,0,31,30,31,30,31,31,30,31,30,31};//static array
    days[1]=LeapYear(year)?29:28;
    return !(month>0&&month<=12&&day>0&&day<=days[month-1]);//return valid
}

int delta(int d1,int m1,int y1,int d2,int m2,int y2)
{
    if(ValiDate(d1,m1,y1)||ValiDate(d2,m2,y2))
    {
        return -1;
    }
	
    t1.tm_sec=0;
    t1.tm_min=0;
    t1.tm_hour=0;
    t1.tm_mday=d1;
    t1.tm_mon=m1;
    t1.tm_year=y1;
    
    t2.tm_sec=0;
    t2.tm_min=0;
    t2.tm_hour=0;
    t2.tm_mday=d2;
    t2.tm_mon=m2;
    t2.tm_year=y2;
	
	return (int)((mktime(&t2)-mktime(&t1))/86400UL);   // 86400 = 24 hours
}

double calc(int days,int val)
{
    return sin(2*(days%val)*M_PI/val);
}


int main(void)
{
    int day,month,year;
    int tday,tmonth,tyear;
	
	fputc_cons(12);

#ifndef CLOCK
    printf("Please enter a valid date for today(dd/mm/yyyy): ");

    while(ReadDate(&tday,&tmonth,&tyear)||ValiDate(tday,tmonth,tyear))
    {
        printf("Invalid date!\nTry again: ");
    }
#endif

    printf("Please enter your birth date(dd/mm/yyyy): ");

    while(ReadDate(&day,&month,&year)||ValiDate(day,month,year))
    {
        printf("Invalid date!\nTry again: ");
    }


#ifdef CLOCK
    time_t sec;
    struct tm *sys;
    time(&sec);
    sys=localtime(&sec);

    int d=delta(day,month,year,sys->tm_mday,sys->tm_mon+1,sys->tm_year-1900);
#else
	int d=delta(day,month,year-1900,tday,tmonth,tyear-1900);
#endif

    printf("%u days have passed",d);
	fgets(buf,sizeof(buf),stdin);

    return 0;
}
