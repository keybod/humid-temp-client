
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

#define CS_MCP3208  6        // BCM_GPIO 25

#define SPI_CHANNEL 0
#define SPI_SPEED   1000000  // 1MHz

#define INPUT_VOLTAGE  3.3
#define MAX_ADC 4096


int read_mcp3208_adc(unsigned char adcChannel)
{
  unsigned char buff[3];
  int adcValue = 0;

  buff[0] = 0x06 | ((adcChannel & 0x07) >> 2);
  buff[1] = ((adcChannel & 0x07) << 6);
  buff[2] = 0x00;

  digitalWrite(CS_MCP3208, 0);  // Low : CS Active

  wiringPiSPIDataRW(SPI_CHANNEL, buff, 3);

  buff[1] = 0x0F & buff[1];
  adcValue = ( buff[1] << 8) | buff[2];

  digitalWrite(CS_MCP3208, 1);  // High : CS Inactive

  return adcValue;
}


int main (void)
{
  int adcChannel = 0;
  int adcValue   = 0;
  int i = 0;
  int temperature_dc = 0;
  int humidity_dc = 0;
  double temperature_volt = 0.0, humidity_volt = 0.0;
  double temperature = 0.0, humidity = 0.0;

  if(wiringPiSetup() == -1)
  {
    fprintf (stdout, "Unable to start wiringPi: %s\n", strerror(errno));
    return 1 ;
  }

  if(wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED) == -1)
  {
    fprintf (stdout, "wiringPiSPISetup Failed: %s\n", strerror(errno));
    return 1 ;
  }

  pinMode(CS_MCP3208, OUTPUT);

  //get timestamp
  time_t now;
  now = time(NULL);

  // read temperature
  temperature_dc = read_mcp3208_adc(0); 
  // read humidity
  humidity_dc = read_mcp3208_adc(1);

  temperature_volt = temperature_dc * INPUT_VOLTAGE / MAX_ADC;
  humidity_volt = humidity_dc * INPUT_VOLTAGE / MAX_ADC;

  temperature = temperature_volt / (INPUT_VOLTAGE/165) - 40;
  humidity = humidity_volt / (INPUT_VOLTAGE/100);
  printf("{ \"timestamp\": %ld, \"temperature\": %f, \"humidity\": %f}\n", now, temperature, humidity);

  return 0;
}
