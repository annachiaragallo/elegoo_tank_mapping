 #ifdef GATTLIB_LOG_BACKEND_SYSLOG
 #include <syslog.h>
 #endif
 
#include "gattlib.h"
#include "common.h"
#include <semaphore.h>
 #include <pthread.h>
 #include <stdio.h>
 #include <stdlib.h>

 //#include <assert.h>
 #include <ctype.h>
 #include <glib.h>
 //#include <string.h>
 
 /* -------------------------- SYNCHRONIZATION ----------------------------- */
 /*Declaration of thread condition variable*/
 static pthread_cond_t m_connection_terminated = PTHREAD_COND_INITIALIZER;
 
 /*declaring mutex*/
 static pthread_mutex_t m_connection_terminated_lock = PTHREAD_MUTEX_INITIALIZER;

 /*Declaration of thread condition variable for mode switching*/
 static pthread_cond_t m_changed_mode = PTHREAD_COND_INITIALIZER;

 /*declaring mutex for alternating between modes*/
 static pthread_mutex_t m_change_mode_lock = PTHREAD_MUTEX_INITIALIZER;
 
 #define BLE_SCAN_TIMEOUT   10
 
 /* --------------------- CONNECTION ARGUMENTS ---------------------------*/
 /*Stores connection's parameters, passed among pthreads*/
 static struct {
	 char *adapter_name;
	 const char* mac_address;
	 uuid_t gatt_notification_uuid;
	 uuid_t gatt_write_uuid;
	 int mode;
 } m_argument;

 int stricmp(char const *a, char const *b);
size_t gatt_attr_data_from_string(const char *str, uint8_t **data);
 void on_device_connect(gattlib_adapter_t* adapter, const char *dst, gattlib_connection_t* connection, int error, void* user_data);

  /*BLE DEVICE DISCOVERING*/
 static void ble_discovered_device(gattlib_adapter_t* adapter, const char* addr, const char* name, void *user_data) {
	 int ret;
	 int16_t rssi;
	 int * last_received_mode = &ANG_INT_MOD;
	 //printf("BLE DISC address of ANG_INT_MOD, pointed to by lastreceived: %p\n", last_received_mode);
 
	 if (stricmp(addr, m_argument.mac_address) != 0) {
		 return;
	 }
 
	 ret = gattlib_get_rssi_from_mac(adapter, addr, &rssi);
	 if (ret == 0) {
		 GATTLIB_LOG(GATTLIB_INFO, "Found bluetooth device '%s' with RSSI:%d", m_argument.mac_address, rssi);
	 } else {
		 GATTLIB_LOG(GATTLIB_INFO, "Found bluetooth device '%s'", m_argument.mac_address);
	 }
 
	 ret = gattlib_connect(adapter, addr, GATTLIB_CONNECTION_OPTIONS_NONE, on_device_connect, last_received_mode);
	 if (ret != GATTLIB_SUCCESS) {
		 GATTLIB_LOG(GATTLIB_ERROR, "Failed to connect to the bluetooth device '%s'", addr);
	 }
 }

 /*BLE_SCAN*/
 static void* ble_task(void* arg) {
	 char* addr = arg;
	 gattlib_adapter_t* adapter;
	 int ret;
 
	 ret = gattlib_adapter_open(m_argument.adapter_name, &adapter);
	 if (ret) {
		 GATTLIB_LOG(GATTLIB_ERROR, "Failed to open adapter.");
		 return NULL;
	 }
 
	 ret = gattlib_adapter_scan_enable(adapter, ble_discovered_device, BLE_SCAN_TIMEOUT, addr);
	 if (ret) {
		 GATTLIB_LOG(GATTLIB_ERROR, "Failed to scan.");
		 return NULL;
	 }
 
	 /*Wait for the device to be connected*/
	 pthread_mutex_lock(&m_connection_terminated_lock);
	 pthread_cond_wait(&m_connection_terminated, &m_connection_terminated_lock);
	 pthread_mutex_unlock(&m_connection_terminated_lock);
	 return NULL;
 }
