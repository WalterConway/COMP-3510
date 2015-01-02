
/*****************************************************************************\
* Laboratory Exercises COMP 3510						*
* Author: Jordan Hutcheson							*
* Author: Walter Conway								*
* Date  : April 6, 2013								*
\*****************************************************************************/
/*****************************************************************************\
*                             Global system headers                           *
\*****************************************************************************/
#include "common.h"
/*****************************************************************************\
*                             Global data types                               *
\*****************************************************************************/
/*****************************************************************************\
*                             Global definitions			      *
\*****************************************************************************/
#define MAX_QUEUE_SIZE 2 /* Max Queue Size */
#define MAX_LAB_DEVICE_SIZE 8 /*Max devices in lab exercise*/
/*****************************************************************************\
*                            Global data structures                           *
\*****************************************************************************/
struct node{
	Event event;
	struct node *next;
};

struct queue{
	struct node * tail;
	struct node * head;
	int size;
};
/*****************************************************************************\
*                                  Global data                                *
\*****************************************************************************/
struct queue que0;
struct queue que1;
struct queue que2;
struct queue que3;
struct queue que4;
struct queue que5;
struct queue que6;
struct queue que7;


float totalServiceTime = 0 ;
float totalResponseTime = 0;
float totalTurnAroundTime = 0;
int totalEvents = 0;
int totalServicedEvents = 0;
int totalMissedEvents = 0;
int totalGeneratedEvents = 0;


struct queue* queueAddresses[8] = {&que0,&que1,&que2,&que3,&que4,&que5,&que6,&que7};
int numberOfDevices=0;
float totalResponseTimeByDevice[8] =  {0,0,0,0,0,0,0,0};
float totalTurnAroundTimeByDevice[8] = {0,0,0,0,0,0,0,0};
int totalEventsGeneratedByDevice[8] =  {0,0,0,0,0,0,0,0};
int totalMissedEventsByDevice[8] =  {0,0,0,0,0,0,0,0};
int totalEventServicedByDevice[8] = {0,0,0,0,0,0,0,0};
int nextEventThatShouldBeServiced[8] = {0,0,0,0,0,0,0,0};

/*****************************************************************************\
*                               Function prototypes                           *
\*****************************************************************************/
void Control(void);
void InterruptRoutineHandlerDevice(void);
void BookKeeping();

void calculate(int deviceNumber, int eventNumber, float eventTime, float endOfServiceTime);
void enq(struct queue *q, Event *eventAddr);
Event *deq(struct queue *q);
int isEmpty(struct queue *q);
void create(void);

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
* START CONTROL ROUTINE                                                       *
\*****************************************************************************/
int main (int argc, char **argv) {
   if (Initialization(argc,argv)){
     Control();
   } 
} /* end of main function */

/***********************************************************************\
 * Input : none                                                          *
 * Output: None                                                          *
 * Function: Monitor Devices and process events (written by students)    *
 \***********************************************************************/
void Control(void){
//Local Variables
create();
Event* currentEvent;
float eventTime = 0;
float endOfServiceTime = 0;
int eventNumber = 0;
int deviceNumber = 0;
	while (1) {
		deviceNumber = 0;
		while(queueAddresses[deviceNumber]->size == 0){
		deviceNumber = (deviceNumber+1)%MAX_LAB_DEVICE_SIZE;
		}
		currentEvent =deq(queueAddresses[deviceNumber]);
		eventTime = currentEvent->When;
		eventNumber = currentEvent->EventID;
		Server(currentEvent);
		endOfServiceTime = Now();
		calculate(deviceNumber, eventNumber,eventTime, endOfServiceTime);
		}


}
/***********************************************************************\
* Input : None                                                          *
* Output: None                                                          *
* Function: This routine is run whenever an event occurs on a device    *
*           The id of the device is encoded in the variable flag        *
\***********************************************************************/
void InterruptRoutineHandlerDevice(void){
totalGeneratedEvents++;
float startTime = 0;
float eventTime = 0;
float reponseTime = 0;
Event *currEvent;
int tempFlags = Flags;
Flags = 0;
int deviceNumber = 0;
int eventNumber;
	while(tempFlags != 0){
		if(tempFlags & 1){
			currEvent = &BufferLastEvent[deviceNumber];
			eventTime = currEvent->When;
			eventNumber = currEvent->EventID;
			startTime = Now();
			DisplayEvent('A'+deviceNumber, currEvent);
			totalEventsGeneratedByDevice[deviceNumber]++;
			reponseTime = (startTime-eventTime);
			totalResponseTimeByDevice[deviceNumber] += reponseTime;
			totalResponseTime += reponseTime;
			fprintf(stderr,"The queue for Device %c is: %d\n", 'A'+deviceNumber ,queueAddresses[deviceNumber]->size);
			enq(queueAddresses[deviceNumber], currEvent);
			
		}
	tempFlags >>= 1;
	deviceNumber++;

	}
	
//This finds out how many devices there are
//throughout the life cycle of the program.
if(deviceNumber > numberOfDevices) {
	numberOfDevices = deviceNumber;
}
}

