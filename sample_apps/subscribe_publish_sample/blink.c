#include <stdio.h>
#include <stdlib.h>

#include <wiringPi.h>
#define LEDPIN 1

void blink(int alert){
	if(alert==1){
		digitalWrite (LEDPIN, HIGH) ; delay (200) ;
    		digitalWrite (LEDPIN,  LOW) ;
	}else{
    		digitalWrite (LEDPIN,  LOW) ; 
	}
}
/*
int main(int argc, char** argv) {
 printf( "Raspberry Pi wiringPi test program\n" );

        if ( wiringPiSetup() == -1 )
                exit( 1 );

 printf( "Raspberry Pi wiringPi test starting\n" );
		printf("Before init  blink \n");
  pinMode (LEDPIN, OUTPUT) ;
		printf("starting \n");
  for (;;)
  {
    blink (1);
  }
	
	return 0;
}*/
