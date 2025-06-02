#include "main.h"

 static void notification_handler(const uuid_t* uuid, const uint8_t* data, size_t data_length, void* user_data) {
	 uintptr_t i;
	 char * json_string = malloc((data_length * sizeof(uint8_t)));
	 size_t mode_value_size, time_value_size, dir_value_size; //angle_value_size; anna| usa format +30, -80... so it's shorter!!
	 int start, int_mode;
	 char * mode_value_char; char * time_value_char; char * dir_value_char;
	 int * last_received_mode = (int *) user_data;
	 FILE * directions;
	 
	 printf("Notification Handler, address last red: %p, LAST_RECEIVED_MODE: %i\n", last_received_mode, *last_received_mode);
	 printf("m argument from handler: %s\n", m_argument.mac_address);
	 
	 /*Obtain Json string to convert to char*/
	 for(i = 0; i < (data_length); i++){
		json_string[i] = data[i];
	 }
	 printf("json_string: %s\n", json_string);

	 /*Find "M" key in Json string*/
	 mode_value_size = json_value_size(json_string, MODE_KEY_JSN, &start);
	 if(mode_value_size == 0){
		GATTLIB_LOG(GATTLIB_ERROR, "Can't find json key \"M\"");
	 }

	 /*Extract "M" value from string, convert it to integer*/
	 mode_value_char = malloc((mode_value_size * sizeof(char)) + 1);
	 strncpy(mode_value_char, json_string + start, mode_value_size);
	 mode_value_char[mode_value_size] = '\0';
	 int_mode = mode_value_char[0] - '0';

	
	 if(int_mode == OBS_AVOID_MODE){
		/*Find "T" key in Json string*/
		time_value_size = json_value_size(json_string, TIME_KEY_JSN, &start);
	 if(time_value_size == 0){
		GATTLIB_LOG(GATTLIB_ERROR, "Can't find json key \"M\"");
	 	}
		printf("found: T of size %li, starts at %i\n", time_value_size, start);
		/*Extract "T" value from string*/
		time_value_char = malloc((time_value_size * sizeof(char)) + 1);
		strncpy(time_value_char, json_string + start, time_value_size);
		time_value_char[time_value_size] = '\0';

		/*Write on .csv*/
		directions = fopen(DIRECTIONS_PATH, "a");
		fprintf(directions, "T;%s\n", time_value_char);
		fclose(directions);
		free(time_value_char);
	 } else {
		/*Find "T" key in Json string*/
		dir_value_size = json_value_size(json_string, DIRECTION_KEY_JSN, &start);
	 	if(dir_value_size == 0){
			GATTLIB_LOG(GATTLIB_ERROR, "Can't find json key \"M\"");
	 	}
		 printf("found: D of size %li, starts at %i\n", dir_value_size, start);
		/*Extract "D" value from string*/
		dir_value_char = malloc((dir_value_size * sizeof(char)) + 1);
		strncpy(dir_value_char, json_string + start, dir_value_size);
		dir_value_char[dir_value_size] = '\0';

		/*Write on .csv*/
		directions = fopen(DIRECTIONS_PATH, "a");
		fprintf(directions, "D;%s\n", dir_value_char);
		fclose(directions);
		free(dir_value_char);
	 }

	 m_argument.mode = int_mode;
	 if(m_argument.mode == OBS_AVOID_MODE){
		printf("M0, sono fermo\n");
	 } else if(m_argument.mode == ANG_CALC_MODE){
		printf("calcolo angolo...\n");
		g_usleep(2 * G_USEC_PER_SEC);
		printf("ok, calcolato!\n");	
	 }

	 if(int_mode != *last_received_mode){
		//printf("handler prelock\n");
		pthread_mutex_lock(&m_change_mode_lock);
		printf("MODE CHANGED: %i != %i\n", int_mode, *last_received_mode);
		if(int_mode == OBS_AVOID_MODE){
			*last_received_mode = 0;
		} else {
			*last_received_mode = 1;
		}
		//printf("handler locked\n");
		pthread_cond_signal(&m_changed_mode);
		pthread_mutex_unlock(&m_change_mode_lock);
		//printf("handler unlocked\n");
	 } else {
		printf("same mode...\n");
	 }

	 free(json_string);
	 free(mode_value_char);
	 //fclose(directions);
 }
 
 void on_device_connect(gattlib_adapter_t* adapter, const char *dst, gattlib_connection_t* connection, int error, void* user_data) {
	 int ret;
	 int * last_received_mode = (int *) user_data;
	 int i;
	 printf("ON CONNECT address of ANG_INT_MOD, pointed to by lastreceived: %p\n", last_received_mode);

	 ret = gattlib_register_notification(connection, notification_handler, last_received_mode);
	 if (ret) {
		 GATTLIB_LOG(GATTLIB_ERROR, "Fail to register notification callback.");
		 goto EXIT;
	 }
 
	 ret = gattlib_notification_start(connection, &m_argument.gatt_notification_uuid);
	 if (ret) {
		 GATTLIB_LOG(GATTLIB_ERROR, "Fail to start notification.");
		 goto EXIT;
	 }

	 for(i = 8; i >= 0; i--){
		if(*last_received_mode == ANG_CALC_MODE){
			printf("writing obstacle avoidance command...\n");
			if (elegoo_commands.obstacle_avoidance_mode != 0) {
				ret = gattlib_write_char_by_uuid(connection, &m_argument.gatt_write_uuid, elegoo_commands.obstacle_avoidance_mode, sizeof(elegoo_commands.obstacle_avoidance_mode));
				if (ret != GATTLIB_SUCCESS) {
					GATTLIB_LOG(GATTLIB_ERROR, "Fail to send obstacle avoidance command.");
				}

			}

			//printf("writing prelocked\n");
			pthread_mutex_lock(&m_change_mode_lock);
			printf("current (after writing OBS AV CMD) mode: %i\n", *last_received_mode);
			//printf("writing, locked\n");
			pthread_cond_wait(&m_changed_mode, &m_change_mode_lock);
			//printf("writing, post wait\n");
			printf("After writing OBS AV CMD, mode changed to %i\n", *last_received_mode);
			pthread_mutex_unlock(&m_change_mode_lock);
		} 
		else if (*last_received_mode == OBS_AVOID_MODE){
			printf("writing angle calculation command...\n");
			if (elegoo_commands.angle_calculation_mode != 0) {
				ret = gattlib_write_char_by_uuid(connection, &m_argument.gatt_write_uuid, elegoo_commands.angle_calculation_mode, sizeof(elegoo_commands.angle_calculation_mode));
				if (ret != GATTLIB_SUCCESS) {
					GATTLIB_LOG(GATTLIB_ERROR, "Fail to send obstacle avoidance command.");
				}

			}

			//printf("writing prelocked\n");
			pthread_mutex_lock(&m_change_mode_lock);
			printf("current (after writing ANG CALC CMD) mode: %i\n", *last_received_mode);
			///printf("writing, locked\n");
			pthread_cond_wait(&m_changed_mode, &m_change_mode_lock);
			//printf("writing, post wait\n");
			printf("After writing ANG CALC CMD mode changed to %i\n", *last_received_mode);
			pthread_mutex_unlock(&m_change_mode_lock);
		}
	}
	
	printf("----------- END OF DEMO -----------\n");

	printf("writing standby command...\n");
	if (elegoo_commands.standby_mode != 0) {
		ret = gattlib_write_char_by_uuid(connection, &m_argument.gatt_write_uuid, elegoo_commands.standby_mode, sizeof(elegoo_commands.standby_mode));
		if (ret != GATTLIB_SUCCESS) {
			GATTLIB_LOG(GATTLIB_ERROR, "Fail to send obstacle avoidance command.");
		}

	}

 EXIT:
	 gattlib_disconnect(connection, false /* wait_disconnection */); 
	 pthread_mutex_lock(&m_connection_terminated_lock);
	 pthread_cond_signal(&m_connection_terminated);
	 pthread_mutex_unlock(&m_connection_terminated_lock);
 }

 
 /*MAIN*/
 int main(int argc, char *argv[]) {
	/*------------------------------------------- BLE CONNECTION ---------------------------------------------------*/
	 int ret;
	 size_t len;
 
 #ifdef GATTLIB_LOG_BACKEND_SYSLOG
	 openlog("gattlib_notification", LOG_CONS | LOG_NDELAY | LOG_PERROR, LOG_USER);
	 setlogmask(LOG_UPTO(LOG_INFO));
 #endif
 
	 /*BLE device's data*/
	 m_argument.adapter_name = NULL;
	 m_argument.mac_address = OWL_BOT_ADDR;
	 
	 /*Custom Hex commands corresponding to Json Documents; new commands can be added from the elegoo_commands struct*/
	 len = gatt_attr_data_from_string(ANG_CALC_CMD_HEX, &elegoo_commands.angle_calculation_mode);
	 if (len == 0) {
	 	GATTLIB_LOG(GATTLIB_ERROR, "Error while decoding command \"angle_calculation_mode\"\n");
	 }
 
	 len = gatt_attr_data_from_string(OBS_AVD_CMD_HEX, &elegoo_commands.obstacle_avoidance_mode);
	 if (len == 0) {
	 	GATTLIB_LOG(GATTLIB_ERROR, "Error while decoding command \"obstacle_avoidance_mode\"\n");
	 }
	 
	 len = gatt_attr_data_from_string(STANDBY_CMD_HEX, &elegoo_commands.standby_mode);
	 if (len == 0) {
	 	GATTLIB_LOG(GATTLIB_ERROR, "Error while decoding command \"standby_mode\"\n");
	 }
	 
 
	 /*Encoding BLE device's channel of communication. UUIDs can be updated in the "Device-specific UUID" sectiom*/
	 if (gattlib_string_to_uuid(WRITE_UUID, strlen(WRITE_UUID) + 1, &m_argument.gatt_write_uuid) < 0) {
		 GATTLIB_LOG(GATTLIB_ERROR, "Error while encoding write_uuid \n");
	 }
	 if (gattlib_string_to_uuid(NOTIFICATION_UUID, strlen(NOTIFICATION_UUID) + 1,  &m_argument.gatt_notification_uuid) < 0) {
		 GATTLIB_LOG(GATTLIB_ERROR, "Error while encoding notification_uuid \n");
	 }
 
	 /*Start of phtread*/
	 ret = gattlib_mainloop(ble_task, NULL);
	 if (ret != GATTLIB_SUCCESS) {
		 GATTLIB_LOG(GATTLIB_ERROR, "Failed to create gattlib mainloop");
	 }
	 /*------------------------------------------ END OF BLE CONNECTION -----------------------------------------------------------*/

     if(create_svg()){
        printf("Could not create SVG\n");
     }

    

	 return 0;
 }