/***********************************************************************\
* Input : None                                                          *
* Output: None                                                          *
* Function: This must print out the number of Events buffered not yet   *
*           not yet processed (Server() function not yet called)        *
\***********************************************************************/
void BookKeeping(void){
fprintf(stderr,"Total Generated Events: %d\n", totalGeneratedEvents);
fprintf(stderr,"Total Unserviced Events: %d\n", totalMissedEvents);
fprintf(stderr,"Total Serviced Events: %d\n", totalServicedEvents);

fprintf(stderr,"Average percentage of missed events: %10.6f%\n", ((float)totalMissedEvents/totalGeneratedEvents)*100);
fprintf(stderr,"Average response time: %10.6f\n",totalResponseTime/totalServicedEvents);
fprintf(stderr,"Average turn around time: %10.6f\n",totalTurnAroundTime/totalServicedEvents);
int i;
	for(i = 0; i < (numberOfDevices +1); i++) {
	if(i == 0 || i == numberOfDevices-1){
		fprintf(stderr,"---------------Information for Device: %d---------------\n",i);
		fprintf(stderr,"Total Generated Events: %d\n", totalEventsGeneratedByDevice[i]);		
		fprintf(stderr,"Total Unserviced Events: %d\n", totalMissedEventsByDevice[i]);
		fprintf(stderr,"Total Serviced Events: %d\n", totalEventServicedByDevice[i]);
		fprintf(stderr,"Average percentage of missed events: %10.6f%\n", ((float)totalMissedEventsByDevice[i]/totalEventsGeneratedByDevice[i])*100);
		fprintf(stderr,"Average response time: %10.6f\n",totalResponseTimeByDevice[i]/totalEventServicedByDevice[i]);
		fprintf(stderr,"Average turn around time: %10.6f\n",totalTurnAroundTimeByDevice[i]/totalEventServicedByDevice[i]);
		}
	}
}

/* Create an empty queue */
void create()
{
	que0.size = 0;
	que0.head = NULL;
	que0.tail = NULL;

	que1.size = 0;
	que1.head = NULL;
	que1.tail = NULL;

	que2.size = 0;
	que2.head = NULL;
	que2.tail = NULL;

	que3.size = 0;
	que3.head = NULL;
	que3.tail = NULL;

	que4.size = 0;
	que4.head = NULL;
	que4.tail = NULL;

	que5.size = 0;
	que5.head = NULL;
	que5.tail = NULL;

	que6.size = 0;
	que6.head = NULL;
	que6.tail = NULL;

	que7.size = 0;
	que7.head = NULL;
	que7.tail = NULL;
}
 
/* En-queuing the queue */
void enq(struct queue *q, Event *eventAddr)
{

	if((q->size) < MAX_QUEUE_SIZE){
	
		struct node * newPtr;
		newPtr = (struct node *)malloc(1*sizeof(struct node));

		if (newPtr != NULL) {
			newPtr->event = *eventAddr;
			newPtr->next = NULL;
			if(isEmpty(q))
			{
				q->head = newPtr;
			} else {
					q->tail->next = newPtr;
				}
		q->tail = newPtr;
		q->size++;

		} else {
			printf("not inserted\n");
		}

} else{
fprintf(stderr, "missed one\n");
}
	
}



/* Dequeing the queue */
Event *deq(struct queue *q)
{
Event *tempEvent;
tempEvent = (struct EventTag *)malloc(sizeof(struct EventTag));
struct node * tempPtr;
*tempEvent = q->head->event;
tempPtr = q->head;
q->head = q->head->next;
if(q->head == NULL){
	q->tail = NULL;
}
free (tempPtr);
q->size--;
return tempEvent;
}

int isEmpty(struct queue *q){
return q->head == NULL;
}


void calculate(int deviceNumber, int eventNumber, float eventTime, float endOfServiceTime){
int missedEvents=0;
int turnAroundTime = 0;
//This finds the number of missed events since last serviced events of the particular device
//and updates two variables: Total missed and total missed by device.
if((nextEventThatShouldBeServiced[deviceNumber]) != eventNumber) {
	missedEvents = eventNumber - (nextEventThatShouldBeServiced[deviceNumber]);
	totalMissedEvents += missedEvents;
	totalMissedEventsByDevice[deviceNumber] += missedEvents;
}

nextEventThatShouldBeServiced[deviceNumber] = (eventNumber) + 1;
totalEventServicedByDevice[deviceNumber]++;
turnAroundTime = endOfServiceTime - (eventTime);
totalTurnAroundTimeByDevice[deviceNumber] += turnAroundTime;

totalServicedEvents++;
totalTurnAroundTime += turnAroundTime;

}

