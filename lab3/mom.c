/************** ML13 STATUS REGISTER READ **************\
Bit 7, 		CLOSING.	  	1 = The door is closing.      
Bit 6, 		OPENING. 	  	1 = Door is opening             
Bit 5,       	S2: Sense 2.         	1 = Door is closed                
Bit 4,       	S4: Sense 4.         	1 = passed  Sensor 4 and is nearly closed     
Bit 3,       	S1: Sense 1.         	1 = Door is wide open.        
Bit 2,       	S3: Sense 3.         	1 = passed  Sensor 3 and is nearly open.      
Bit 1,       	RIGHT: Sensor B. 	1 = Sensor B is activated (Button S2) 
Bit 0,       	LEFT: Sensor A.      	1 = Sensor A is activated (Button S1) 
/*******************************************************\
* ***************************************************** *
/************ ML13 CONTROL REGISTER WRITE **************\
Bit 7-2,     	Not used.               
Bit 1,       	CLOSE   		1 = Actuate "Close Door" 
Bit 0,       	OPEN     		1 = Actuate "Open Door" 

Setting bit 1 AND bit 0 to 1 will yield an undefined behavior.
/*******************************************************
* ***************************************************** *
/****** ML13 INTERUPT STATUS REGISTER READ BASE+1 ******\
Bit 5 to 0 reflects any pending interrupt request. NOTE: These are set during
RESET and cleared when writing the Interrupt Control register (se below).
 
Bit 7-6,   	Not Used.              
Bit 5,       	ICLOSING: Is Closing	1 = Door is moving towards closed position.
Bit 4,       	IOPENING: Is Opening    1 = Door is moving towards open position.
Bit 3,       	SLEFT: Sensor A.        1 = Sensor A is activated (Button S1) 
Bit 2,       	RIGHT: Sensor B. 	1 = Sensor B is activated (Button S2)
Bit 1,       	ISENS2: Sensor 2        1 = Door is closed                
Bit 0,       	ISENS1: Sensor 1.       1 = Door is open.
/*******************************************************/

// I/O
#define ML13Status 	0xB01
#define ML13Control 	0xB01
// Lägg till IRQSTATUS/IRWCONTROL 0xB01
#define readControl 	*((char *)ML13Status)
#define setControl(x) 	*((char *)ML13Control) = x

// IRQ TYPES
#define NO_IRQ_TYPE = 0;

// MASKS
#define OPEN_DOOR 	0x01
#define CLOSE_DOOR 	0x02
#define DOOR_OPENED 	0x04 	// 0000 0100 Bit 3
#define DOOR_CLOSED 	0x02 	// 0010 0000 Bit 5
#define SENSOR 		0x01	// 0000 0011 Bit 1 och 2
#define TIME_FOR_DELAY 	0xFF	// Time for delay
#define TIME_OUT	0x20	// Timeout is given

int interrupt_type;
//int standby();

/**

TODO


*/

int main()
{
	// 1. Initiera (Görs ovan med definitionerna?)
	unsigned int door = DOOR_CLOSED;
	init_irq();

	while(1)
	{
		// Sätt IRQ typ till 0 eftersom vi inte har avbrott
		interrupt_type = NO_IRQ_TYPE;
		
		// Vänta på avbrott, WAI i assembler modulen
		standby();
		
		// Nu har vi avbrott, switcha på interrupt_type som sätts
		// i assemblermodulen för att avgöra vilken typ av avbrott
		switch(interrupt_type)
		{
			// Sensor har gett utslag, öppna dörren
			case SENSOR:
				if(door == DOOR_CLOSED)
					setControl(OPEN_DOOR);
				break;
			// Dörren har stängts
			case DOOR_CLOSED:
				door = DOOR_CLOSED;
				break;
			// Dörren har öppnats, börja räkna ner innan den ska stängas igen
			case DOOR_OPENED:
				door = DOOR_OPENED;
				set_timeout(30);
				break;
			// Timeouten är klar, stäng nu dörren
			case TIME_OUT:
				if(door == DOOR_OPENED)
					setControl(CLOSE_DOOR);
				break;
		}
	}
	return 0;
}




