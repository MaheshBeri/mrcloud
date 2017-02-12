#include <stdio.h>
#include <stdlib.h>

#include <wiringPi.h>
#include <lcd.h>

int main(int argc, char** argv) {
 printf( "Raspberry Pi LCD test program\n" );

        if ( wiringPiSetup() == -1 )
                exit( 1 );

	int lcdhandle;
	lcdhandle=lcdInit(2,16,4,11,10,3,2,0,7,0,0,0,0);
	lcdPuts(lcdhandle,"Hello World !");
	sleep(3);
	lcdClear(lcdhandle);
	return 0;
}
