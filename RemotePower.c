
int in = 0;
int stat = 0;
int mode = 0;
int port;
int pstat[4] = {0,0,0,0};
int ppin[4] = {2, 3, 4, 5};

void setup() { 
	int i ; 

	Serial.begin(115200); 
	while (!Serial) {
		Serial.println("Wait Init Serial"); 
	}
	pinMode(ppin[0], OUTPUT);
	pinMode(ppin[1], OUTPUT);
	pinMode(ppin[2], OUTPUT);
	pinMode(ppin[3], OUTPUT);
	
	digitalWrite(ppin[0], HIGH); 
	digitalWrite(ppin[1], HIGH); 
	digitalWrite(ppin[2], HIGH); 
	digitalWrite(ppin[3], HIGH); 
	
	Serial.println("Serial Remote Power 0.1");
	PrintPromt();
} 

void PrintPromt () {
	Serial.println("");
	Serial.print(pstat[0],DEC);
	Serial.print(pstat[1],DEC);
	Serial.print(pstat[2],DEC);
	Serial.print(pstat[3],DEC);
	Serial.print(" # ");
}

void setGPIO(int cport, int cmode) {
	Serial.println("");
	int iport = cport-48;
	int imode = cmode-48;
	/* Serial.println(iport,DEC);
	Serial.println(imode,DEC); */
	pstat[iport-1]=imode;
	if(imode == 1) {
		digitalWrite(ppin[iport-1], LOW); 
	} else if(imode == 0){
		digitalWrite(ppin[iport-1], HIGH); 
	}
}

void loop() { 
    
	if (Serial.available() > 0) {
		in = Serial.read();
		
		if (stat == 0 && in == 'p') {
			stat = 0;
			PrintPromt();
		} 
		else if (stat == 0 && in == 's') {
			Serial.write(in);
			stat = 1;
		} 
		else if (stat == 1 && (in > '0' && in < '5')) {
			Serial.write(in);
			port = in;
			stat = 2;
		}
		else if (stat == 2 && (in =='0' || in == '1')) {
			Serial.write(in);
			mode = in;
			stat = 0;
			setGPIO(port, mode);
			PrintPromt();
		}
	}
} 
