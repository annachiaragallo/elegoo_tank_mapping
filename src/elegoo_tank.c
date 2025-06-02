/*
 */



#include "elegoo_tank.h"

  /* ---------------------- UTILS --------------------*/
 /*SUBROUTINE OF NOTIFICATION_HANDLER*/
 /*Finds key in Json string, return size of the corresponding value*/
 size_t json_value_size(char *structure, const char *key, int * start){
	char *ptr = structure;
	char *comma;
	size_t size;
	
	ptr = strstr(ptr, key);
    if (ptr == NULL) {
			return 0;
    } else {
        ptr = strchr(ptr, ':');
        	if (ptr == NULL) {
				return 0;
        	}	
			else {
			    ptr++;

				comma = strchr(ptr, ',');
				
				if(comma == NULL){
					comma = strchr(ptr, '}');
					if(comma == NULL){
						return 0;
					}
				}
				
				size = (comma - ptr) / sizeof(char);
				*start = (int) ((ptr - structure) / sizeof(char));
				return size;
			}
		}

}