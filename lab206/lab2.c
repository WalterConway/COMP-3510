
/*****************************************************************************\
* Laboratory Exercises COMP 3510						*
* Author: Jordan Hutcheson							*
* Author: Walter Conway								*
* Date  : March 18, 2013								*
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
/*****************************************************************************\
*                            Global data structures                           *
\*****************************************************************************/
typedef struct node
{
	Event *event;
	float beginServiceTime;
	struct node *ptr;
} node_t;

struct queue{
	node_t * tail;
	node_t * head;
	int size;
};
/*****************************************************************************\
*                                  Global data                                *
\*****************************************************************************/
struct queue que;

float totalServiceTime = 0 ;
float totalResponseTime = 0;
float totalTurnAroundTime = 0;

int numberOfDevices = 0;
int totalEvents = 0;
int totalServicedEvents = 0;
int totalMissedEvents = 0;
int totalGeneratedEvents = 0;

int nextEventThatShouldBeServiced[MAX_NUMBER_DEVICES] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

/*****************************************************************************\
*                               Function prototypes                           *
\*****************************************************************************/
void Control(void);
void InterruptRoutineHandlerDevice(void);
void BookKeeping();

Event *headelement(void);
void enq(Event *eventAddr);
Event *deq(void);
void empty(void);
void display(void);
int queuesize(void);
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

Event *currentEvent;
float turnAroundTime = 0;

int deviceNumber = 0;
int eventNumber = 0;

float eventTime = 0;
float begunTime = 0;
float endServiceTime = 0;

	while (1) {
		while(queuesize() != 0) {

				//Storing current event's information to reference
				{
					currentEvent = deq();
					eventNumber = currentEvent->EventID;
					eventTime = currentEvent->When;
					deviceNumber = currentEvent->DeviceID;
				}

				//This finds the number of missed events since last serviced events of the particular device
				//and updates two variables: Total missed and total missed by device.
				if((nextEventThatShouldBeServiced[deviceNumber]) != eventNumber) {
					totalMissedEvents += eventNumber - (nextEventThatShouldBeServiced[deviceNumber]);
				}

				//This Displays the event and records the begining service time.
				//This also services the event and records the end of service time.
				{
					Server(currentEvent);
					endServiceTime = Now();
				}
		
				//This updates the total number of serviced events
				//by device and overall as well as recording the next event
				//that should be service this helps in determining the amount
				//of services missed since last serviced event took place.
				{
					totalServicedEvents++;
					nextEventThatShouldBeServiced[deviceNumber] = (eventNumber) + 1;
				}

				//Calculation being done to determine the response time,
				//total response time, turn around time and total turn around time.
				{
					turnAroundTime = endServiceTime - (eventTime);
					totalTurnAroundTime += turnAroundTime;
				}
				
				//This finds out how many devices there are
				//throughout the life cycle of the program.
				if(deviceNumber > numberOfDevices) {
					numberOfDevices = deviceNumber;
				}
		}
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
Event *currEvent;
float startTime = 0;
float eventTime = 0;
int tempFlags = Flags;
Flags = 0;
int count = 0;
	while(tempFlags != 0){
		if(tempFlags & 1){
			currEvent = &BufferLastEvent[count];
			eventTime = currEvent->When;
			startTime = Now();
			DisplayEvent('A'+count, currEvent);
			enq(currEvent);
			fprintf(stderr,"size of queue: %d\n",queuesize());
			totalResponseTime += startTime - eventTime;
		}
	tempFlags >>= 1;
	count++;
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
}

/* Create an empty queue */
void create()
{
	que.size = 0;
	que.head = NULL;
	que.tail = NULL;
}

/* Returns queue size */
int queuesize()
{
   return que.size;
}
 
/* Enqueing the queue */
void enq(Event *eventAddr)
{
	if (que.tail == NULL)
    {
        que.tail = (struct node *)malloc(1*sizeof(struct node));
        que.tail->ptr = NULL;
		que.tail->event = eventAddr;
		que.head = que.tail;
    }
    else
    {
		node_t *temp;
        temp=(struct node *)malloc(1*sizeof(struct node));
		que.tail->ptr = temp;
		temp->event = eventAddr;
        temp->ptr = NULL;
		que.tail = temp;
    }
	que.size++;
}

 
/* Displaying the queue elements */
void display()
{
	node_t *front1;
	front1 = que.head;
 
	if ((front1 == NULL) && (que.tail == NULL))
    {
        printf("Queue is empty");
        return;
    }
    while (front1 != que.tail)
    {
	DisplayEvent('Z',front1->event);	
        front1 = front1->ptr;
    }
    if (front1 == que.tail)
       DisplayEvent('Z',front1->event);
}
 
/* Dequeing the queue */
Event *deq()
{
	node_t *front1;
	Event *tempEvent = NULL;
	front1 = que.head;
 
    if (front1 == NULL)
    {
        printf("\n Error: Trying to display elements from empty queue");
        return tempEvent;
    }
    else
        if (front1->ptr != NULL)
        {
            front1 = front1->ptr;
			tempEvent = que.head->event;
			free(que.head);
			que.head = front1;
        }
        else
        {
			tempEvent = que.head->event;
			free(que.head);
			que.head = NULL;
			que.tail = NULL;
        }
		que.size--;
		return tempEvent;
}
 
/* Returns the front element of queue */
Event *headelement()
{
	if ((que.head != NULL) && (que.tail != NULL))
		return(que.head->event);
    else
        return 0;
}

/* Display if queue is empty or not */
void empty()
{
	if ((que.head == NULL) && (que.tail == NULL))
        printf("\n Queue empty");
    else
       printf("Queue not empty");
}

