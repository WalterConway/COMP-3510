
/*****************************************************************************\
* Laboratory Exercises COMP 3510                                              *
* Author: Saad Biaz                                                           *
* Date  : January 18, 2014                                                    *
\*****************************************************************************/

/*****************************************************************************\
*                             Global system headers                           *
\*****************************************************************************/


#include "common.h"

/*****************************************************************************\
*                             Global data types                               *
\*****************************************************************************/



/*****************************************************************************\
*                             Global definitions                              *
\*****************************************************************************/





/*****************************************************************************\
*                            Global data structures                           *
\*****************************************************************************/




/*****************************************************************************\
*                                  Global data                                *
\*****************************************************************************/

float totalServiceTime = 0 ;
float totalResponseTime = 0;
float totalTurnAroundTime = 0;

int totalEvents = 0;
int totalServicedEvents = 0;
int totalMissedEvents = 0;

int totalMissedEventsByDevice[MAX_NUMBER_DEVICES]  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int totalEventServicedByDevice[MAX_NUMBER_DEVICES] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


/*****************************************************************************\
*                               Function prototypes                           *
\*****************************************************************************/

void Control(void);

/*****************************************************************************\
* function: main()                                                            *
* usage:    Create an artificial environment for embedded systems. The parent *
*           process is the "control" process while children process will gene-*
*           generate events on devices                                        *
*******************************************************************************
* Inputs: ANSI flat C command line parameters                                 *
* Output: None                                                                *
*                                                                             *
* INITIALIZE PROGRAM ENVIRONMENT                                              *
* WHILE JOBS STILL RUNNING                                                    *
*    CREATE PROCESSES                                                         *
*    RUN LONG TERM SCHEDULER                                                  *
*    RUN SHORT TERM SCHEDULER                                                 *
*    DISPATCH NEXT PROCESS                                                    *
\*****************************************************************************/

int main (int argc, char **argv) {

   if (Initialization(argc,argv)){
     Control();
   } 
} /* end of main function */

/***********************************************************************\
 * Input : none                                                          *
 * Output: None                                                          *
 * Function: Monitor Devices and process events                          *
 \***********************************************************************/
void Control(void){

//Local Variables
Event *currentEvent;
int deviceNumber = 0;
int eventNumber = 0;
int flagTemp = 0;
float beginServiceTime = 0;
float endServiceTime = 0;
float responseTime = 0;
float turnAroundTime = 0;
float eventTime = 0;
int nextEventServiced[MAX_NUMBER_DEVICES] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	while (1) {
		if(Flags != 0 ) {
	
			flagTemp = Flags;
			Flags = 0;
			deviceNumber = 0;
			while(flagTemp != 0) {
				if(flagTemp & 1) {
	
				    //Storing current event's information to reference
					{
						currentEvent = &BufferLastEvent[deviceNumber];
						eventNumber = currentEvent->EventID;
						eventTime = currentEvent->When;	
					}
			
			
					//This finds the number of missed events since last serviced events of the particular device
					//and updates two variables: Total missed and total missed by device.
					if((nextEventServiced[deviceNumber]) != eventNumber) {
						totalMissedEvents += currentEvent->EventID - (nextEventServiced[deviceNumber]);
						totalMissedEventsByDevice[deviceNumber] += (eventNumber-(nextEventServiced[deviceNumber]));
					}

					//This Displays the event and records the begining service time.
					//This also services the event and records the end of service time.
					{
						beginServiceTime = Now();
						DisplayEvent('A'+ deviceNumber, currentEvent);
						Server(currentEvent);
						endServiceTime = Now();
					}
			
					//This updates the total number of serviced events
					//by device and overall as well as recording the next event
					//that should be service this helps in determining the amount
					//of services missed since last serviced event took place.
					{
						totalEventServicedByDevice[deviceNumber]++;
						totalServicedEvents++;
						nextEventServiced[deviceNumber] = (eventNumber) + 1;
					}

					//Calculation being done to determine the response time,
					//total response time, turn around time and total turn around time.
					{
						responseTime = beginServiceTime - (eventTime);
						totalResponseTime += responseTime;	
 						turnAroundTime = endServiceTime - (eventTime);
						totalTurnAroundTime += turnAroundTime;
					}
				}

				flagTemp >>= 1;
				deviceNumber++;
			}
		}
	}
}

/***********************************************************************\
* Input : None                                                          *
* Output: None                                                          *
* Function: This must print out the number of Events buffered not yet   *
*           not yet processed (Server() function not yet called)        *
\***********************************************************************/
void BookKeeping(void){
printf("\n >>>>>> Done\n");
int totalGeneratedEvents = totalServicedEvents + totalMissedEvents;
fprintf(stderr,"Total Generated Events: %d\n", totalGeneratedEvents);
fprintf(stderr,"Total Unserviced Events: %d\n", totalMissedEvents);
fprintf(stderr,"Total Serviced Events: %d\n", totalServicedEvents);
fprintf(stderr,"Average percentage of missed events: %10.6f%\n", ((float)totalMissedEvents/totalGeneratedEvents)*100);
fprintf(stderr,"Average response time: %10.6f\n",totalResponseTime/totalServicedEvents);
fprintf(stderr,"Average turn around time: %10.6f\n",totalTurnAroundTime/totalServicedEvents);
}

