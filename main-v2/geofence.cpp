#include <Arduino.h>
#include <avr/pgmspace.h>

#include "geofence.h"
#include "config.h" 

geofence::geofence() 
{  
  
}

geofence::~geofence() 
{  

}

const float region1_vertices_lat[] = {
    38.000,
    21.000,
    7.000,
    24.000,
    34.000,
    42.000,
    49.000,
    42.000,
    54.000,
    48.000,
    78.000,
    78.000
};

const float region1_vertices_long[] = {  
   -33.000,
   -27.000,
    88.000,
    88.000,
    71.000,
    70.000,
    86.000,
    103.000,
    122.000,
    146.000,
    178.000,
   -27.000
};

const float region2_vertices_lat[] = {
79.000,
1.000,
1.000,
79.000
};

const float region2_vertices_long[] = {  
-173.000,
-172.000,
-35.000,
-38.000
};


const float china_vertices_lat[] = {
26.000,
16.000,
23.000,
36.000,
48.000,
54.000,
42.000,
49.000,
39.000
};

const float china_vertices_long[] = {  
90.000,
111.000,
126.000,
126.000,
134.000,
122.000,
105.000,
87.000,
71.000
};

const float japan_vertices_lat[] = {
  49.000,
  44.000,
	28.000,
	27.000,
	50.000
};

const float japan_vertices_long[] = {  
  141.000,
  134.000,
	123.000,
	146.000,
	162.000
};

const float thailand_vertices_lat[] = {
    7.000,
    6.000,
    7.000,
    17.000,
    21.000,
    24.000,
    28.000,
    27.000,
    24.000
};

const float thailand_vertices_long[] = {  
    97.000,
    105.000,
    113.000,
    111.000,
    108.000,
    102.000,
    97.000,
    92.000,
    88.000
};

const float newzealand_vertices_lat[] = {
    -30.000,
    -52.000,
    -52.000,
    -30.000,
};

const float newzealand_vertices_long[] = {  
    161.000,
    161.000,
    186.000,
    186.000,
};

const float australia_vertices_lat[] = {
    -9.000,
    -45.000,
    -45.000,
    -9.000
};

const float australia_vertices_long[] = {  
    105.000,
    105.000,
    160.000,
    160.000
};
/*
const float uk_vertices_lat[] = {
59.000,
54.000,
50.000,
50.000,
52.000,
56.000,
57.000,
58.000
};

const float uk_vertices_long[] = {  
-7.000,
-8.000,
-5.000,
1.000,
2.000,
-2.000,
-1.000,
-3.000
};
*/
const float brazil_vertices_lat[] = {
      6.000,
    -33.000,
    -33.000,
      6.000
};

const float brazil_vertices_long[] = {  
    -76.000,
    -76.000,
    -30.000,
    -30.000
};

// check if point is in geographic region - latitude and longitude in deg
// function based on example from W. Randolph Franklin - https://wrf.ecse.rpi.edu/Research/Short_Notes/pnpoly.html
bool geofence::check_if_point_is_in_geographic_region (int number_of_vertices, const float *vertices_latitude_degrees_list, const float *vertices_longitude_degrees_list, float test_point_latitude, float test_point_longitude)
{
  bool c = 0;
  int i, j = 0;
  for (i = 0, j = number_of_vertices-1; i < number_of_vertices; j = i++) {
    if ( ((vertices_longitude_degrees_list[i]>test_point_longitude) != (vertices_longitude_degrees_list[j]>test_point_longitude)) &&
	 (test_point_latitude < (vertices_latitude_degrees_list[j]-vertices_latitude_degrees_list[i]) * (test_point_longitude-vertices_longitude_degrees_list[i]) / (vertices_longitude_degrees_list[j]-vertices_longitude_degrees_list[i]) + vertices_latitude_degrees_list[i]) )
       c = !c;
  }
  return c;
}

// get aprs frequency depending on the region - latitude and longitude in deg
float geofence::get_aprs_frequency(float gps_latitude, float gps_longitude)
{
    // invalid gnss position
    if(gps_latitude == 0 && gps_longitude == 0) return aprs_frequency_default;

    // uk
    // if (check_if_point_is_in_geographic_region (sizeof(uk_vertices_lat)/sizeof(uk_vertices_lat[0]), uk_vertices_lat, uk_vertices_long, gps_latitude, gps_longitude)) return aprs_frequency_uk;
       
    // australia
    if (check_if_point_is_in_geographic_region (sizeof(australia_vertices_lat)/sizeof(australia_vertices_lat[0]), australia_vertices_lat, australia_vertices_long, gps_latitude, gps_longitude)) return aprs_frequency_australia;
    
    // newzealand
    if (check_if_point_is_in_geographic_region (sizeof(newzealand_vertices_lat)/sizeof(newzealand_vertices_lat[0]), newzealand_vertices_lat, newzealand_vertices_long, gps_latitude, gps_longitude)) return aprs_frequency_newzealand;
    
    // thailand
    if (check_if_point_is_in_geographic_region (sizeof(thailand_vertices_lat)/sizeof(thailand_vertices_lat[0]), thailand_vertices_lat, thailand_vertices_long, gps_latitude, gps_longitude)) return aprs_frequency_thailand;
    
    // japan
    if (check_if_point_is_in_geographic_region (sizeof(japan_vertices_lat)/sizeof(japan_vertices_lat[0]), japan_vertices_lat, japan_vertices_long, gps_latitude, gps_longitude)) return aprs_frequency_japan;
    
    // china
    if (check_if_point_is_in_geographic_region (sizeof(china_vertices_lat)/sizeof(china_vertices_lat[0]), china_vertices_lat, china_vertices_long, gps_latitude, gps_longitude)) return aprs_frequency_china;
    
    // region2
    if (check_if_point_is_in_geographic_region (sizeof(region2_vertices_lat)/sizeof(region2_vertices_lat[0]), region2_vertices_lat, region2_vertices_long, gps_latitude, gps_longitude)) return aprs_frequency_region2;
    
    // region1
    if (check_if_point_is_in_geographic_region (sizeof(region1_vertices_lat)/sizeof(region1_vertices_lat[0]), region1_vertices_lat, region1_vertices_long, gps_latitude, gps_longitude)) return aprs_frequency_region1;

    // brazil
    if (check_if_point_is_in_geographic_region (sizeof(brazil_vertices_lat)/sizeof(brazil_vertices_lat[0]), brazil_vertices_lat, brazil_vertices_long, gps_latitude, gps_longitude)) return aprs_frequency_brazil;
    
    // if no position found: transmit on default frequency
    return aprs_frequency_default;
}
