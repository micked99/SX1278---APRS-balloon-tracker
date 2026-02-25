//---BSS---
#include "config.h"
#include "aprs.h"
#include "gps.h"
#include "geofence.h"
#include <avr/power.h>
#include <AltSoftSerial.h>

AltSoftSerial mySerial;
aprs aprs_module;
gps gps_module;
geofence geofence_module;
float gps_latitude;
float gps_longitude;
float gps_altitude;
int gps_satellites;
int gps_speed;
String gps_latitude_DMH;
String gps_longitude_DMH;
int mcu_input_voltage;
int solar_input_voltage;
float ref_millis = float(aprs_packet_delay) * -1000;
int aprs_packet_counter = 1;

void setup()
{
  pinMode(A1, OUTPUT); //GPS on
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  digitalWrite(A1, HIGH);
  digitalWrite(A2, HIGH);
  digitalWrite(A3, HIGH);
  digitalWrite(A4, HIGH);
  digitalWrite(A5, HIGH);

  //clock_prescale_set(clock_div_2);

  // pinMode(7, OUTPUT); digitalWrite(7, LOW);delay (10);digitalWrite(7, HIGH); // resets sx1278
  // pinMode(A0, OUTPUT); digitalWrite(A0, HIGH);  // Make sure the GPS is on

   mySerial.begin(9600);
   delay(1200); mySerial.println("$PCAS04,1*18\r\n");    // Sets ATGM336 to GPS only
   mySerial.end;

  // turn serial debug on or off
  #ifdef enable_serial_debug
    Serial.begin(serial_baud_rate);
    #define DEBUG_PRINT(x)  Serial.print (x)
    #define DEBUG_PRINTLN(x)  Serial.println (x)
  #else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
  #endif
  
  aprs_module.begin();  // begin sx1278 spi connection
  gps_module.m_pgps_serial->begin(gps_baud_rate);  // set serial baud of gps module
 }

void loop()
{
  while (gps_module.m_pgps_serial->available())
  {
    // read gnss position data
    if (gps_module.m_pgps_device->encode(gps_module.m_pgps_serial->read())) 
    {
      // valid gnss position-fix
      if (gps_module.m_pgps_device->location.isValid())
      {
       // DEBUG_PRINTLN(millis() - ref_millis);
        
        // no packet data rate limitation - send aprs paket
        if((millis() - ref_millis) > float(aprs_packet_delay) * 1000) 
        {
          send_aprs_packet();
          ref_millis = millis();
        }     
      }
        // no packet data rate limitation
     else 
      {
      //  DEBUG_PRINTLN("INVALID");
        
        // send aprs packet after gps timeout
        if(millis() >= float(gps_timeout) * 1000)
        {
         // DEBUG_PRINTLN(millis() - ref_millis);
        
          if((millis() - ref_millis) > float(aprs_packet_delay) * 1000)
          {
            send_aprs_packet();
           
            ref_millis = millis();
          }  
        }
      }
    }
  }
}

void send_aprs_packet()
{

  // get gps location,altitude and satellite data
  gps_latitude = gps_module.m_pgps_device->location.lat();
  gps_longitude = gps_module.m_pgps_device->location.lng();
  gps_altitude = gps_module.m_pgps_device->altitude.meters();
  gps_satellites = gps_module.m_pgps_device->satellites.value();
  gps_speed = int (gps_module.m_pgps_device->speed.kmph());
  //gps_course = int (gps_module.m_pgps_device->course.deg());  
  
  gps_module.m_pgps_serial->end();   // needed for aprs to properly work

  // get aprs frequency using geofence
  float freq = geofence_module.get_aprs_frequency(gps_latitude, gps_longitude);
  
  // convert gps coordinates to DMH format needed for aprs
  gps_module.convert_gps_coordinates_to_DMH(gps_latitude, gps_longitude, &gps_latitude_DMH, &gps_longitude_DMH);
  
  // print out gps data
  //DEBUG_PRINTLN(gps_latitude);
  //DEBUG_PRINTLN(gps_longitude);
  //DEBUG_PRINTLN(gps_altitude);
  //DEBUG_PRINTLN(gps_satellites);

  // create aprs comment with payload data
  String aprs_comment = aprs_module.create_comment(gps_altitude, aprs_packet_counter, gps_speed, gps_satellites, additional_aprs_comment); 
  
  aprs_module.send(gps_latitude_DMH, gps_longitude_DMH, aprs_comment, freq);

  // increase aprs packet counter by one
  aprs_packet_counter = aprs_packet_counter + 1;
  
  gps_module.m_pgps_serial->begin(gps_baud_rate);   // needed for aprs to properly work
}
