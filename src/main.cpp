#include <OneWire.h>
#include <DallasTemperature.h>
#include <Arduino.h>
#include "WIFIConnector_MKR1000.h"
#include "MQTTConnector.h"
#include "Wifi.h"

 
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 9 // Lower resolution

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

int numberOfDevices; // Number of temperature devices found


DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address



// function to print a device address
String printAddress(DeviceAddress deviceAddress)
{
   
  String idCapteur1 ;
  
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16){ 
   
      idCapteur1 = idCapteur1 + '0';
      idCapteur1 = idCapteur1 +  String(deviceAddress[i], HEX);

      } else
     {
       idCapteur1 = idCapteur1 +  String(deviceAddress[i], HEX);
     }
    
    
  }
 Serial.println(idCapteur1); 
 return idCapteur1 ;
 
}



void setup(void)
{
  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");

  wifiConnect();                  //Branchement au réseau WIFI
  MQTTConnect();  


   
  // Start up the library
  sensors.begin();
  
  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();
  
  // locate devices on the bus
  Serial.print("Locating devices...");
  
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");
  
  // Loop through each device, print out address
  for(int i=0;i<numberOfDevices; i++)
  {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i))
	{
		Serial.print("Found device ");
		Serial.print(i, DEC);
		Serial.print(" with address: ");
		printAddress(tempDeviceAddress);
		Serial.println();
		
		Serial.print("Setting resolution to ");
		Serial.println(TEMPERATURE_PRECISION, DEC);
		
		// set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
		sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
		
		Serial.print("Resolution actually set to: ");
		Serial.print(sensors.getResolution(tempDeviceAddress), DEC); 
		Serial.println();
	}else{
		Serial.print("Found ghost device at ");
		Serial.print(i, DEC);
		Serial.print(" but could not detect address. Check power and cabling");

	}  

        byte mac[6];
        WiFi.macAddress(mac);
        String addrMac = printMacAddress(mac);
        appendPayload2( "Mac adresse", addrMac);
        sendPayload2(); 
  }

}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  // method 1 - slower
  //Serial.print("Temp C: ");
  //Serial.print(sensors.getTempC(deviceAddress));
  //Serial.print(" Temp F: ");
  //Serial.print(sensors.getTempF(deviceAddress)); // Makes a second call to getTempC and then converts to Fahrenheit

  // method 2 - faster
  float tempC = sensors.getTempC(deviceAddress);
  if(tempC == DEVICE_DISCONNECTED_C) 
  {
    Serial.println("Error: Could not read temperature data");
    return;
  }
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" Temp F: ");
  Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit
}


void loop(void)
{ 

  
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  
  
  
  // Loop through each device, print out temperature data
 for(int i=0;i<numberOfDevices; i++)
  {
     //Search the wire for address
        if(sensors.getAddress(tempDeviceAddress, i))
      {
        // Output the device ID
        Serial.print("Temperature for device: ");
        Serial.println(i,DEC);
        
        
        float valtemp =  sensors.getTempC(tempDeviceAddress); 
        Serial.println();
       String addrCapteur = printAddress(tempDeviceAddress);


        //delay(2000);
        appendPayload(addrCapteur ,valtemp);
        sendPayload(); 
        delay(5000);
        
        
      } 
  } 
    

	
 
  

}

