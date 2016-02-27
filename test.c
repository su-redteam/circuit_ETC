/* program test.c
   Author: STeve Wilson


*/

#include <wiringPi.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#define floor1switch 0
#define floor2switch 1
#define floor3switch 2
#define floor4switch 3
#define floor5switch 4
#define floor6switch 5

int main(void){

// setup gpio pins
// input pins for switches
wiringPiSetup();
pinMode(0,INPUT);
pinMode(1,INPUT);
pinMode(2,INPUT);
pinMode(3,INPUT);
pinMode(4,INPUT);
pinMode(5,INPUT);

// output pins for the relay engage
pinMode(7,OUTPUT);
pinMode(8,OUTPUT);
pinMode(9,OUTPUT);
pinMode(10,OUTPUT);
pinMode(11,OUTPUT);
pinMode(12,OUTPUT);
pinMode(13,OUTPUT);

// input pins for the power monitoring
pinMode(14,INPUT);



//variables

//previous states of each button:  the switches that we're monitoring are momentary,
//                                 we need to know the previous state so that when we 
//                                see that the buttons is pressed, we dont continually 
//			 	  switch states while it is held down.  the floorstatus changes
//			          states only when the prevButton variable for a given floor and 
//				  the DigitalRead monitoring the button for the same floor are different

bool floor1status,prevbutton1 = 1; 
bool floor2status,prevbutton2 = 1;
bool floor3status,prevbutton3 = 1;
bool floor4status,prevbutton4 = 1;
bool floor5status,prevbutton5 = 1;
bool floor6status,prevbutton6 = 1;
bool masterstatus,prevmaster = 0;

//floor error flags indicate that the pin monitoring the floor and the pin controlling the floor are in conflict.
// This occurs in two scenarios: 1, The control signal has just changed and the relay controling the power to each floor 
// has not switched yet.  (there is a delay time for each relay)
// the second scenario is that there is a fault; The floor should be on,  but for some reason it is not receiving power. 

bool floor1errorflag = 0;

// the floordelaycount variables allow for a delay count for each floor and for the master control,  this is because the 
// we need to account for the delay of the relays.  
// example:  if Floor1status = 1 and DigitalRead(0) = 0  the delay count will begin counting.  once the counter reaches
// a set value that is long enough to account for any delay by the relay, it will set the value of floor1errorflag to 1.
int  floor1delaycount = 0;



while (1) {

if (digitalRead(0) == HIGH){
//check floor 1
	if ((floor1status == 1) && (prevbutton1 != 1)){
		
		floor1status = 0;
		prevbutton1 = 1; 
		floor1delaycount = 2000;
		}
        else if ((floor1status == 0) && (prevbutton1 != 1)){
		
		floor1status = 1;
		prevbutton1 = 1; 
		floor1delaycount = 2000;
		}
}

if ((digitalRead(floor1switch) == LOW) && (prevbutton1 == 1))
{
prevbutton1 = 0;
}

//--------------------------------------------------------------------


//check floor 2
if (digitalRead(1) == HIGH){
	if ((floor2status == 1) && (prevbutton2 != 1)){
		
		floor2status = 0;
		prevbutton2 = 1; 
		
		}
        else if ((floor2status == 0) && (prevbutton2 != 1)){
		
		floor2status = 1;
		prevbutton2 = 1; 
		
		}
	
}
if ((digitalRead(floor2switch) == LOW) && (prevbutton2 == 1))
{
prevbutton2 = 0;
}

//-------------------------------------------------------------------------

//check floor 3

if (digitalRead(2) == HIGH){
	if ((floor3status == 1) && (prevbutton3 != 1)){
		
		floor3status = 0;
		prevbutton3 = 1; 
		
		}
        else if ((floor3status == 0) && (prevbutton3 != 1)){
		
		floor3status = 1;
		prevbutton3 = 1; 
		
		}
	
}
if ((digitalRead(floor3switch) == LOW) && (prevbutton3 == 1))
{
prevbutton3 = 0;
}	

//-----------------------------------------------------------------------------

//check floor 4

if (digitalRead(3) == HIGH){
	if ((floor4status == 1) && (prevbutton4 != 1)){
		
		floor4status = 0;
		prevbutton4 = 1; 
		
		}
        else if ((floor4status == 0) && (prevbutton4 != 1)){
		
		floor4status = 1;
		prevbutton4 = 1; 
		
		}
	
}
if ((digitalRead(floor4switch) == LOW) && (prevbutton4 == 1))
{
prevbutton4 = 0;
}

//-------------------------------------------------------------------------
//check floor 5

if (digitalRead(4) == HIGH){
	if ((floor5status == 1) && (prevbutton5 != 1)){
		
		floor5status = 0;
		prevbutton5 = 1; 
		
		}
        else if ((floor5status == 0) && (prevbutton5 != 1)){
		
		floor5status = 1;
		prevbutton5 = 1; 
		
		}
	
}
if ((digitalRead(floor5switch) == LOW) && (prevbutton5 == 1))
{
prevbutton5 = 0;
}	
	
//--------------------------------------------------------------------------

//check floor 6

if (digitalRead(5) == HIGH){
	if ((floor6status == 1) && (prevbutton6 != 1)){
		
		floor6status = 0;
		prevbutton6 = 1; 
		
		}
        else if ((floor6status == 0) && (prevbutton6 != 1)){
		
		floor6status = 1;
		prevbutton6 = 1; 
		
		}
	
}
if ((digitalRead(floor6switch) == LOW) && (prevbutton6 == 1))
{
prevbutton6 = 0;
}	

//---------------------------------------------------------------------------


// check master button


if (digitalRead(6) == HIGH){
	if ((masterstatus == 1) && (prevmaster == 0)){
		
		masterstatus = 0;
		prevmaster = 1; 
		
		}
        else if ((masterstatus == 0) && (prevmaster == 0)){
		
		masterstatus = 1;
		prevmaster = 1; 
		
		}
	
}
if ((digitalRead(6) == LOW) && (prevmaster == 1))
{
prevmaster = 0;
}	

//-------------------------------------------------------------------------


		
//setfloorvalues given updated floor status values 
// if the master status is on,  then each individual floor controls its own status
if (!masterstatus){
digitalWrite(7,(!floor1status));
digitalWrite(8,(!floor2status));
digitalWrite(9,(!floor3status));
digitalWrite(10,(!floor4status));
digitalWrite(11,(!floor5status));
digitalWrite(12,(!floor6status));
}
// if the master status is off,  all floors are off. 
else{
digitalWrite(7,HIGH);
digitalWrite(8,HIGH);
digitalWrite(9,HIGH);
digitalWrite(10,HIGH);
digitalWrite(11,HIGH);
digitalWrite(12,HIGH);
}

//--------------------------------------------------------------------------

//check for faults

if(digitalRead(14) != floor1status){

//	if (floor1delaycount > 0){
//	floor1delaycount --;
//	}

	if (floor1errorflag == 0){
		printf("There is an unauthorised powerdown on floor 1");
		floor1errorflag = 1;
	}
}

if(floor1errorflag && (digitalRead(14) == floor1status))
	{
	floor1errorflag = 0;
	}


//--------------------------------------------------------------------------
}

return 0;

}



