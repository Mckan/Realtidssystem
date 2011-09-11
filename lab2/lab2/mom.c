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
Bit 5 to 0 reflects any pending interrupt request. NOTE: These are set during RESET and cleared when writing the Interrupt Control register (se below). 
Bit 7-6,   Not Used.              
Bit 5,       ICLOSING: Is Closing	1 = Door is moving towards closed position.
Bit 4,       IOPENING: Is Opening     	1 = Door is moving towards open position.
Bit 3,       SLEFT: Sensor A.           1 = Sensor A is activated (Button S1) 
Bit 2,       RIGHT: Sensor B. 		1 = Sensor B is activated (Button S2)
Bit 1,       ISENS2: Sensor 2           1 = Door is closed                
Bit 0,       ISENS1: Sensor 1.          1 = Door is open.
/*******************************************************/

// I/O
#define ML13Status 	0xB00
#define ML13Control 	0xB00
// L�gg till IRQSTATUS/IRWCONTROL 0xB01
#define readControl 	*((char *)ML13Status)
#define setControl(x) 	*((char *)ML13Control) = x

// MASKS
#define OPEN_DOOR 	0x01
#define CLOSE_DOOR 	0x02
#define DOOR_OPENED 	0x08 	// 0000 1000 Bit 3
#define DOOR_CLOSED 	0x32 	// 0010 0000 Bit 5
#define SENSOR 		0x03	// 0000 0011 Bit 1 och 2
#define TIME_FOR_DELAY 	0xFF	// Time for delay, needs to be huge!

int main(int argc, char *argv[])
{
	// 1. Initiera (G�rs ovan med definitionerna?)
	unsigned int i, attemptClose, doorUnlocked;
	// S�tter doorUnlocked till 1 eftersom den inte verka anv�ndas, har �ven
	// kommenterat ut l�s-kod nedan
	doorUnlocked = 1;
	while(1)
	{
		// Om d�rren �r ol�st (det �r den alltid!)
		if(doorUnlocked)
		{
			// D�rren ska inte l�sas, f�rs�k �ppna den som vi f�r utslag
			if(readControl & SENSOR)
			{
				// �ppna d�rren
				setControl(OPEN_DOOR);
				
				// V�nta p� att d�rren �ppnats
				while(!(readControl & DOOR_OPENED));
				
				// S�tt f�rs�ksflaggan
				attemptClose = 1;
				while(attemptClose)
				{
					// V�nta ett tag, st�ng sedan d�rren
					for(i = 0; i < TIME_FOR_DELAY; i++)
					{
						// Har vi f�tt nytt utslag?
						if(readControl & SENSOR)
						{
							// Nollst�ll d� r�knaren f�r att b�rja r�kna om
							i = 0;
						}
					}
					
					// Tiden �r ute, p�b�rja st�ngning av d�rren
					attemptClose = 0;
					setControl(CLOSE_DOOR);
					
					// Kolla s� att ingen vill �ppna d�rren under st�ngning
					while(!(readControl & DOOR_CLOSED))
					{
						// Har vi nytt utslag under st�ngning?
						if(readControl & SENSOR)
						{
							// �ppna d� d�rren
							setControl(OPEN_DOOR);
							
							// S�tt st�ngningsflaggan igen f�r att b�rja om
							attemptClose = 1;
							break;
						}
						
					}
				}
			}
		}
	}
	return 0;
}




