// AFSK modulator based on example from github.com/handiko/ Arduino-APRS
#include <Arduino.h>
#include <math.h>
#include <stdio.h>
#include <SPI.h>                                       
#include <SX127XLT.h>
#include "aprs.h"
#include "config.h"
 
aprs::aprs()
: m_pSX1278(NULL)
{   
  if (!m_pSX1278)
  {
    m_pSX1278 = new SX127XLT();
  }
}

aprs::~aprs() 
{  
  if (m_pSX1278)
  {
    delete m_pSX1278;
    m_pSX1278 = NULL;
  }
}

void aprs::begin(void) 
{
  // init sx1278 radio connection
  SPI.begin();
  m_pSX1278->begin(NSS, NRESET, DIO0, DIO1, DIO2, LORA_DEVICE);
  pinMode(sx1278_radio_pwm_pin, OUTPUT);
  mycall = aprs_source_callsign;
  myssid = aprs_source_ssid;
  dest = aprs_destination_callsign;
  digi = aprs_digipeater;
  digissid = aprs_digipeater_ssid;
  sym_ovl = aprs_symbol_overlay;
  sym_tab = aprs_symbol;
  }

  String aprs::create_comment(int altitude, int aprs_packet_number, int gps_speed, int number_of_gps_satellites, String additional_comment)
  {
    char comment_buffer[80];
    // altitude*3.28084 - for conversion from meter to feet
    sprintf(comment_buffer, "/A=%06ldNr%d Kmh%d GPS%d", long(altitude*3.28085), aprs_packet_number, gps_speed, number_of_gps_satellites, additional_comment.c_str()); // %s // additional_comment.c_str
    return String(comment_buffer);
  }
  
void GPS_VCC_off()
 {
  digitalWrite(8, LOW);    //End Serial
  digitalWrite(9, LOW);    //End Serial
  //digitalWrite(A0, LOW); //pin5 controll
  digitalWrite(A1, LOW);   //GPS Vcc off 
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
  digitalWrite(A4, LOW);
  digitalWrite(A5, LOW);
 }
  
 void GPS_VCC_on()
 {
  digitalWrite(8, HIGH);    //Start serial
  digitalWrite(9, HIGH);    //Start serial
  //digitalWrite(A0, HIGH); //pin5 controll
  digitalWrite(A1, HIGH);   //GPS Vcc on
  digitalWrite(A2, HIGH);
  digitalWrite(A3, HIGH);
  digitalWrite(A4, HIGH);
  digitalWrite(A5, HIGH);
 }
 
  void aprs::send(String latitude, String longitude, String comment, float frequency) 
  {
  GPS_VCC_off(); delay(20);
  //setup sx1278 radio in "continuous" mode and enable tx
  m_pSX1278->writeRegister(0x4B, 0x10); // TCXO input of 1278 / 0x09 = xo / 0x10 = tcxo
  m_pSX1278->setupDirect(frequency * 1000000, sx1278_frequency_correction);
  m_pSX1278->setTXDirect();
  m_pSX1278->setTxParams(sx1278_tx_power, RADIO_RAMP_DEFAULT);             
  m_pSX1278->writeRegister(REG_FDEVLSB, sx1278_deviation);        
  m_pSX1278->writeRegister(REG_PLLHOP, 0xAD);
  //m_pSX1278->writeRegister(0x4d, 0x87); //20db PA on?  0x84=17db
  // send ax.25 flag, header, payload information (comment), fcs (crc) and flag
  send_flag(20);
  crc = 0xffff;
  send_header();
  send_payload_information(comment, latitude, longitude);
  send_crc();
  send_flag(3);
  m_pSX1278->resetDevice(); //reset sx1278 radio and disable tx
  //GPS_VCC_on();
  delay(9000); 
  //GPS_VCC_off(); 
  // setup sx1278 radio in "continuous" mode and enable tx
  m_pSX1278->writeRegister(0x4B, 0x10); // TCXO input of 1278 / 0x09 = xo / 0x10 = tcxo
  m_pSX1278->setupDirect(frequency * 1000000, sx1278_frequency_correction);
  m_pSX1278->setTXDirect();
  m_pSX1278->setTxParams(sx1278_tx_power, RADIO_RAMP_DEFAULT);             
  m_pSX1278->writeRegister(REG_FDEVLSB, sx1278_deviation);                  
  m_pSX1278->writeRegister(REG_PLLHOP, 0xAD);
  //m_pSX1278->writeRegister(0x4d, 0x87); //20db PA on?  0x84=17db
  // send ax.25 flag, header, payload information (comment), fcs (crc) and flag
  send_flag(20);
  crc = 0xffff;
  send_header();
  send_payload_information(comment, latitude, longitude);
  send_crc();
  send_flag(3);
  m_pSX1278->resetDevice(); delay(15);  // reset sx1278 radio and disable tx
  m_pSX1278->setMode(MODE_SLEEP); // Sleep SX1278 //m_pSX1278->writeRegister(0x01, 0x00);
  GPS_VCC_on(); delay(15);  // GPS on
  if (sx1278_tx_power < 20) sx1278_tx_power++; // Increase power by one each sequnce until 18db
  if (w < 5) w++;
  if (w == 5) digi = aprs_digipeater2; 
  else digi = aprs_digipeater;  // Tx WIDE2 once every 5 tx, else WIDE0
  if (w == 5) w=0;
}

