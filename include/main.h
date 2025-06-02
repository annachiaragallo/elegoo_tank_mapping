 #include <glib.h>
 #include "ble_connect.h"
 #include "elegoo_tank.h"
 #include "draw.h"
 #include "common.h"

 
 /*Stores device-specific commands in hexadecimal format (gatttool-like: lowercase and no spaces)*/
 static struct {
	 uint8_t *obstacle_avoidance_mode;
	 uint8_t *angle_calculation_mode;
	 uint8_t *standby_mode;
 } elegoo_commands;

 /*------------------------------- UTILS ------------------------------------ */
size_t json_value_size(char *structure, const char *key, int * start);