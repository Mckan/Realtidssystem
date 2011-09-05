/************** ML13 STATUS REGISTER READ **************\
﻿Bit 7, 		 CLOSING.			  1 = The door is closing.      
Bit 6, 		 OPENING. 			  1 = Door is opening             
Bit 5,       S2: Sense 2.         1 = Door is closed                
Bit 4,       S4: Sense 4.         1 = passed  Sensor 4 and is nearly closed     
Bit 3,       S1: Sense 1.         1 = Door is wide open.        
Bit 2,       S3: Sense 3.         1 = passed  Sensor 3 and is nearly open.      
Bit 1,       RIGHT: Sensor B. 	  1 = Sensor B is activated (Button S2) 
Bit 0,       LEFT: Sensor A.      1 = Sensor A is activated (Button S1) 
/*******************************************************\
* ***************************************************** *
/************ ML13 CONTROL REGISTER WRITE **************\
﻿Bit 7-2,     Not used.               
Bit 1,       CLOSE   			  1 = Actuate "Close Door" 
Bit 0,       OPEN     			  1 = Actuate "Open Door" 

Setting bit 1 AND bit 0 to 1 will yield an undefined behavior.
/*******************************************************/

// I/O
#define ML13Status 		0xB00
#define ML13Control 	0xB00
#define readControl 	*((char *)ML13Status)
#define setControl(x) 	*((char *)ML13Control) = x

// MASKS
#define OPEN_DOOR 		0x01
#define CLOSE_DOOR 		0x02
#define DOOR_OPENED 	0x08 	// 0000 1000 Bit 3
#define DOOR_CLOSED 	0x32 	// 0010 0000 Bit 5
#define SENSOR 			0x03	// 0000 0011 Bit 1 och 2
#define TIME_FOR_DELAY 	0xFFFF	// Time for delay, needs to be huge!

int main(int argc, char *argv[])
{
	// 1. Initiera (Görs ovan med definitionerna?)
	unsigned int i, attemptClose, doorUnlocked;
	// Sätter doorUnlocked till 1 eftersom den inte verka användas, har även
	// kommenterat ut lås-kod nedan
	doorUnlocked = 1;
	while(1)
	{
		// Om dörren är olåst (det är den alltid!)
		if(doorUnlocked)
		{
			// Dörren ska inte låsas, försök öppna den som vi får utslag
			if(readControl & SENSOR)
			{
				// Öppna dörren
				setControl(OPEN_DOOR);
				
				// Vänta på att dörren öppnats
				while(!(readControl & DOOR_OPENED));
				
				// Sätt försöksflaggan
				attemptClose = 1;
				while(attemptClose)
				{
					// Vänta ett tag, stäng sedan dörren
					for(i = 0; i < TIME_FOR_DELAY; i++)
					{
						// Har vi fått nytt utslag?
						if(readControl & SENSOR)
						{
							// Nollställ då räknaren för att börja räkna om
							i = 0;
						}
					}
					
					// Tiden är ute, påbörja stängning av dörren
					attemptClose = 0;
					setControl(CLOSE_DOOR);
					
					// Kolla så att ingen vill öppna dörren under stängning
					while(!(readControl & DOOR_CLOSED))
					{
						// Har vi nytt utslag under stängning?
						if(readControl & SENSOR)
						{
							// Öppna då dörren
							setControl(OPEN_DOOR);
							
							// Sätt stängningsflaggan igen för att börja om
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




