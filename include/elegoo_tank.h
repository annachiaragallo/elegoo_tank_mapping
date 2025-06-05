#include <glib.h>


/* -------------------- DEVICE-SPECIFIC PARAMETERS -----------------------*/

 /*Device's address*/
 static const char OWL_BOT_ADDR[] = "AA:BB:CC:DD:EE:FF";

 /*Values stored in "M" key of Json Docs received from BLE device*/
 #define OBS_AVOID_MODE 0
 #define ANG_CALC_MODE 1

 /*Device-specific UUID. Must not be changed unless BT module (hardware) is replaced*/
 static const char WRITE_UUID[] = "0000ffe2-0000-1000-8000-00805f9b34fb";
 static const char NOTIFICATION_UUID[] = "0000ffe1-0000-1000-8000-00805f9b34fb";

 /*Device-specific hexadecimal commands to send through bluetooth. Can be changed if .ino code is also changed accordingly*/
 static const char ANG_CALC_CMD_HEX[] = "7b224e223a3130312c224431223a317d";
 static const char OBS_AVD_CMD_HEX[] = "7b224e223a3130312c224431223a327d";
 static const char STANDBY_CMD_HEX[] = "7b224e223a3130307d";

 /*Key of values found in Json Docs from BLE device. It's necessary to know the value's type (can be either int or double)*/
 static const char MODE_KEY_JSN[] = "\"M\""; /*Key of value indicating what mode the device is currently in; 0 = obstacle avoidance, 1 = angle_calculation*/
 static const char TIME_KEY_JSN[] = "\"T\"";  /*Key of value indicating how long the bot has been walking straight*/
 static const char DIRECTION_KEY_JSN[] = "\"D\"";  /*Key of value indicating new direction taken by bot to avoid obstacle*/