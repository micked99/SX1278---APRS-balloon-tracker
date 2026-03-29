#ifndef CONFIG_H_
#define CONFIG_H_


/*
 * aprs config
 */

 
 
  #define aprs_source_callsign "SA8BSS"
  #define aprs_source_ssid 11
  
  #define aprs_destination_callsign "APMON"
  #define aprs_digipeater "WIDE0"
  #define aprs_digipeater2 "WIDE2"
  #define aprs_digipeater_ssid 0
  #define aprs_symbol_overlay '/'
  #define aprs_symbol 'O'
  
  #define aprs_packet_delay 120      // aprs packet delay in seconds
 
  #define aprs_frequency_default     144.800  // aprs frequency default in MHz
  #define aprs_frequency_region1     144.800  // aprs frequency region1 in MHz
  #define aprs_frequency_region2     144.390  // aprs frequency region2 in MHz
  #define aprs_frequency_china       144.640  // aprs frequency china in MHz
  #define aprs_frequency_japan       144.660  // aprs frequency japan in MHz
  #define aprs_frequency_thailand    145.525  // aprs frequency thailand in MHz
  #define aprs_frequency_newzealand  144.575  // aprs frequency newzealand in MHz
  #define aprs_frequency_australia   145.175  // aprs frequency australia in MHz
 // #define aprs_frequency_uk          433.500  // aprs frequency uk ism band in MHz
  #define aprs_frequency_brazil      145.570  // aprs frequency brazil in MHz
 
  #define additional_aprs_comment "" // Additional comment


/*
 * radio  config
 */
  #define sx1278_frequency_correction 0 // frequency offset in Hz // 10ppm xo 300hz low, cold will make the freq climb
  //#define sx1278_frequency_correction -82650 // frequency offset in Hz // 144.770Mhz
  //#define sx1278_frequency_correction -52650 // frequency offset in Hz // 10ppm xo
  #define sx1278_deviation 0x32  // 48-20Khz // default 32-15Khz  // 16-10Khz // 
  #define sx1278_radio_pwm_pin 6
/*
 * gps  config
 */
  #define gps_baud_rate 9600 
  #define gps_timeout 1      // gps timeout in sec
  /*
 * serial  config
 */
  // #define enable_serial_debug
  #define serial_baud_rate 9600
  #endif /* CONFIG_H_ */