void aprs::set_nada_1200(void) 
{
  digitalWrite(sx1278_radio_pwm_pin, HIGH);
  delayMicroseconds(tc1200);
  digitalWrite(sx1278_radio_pwm_pin, LOW);
  delayMicroseconds(tc1200);
}

void aprs::set_nada_2400(void) 
{
  digitalWrite(sx1278_radio_pwm_pin, HIGH);
  delayMicroseconds(tc2400);
  digitalWrite(sx1278_radio_pwm_pin, LOW);
  delayMicroseconds(tc2400);
  digitalWrite(sx1278_radio_pwm_pin, HIGH);
  delayMicroseconds(tc2400);
  digitalWrite(sx1278_radio_pwm_pin, LOW);
  delayMicroseconds(tc2400);
}

void aprs::set_nada(bool nada) 
{
  if(nada)
	set_nada_1200();
  else
	set_nada_2400();
}


void aprs::calc_crc(bool in_bit) 
{
  unsigned short xor_in;

  xor_in = crc ^ in_bit;
  crc >>= 1;
  if(xor_in & 0x01)
	crc ^= 0x8408;
}

void aprs::send_crc(void) 
{
  unsigned char crc_lo = crc ^ 0xff;
  unsigned char crc_hi = (crc >> 8) ^ 0xff;

  send_char_NRZI(crc_lo, HIGH);
  send_char_NRZI(crc_hi, HIGH);
}

void aprs::send_header(void) 
{
  char temp;

  // send destination adress
  temp = dest.length();
  for(int j=0; j<temp; j++)
    send_char_NRZI(String(dest)[j] << 1, HIGH);
  if(temp < 6) 
  {
    for(int j=0; j<(6 - temp); j++)
      send_char_NRZI(' ' << 1, HIGH);
  }
  send_char_NRZI('0' << 1, HIGH);


  // send source adress
  temp = mycall.length();
  for(int j=0; j<temp; j++)
    send_char_NRZI(String(mycall)[j] << 1, HIGH);
  if(temp < 6) 
  {
    for(int j=0; j<(6 - temp); j++)
      send_char_NRZI(' ' << 1, HIGH);
  }
 
  send_char_NRZI((myssid + '0') << 1, HIGH);


  // send digipeater adresses
  temp = digi.length();
  for(int j=0; j<temp; j++)
    send_char_NRZI(String(digi)[j] << 1, HIGH);
  if(temp < 6) 
  {
    for(int j=0; j<(6 - temp); j++)
      send_char_NRZI(' ' << 1, HIGH);
  }
  send_char_NRZI(((digissid + '0') << 1) + 1, HIGH);

  // send control field and protocol field
  send_char_NRZI(_CTRL_ID, HIGH);
  send_char_NRZI(_PID, HIGH);
}


void aprs::send_payload_information(String comment, String latitude, String longitude) 
{
  // send ! for uncompressed position packet
	send_char_NRZI(_DT_POS, HIGH);

  // send latitude
	send_string_len(latitude.c_str(), latitude.length());

  // send symbol table id
	send_char_NRZI(sym_ovl, HIGH);

  // send longitude
	send_string_len(longitude.c_str(), longitude.length());

  // send symbol code
	send_char_NRZI(sym_tab, HIGH);

  // send further payload information (comment)
	send_string_len(comment.c_str(), comment.length());
}


void aprs::send_char_NRZI(unsigned char in_byte, bool enBitStuff) 
{
  bool bits;

  for(int i = 0; i < 8; i++) 
  {
    bits = in_byte & 0x01;

    calc_crc(bits);

    if(bits)
    {
      set_nada(nada);
      bit_stuff++;

      if((enBitStuff) && (bit_stuff == 5))
      {
        nada ^= 1;
        set_nada(nada);

        bit_stuff = 0;
      }
    }
    else 
    {
      nada ^= 1;
      set_nada(nada);

      bit_stuff = 0;
    }

    in_byte >>= 1;
  }
}

void aprs::send_string_len(const char *in_string, int len) 
{
  for(int j=0; j<len; j++)
    send_char_NRZI(in_string[j], HIGH);
}

void aprs::send_flag(unsigned char flag_len) 
{
  for(int j=0; j<flag_len; j++)
    send_char_NRZI(_FLAG, LOW);
}
