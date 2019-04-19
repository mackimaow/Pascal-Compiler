
#include "hashtable.h"


/* ----------------------------------------------------------------------------- 
 * hashpjw
 * Peter J. Weinberger's hash function 
 * Source: Aho, Sethi, and Ullman, "Compilers", Addison-Wesley, 1986 (page 436).
 */
int hashpjw( char *s )
{
	char *p; 
	unsigned h = 0, g; 
	
	for ( p = s; *p != EOS; p++ ) 
	{ 
		h = (h << 4) + (*p); 
		if ( g = h & 0xf0000000 ) 
		{ 
			h = h ^ ( g >> 24 ); 
			h = h ^ g; 
		} 
	} 
	return h % TABLE_SIZE; 
}



void putInTable(Hashtable &hashtable, char *id, int value) {
	
}

int getInTable(Hashtable &hashtable, char *id) {

}

void clearTable(Hashtable &hashtable){

}

int sizeTable(Hashtable &hashtable) {

}

void printTable(Hashtable &hashtable){

}