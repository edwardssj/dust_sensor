      /*
       * SD card attached to SPI bus as follows:
       ** UNO:  MOSI - pin 11, MISO - pin 12, CLK - pin 13, CS - pin 4 (CS pin can be changed)
        and pin #10 (SS) must be an output
       ** Mega:  MOSI - pin 51, MISO - pin 50, CLK - pin 52, CS - pin 4 (CS pin can be changed)
        and pin #52 (SS) must be an output
       ** Leonardo: Connect to hardware SPI via the ICSP header
   */
      #include <SPI.h>
      #include <RTClib.h> 
      #include "DHT.h"
      #include <SD.h>    
      #include <Wire.h>
      #include "Chronodot.h"  
      Chronodot RTC;
      DateTime time;  
      
      #define CLOCK_ADDRESS 0x68
      #define DHTPIN 9     // what pin we're connected to
      #define DHTTYPE DHT22   // DHT 22  (AM2302)    
      // Connect sensor's Vin pin to +5V
      // Connect middle pin to whatever your DHTPIN is
      // Connect GND pin to GROUND
      // Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
      DHT dht(DHTPIN, DHTTYPE);
      
      #define S0 4 // Mux has two selector pins, up to 4 combinations.S0 connected to digital pin 4
      #define S1 5 // S1 connected to digital pin 5
      #define enablemux 6
      
      File dataFile;
      
      // change this to match your SD shield or module;
      //  Adafruit SD shields and modules: pin 10
      const int chipSelect = 10;
      
      
      // Declare the variables strings and char for naming the SD Card file name
      String fname;
      String currdir;
      char file_fname[50];
  
      String sdCard;
            
     //Dust sensor set up & controls  
      int dust_read_Pin = 0; //Connect dust sensors to Arduino pins A0-A5
      int dustTrig = 2;   //Dust trigger. Connect the led driver pins (pin 3, LED-trig) of dust sensors to Arduino D2
      const int alarmTrig = 8; // Alarm trigger pin. 
      int alarmState = 0;
      
      int samplingTime = 280;  // Microseconds after the trigger, see datasheet.
      int sleepTime = 9720;
  
      long int dustMeasured = 0; //Raw signal from dust sensor 0-1023
      int Nsamples = 100;  // Number of dust sensor samples to collect and average. We usually collect 100 measurements.
      int count = 0; // Dictates when to write file to SD card.
      long int dust = 0;  // Single measruement response.
      long int dusttotal = 0; // Total response before averaging. 
      
      // This is for the analog mux with a 4051 8-channel multiplexer
      int muxArray[8]; // Create an array for the control pins
      
          
      //This function reads the dust signal. 
      long int dustRead() {
      dust=0;
      dusttotal = 0;
      dustMeasured = 0;
           
      for (int i=0;i<Nsamples;i++){  
        digitalWrite(dustTrig,LOW); 
        delayMicroseconds(samplingTime);
        dust=analogRead(dust_read_Pin);
        dusttotal = dusttotal + dust;
        digitalWrite(dustTrig,HIGH); 
        delayMicroseconds(sleepTime);
       }
       dustMeasured = dusttotal/Nsamples; 
     
       return dustMeasured;
}
         
         
         
      // Reading temperature or humidity takes about 250 milliseconds!
      // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
      float tempRead(){
        float t = dht.readTemperature();
        return t;    
      }  
        // Reading temperature or humidity takes about 250 milliseconds!
        // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
      float humidRead(){
        float h = dht.readHumidity();  
        return h;    
      }
      
      
      // This function gets the data from the Chronodot and converts to strings.
       String timestring()
          {
            time=RTC.now();
            
            String xmonth, xday, xhour, xminute, xsecond;
            
            // For one digit months
             if (time.month()<10){
              xmonth="0"+String(time.month());
            }
              else {
               xmonth=String(time.month());
            }
            //One digit days
            if (time.day()<10){
                xday="0"+String(time.day());
                }
              else {
              xday=String(time.day());
              }
                       
            //For one digit hours
             if (time.hour()<10){
              xhour="0"+String(time.hour());
            }
              else {
               xhour=String(time.hour());
            }
            //One digit minutes
            if (time.minute()<10){
                xminute="0"+String(time.minute());
                }
              else {
              xminute=String(time.minute());
              }
            //One digit seconds
            if (time.second()<10){
                xsecond="0"+String(time.second());
                }
              else {
                xsecond=String(time.second());
                }
            // xx2 gives date and time when sensor data collected.    
            String xx=String(time.year())+"/"+xmonth+"/"+xday;
            String xx2= xx+"\t"+xhour+":"+xminute+":"+xsecond;
            
                    
            // Conversion of the month and date to a string which will be displayed as the sdCard file name 
            String xx3=String(time.year())+xmonth+xday; 
            // Obtain the string xx and save as the name of the sdCard file           
            sdCard = xx3;                
            
           return xx2;
          }
      
      // This function writes the headers of the data file on to the SD Card
      void tosdCard(){
        dataFile.print("Date");
        dataFile.print("\t");
        dataFile.print("Time");
        dataFile.print("\t");
        dataFile.print("Dust1");
        dataFile.print("\t");
        dataFile.print("Dust2");
        dataFile.print("\t");
        dataFile.print("Dust3");
        dataFile.print("\t");
        dataFile.print("Dust4");
        dataFile.print("\t");
        dataFile.print("Dust5");
        dataFile.print("\t");
        dataFile.print("Dust6");
        dataFile.print("\t");
        dataFile.print("Dust7");
        dataFile.print("\t");
        dataFile.print("Dust8");
        dataFile.print("\t");
        dataFile.print("Humdity");
        dataFile.print("\t");
        dataFile.println("Temperature");
       
        Serial.println(fname); // Show the file name of the created file
    
        Serial.print("Date");
        Serial.print("\t");
        Serial.print("Time"); 
        Serial.print("\t");
        Serial.print("Dust1");
        Serial.print("\t");
        Serial.print("Dust2");
        Serial.print("\t");
        Serial.print("Dust3");
        Serial.print("\t");
        Serial.print("Dust4");
        Serial.print("\t");
        Serial.print("Dust5");
        Serial.print("\t");
        Serial.print("Dust6");
        Serial.print("\t");
        Serial.print("Dust7");
        Serial.print("\t");
        Serial.print("Dust8");
        Serial.print("\t");
        Serial.print("Humdity");
        Serial.print("\t");
        Serial.print("Temperature");
        Serial.println("\t");
       
        count = 1;
      }
      
      
      // This function writes and read data from the chronodot registers
      void RTC_send_register(byte reg, byte value){
        Wire.beginTransmission(CLOCK_ADDRESS);
        Wire.write(reg); 
        Wire.write(value);
        Wire.endTransmission();
       }
      
      void setup() {
        Serial.begin(9600);
        // initialize the alarm pin as an input:
        pinMode(alarmTrig, INPUT);
        dht.begin();  
        Wire.begin();
        RTC.begin();
        
        pinMode(dustTrig,OUTPUT);
        
        pinMode(S0, OUTPUT); // mux selector pins
        pinMode(S1, OUTPUT);
        pinMode(enablemux, OUTPUT); // mux pin must alway be low to enable input mode.
       
        digitalWrite(enablemux, LOW);
       
       Serial.print("Initializing SD card...");
        // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
        // Note that even if it's not used as the CS pin, the hardware SS pin 
        // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
        // or the SD library functions will not work. 
        pinMode(10, OUTPUT);
         
          if (!SD.begin(chipSelect)) {
          Serial.println("initialization failed!");
	  return;
        }   
           Serial.println("initialization done.");
         // Checks if the RTC is running
          if (! RTC.isrunning()) {
             Serial.println("RTC is NOT running!");
          }
          
          /*DateTime now = RTC.now();
	  // following line sets the RTC to the date & time this sketch was compiled
	  RTC.adjust(DateTime(__DATE__, __TIME__));*/

          // Set the alarm to go off at each minute
          // Write to the 7,8,9 and A registers on Alarm1
          RTC_send_register(0x07,B00000000);
          RTC_send_register(0x08,B10000000);
          RTC_send_register(0x09,B10000000);
          RTC_send_register(0x0A,B10000000);
          // This will enable Alarm and also enables the alarm interrupt to be generated on the SQW pin
          RTC_send_register(0x0E,B00000101);  
                                                      
      }
    
    
      void loop() {
        // read the state of the alarm output:
        alarmState = digitalRead(alarmTrig);
        
       // check if the alarm output is high or low.
        if (alarmState == LOW) {   
        RTC_send_register(0x0F,0);    // This is to cancel the alarm flag of the RTC:
        
          
          String currTstr=timestring();    // Call the timestring function to gather current date and time
          
	  //Call the dust measure function to measure dust. Can measure up to 8. 
	   for (int i=0; i<8; i++) {  // cycle through the analog read pins.
            digitalWrite(S1, (i&3)>>1);  
            digitalWrite(S0, (i&1));
           {if (i<4)  {
            dust_read_Pin = 0;
            dustMeasured = dustRead();
          }
            else
            dust_read_Pin = 1;
            dustMeasured = dustRead();
          }            
            muxArray[i] = dustMeasured;
          }
          
	  // Call the humidity and Temperature functions to read the current values 
	  float h = humidRead();
	  float t = tempRead();
	  // open the file. Note that only one file can be open at a time,
          // Thus you have to close this one before opening another.
          //fname=String("DATA_"+String(sdCard)+".txt");
          fname=String(String(sdCard)+".txt");
          fname.toCharArray(file_fname,fname.length()+1); // Convert the string to char   
          dataFile = SD.open(file_fname, FILE_WRITE);
         
                 
           // if the file has opend, call the functions then write the data on the file
          if (dataFile) {          
            if (count <1){
              tosdCard();
            }
          
           // Print the current date and time on to the SD Card and the Serial Port
           dataFile.print(currTstr);
           dataFile.print("\t");
           Serial.print(currTstr);
           Serial.print("\t");
           
         // check if returns are valid, if they are NaN (not a number) then something went wrong!
          if (isnan(t) || isnan(h)) {
            dataFile.println("Failed to read from DHT");
            Serial.println("Failed to read from DHT");
          } else {
            // Write the gathered data on to the SD Card file
	    for (int i=0; i<8; i++) {
            dataFile.print(muxArray[i]);
            dataFile.print("  ");
            }
	    dataFile.print("\t");
	    dataFile.print(h);                     
            dataFile.print("\t");
            dataFile.print(t);
	    dataFile.println("\t");
          }
                   
          // close the file:
          dataFile.close();
	  // Output to serial
	  for (int i=0; i<8; i++) {
          Serial.print(muxArray[i]);
          Serial.print("  ");
          }
	  Serial.print("\t");
	  Serial.print(h);
	  Serial.print("\t");
	  Serial.print(t);
	  Serial.println("\t");
	  } 
          else {
          // if the file didn't open, print an error:
         // Serial.println("error opening test.txt");
           Serial.println("error opening Data.txt");
           
           Serial.println(fname); // If the file does not open, display the file name/ just for testing purpose
           delay(60000);
           }
      }
          
    }

