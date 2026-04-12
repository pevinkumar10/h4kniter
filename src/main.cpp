#include <Arduino.h>

#include <display_common.h>
#include <wifi_common.h>
#include <wifi_scanner.h>

void setup()
{

	init_display();
	init_wifi();

	print_text("Scanning started",1,1,1);

	scan_wifi();
	
	print_text("Scan completed",1,1,1);
	delay(1000);

	char* buff[20];

	print_text(networks[0].ssid.c_str(),1,1,1);
}

void loop()
{
}
