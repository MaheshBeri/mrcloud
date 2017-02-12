#include <stdio.h>
#include <stdlib.h>

#include <wiringPi.h>
#define MOTOR1A 4
#define MOTOR1B 5
#define MOTOR1E 6
#define MOTOR1E2 31

void initmotor(){
  pinMode (MOTOR1A, OUTPUT) ;
  pinMode (MOTOR1B, OUTPUT) ;
  pinMode (MOTOR1E, OUTPUT) ;
  pinMode (MOTOR1E2, OUTPUT) ;
}
void startmotor(int switch_option){
	if(switch_option==1){
		digitalWrite (MOTOR1A, HIGH) ; 
		digitalWrite (MOTOR1B, LOW) ; 
		digitalWrite (MOTOR1E, HIGH) ; 
		digitalWrite (MOTOR1E2, HIGH) ; 
	}	else{
		digitalWrite (MOTOR1A, HIGH) ; 
		digitalWrite (MOTOR1B, HIGH) ; 
		digitalWrite (MOTOR1E, LOW) ; 
		digitalWrite (MOTOR1E2, LOW) ; 
	}
}

/*int main(int argc, char** argv) {
 printf( "Raspberry Pi wiringPi test program\n" );

        if ( wiringPiSetup() == -1 )
                exit( 1 );
 printf( "Raspberry Pi wiringPi test starting\n" );
	initmotor();
		printf("Starting  Motor\n");
	startmotor(1);
    sleep (2);
		printf("Stopping  Motor\n");
	startmotor(0);
	return 0;
}*/
