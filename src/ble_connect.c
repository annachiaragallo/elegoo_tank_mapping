#include "ble_connect.h"

   /*BLE_DISCOVERED_DEVICE'S SUBROUTINES*/
   /*compares const strings*/
   int stricmp(char const *a, char const *b) {
	for (;; a++, b++) {
		int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
		if (d != 0 || !*a)
			return d;
	}
}

  /*MAIN'S SUBROUTINES*/
  /*Converts input str into correct format to control BLE device, stores result in second param, returns size*/
  size_t gatt_attr_data_from_string(const char *str, uint8_t **data)
  {
	  char tmp[3];
	  size_t size, i;
  
	  size = strlen(str) / 2; /*Anna occhio; per far funzionare hai raddoppiato in gattchar, forse avrebbe più senso togliere qui?*/
	  *data = g_try_malloc0(size);
	  if (*data == NULL)
		  return 0;
  
	  tmp[2] = '\0';
	  for (i = 0; i < size; i++) {
		  memcpy(tmp, str + (i * 2), 2);
		  (*data)[i] = (uint8_t) strtol(tmp, NULL, 16);
	  }
  
	  return size;
  }
