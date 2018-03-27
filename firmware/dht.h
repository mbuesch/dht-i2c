#ifndef DHT_H_
#define DHT_H_

#include "util.h"


#define DHT_DATA_LEN	5u

typedef void (*dht_callback_t)(const uint8_t *data);

void dht_start(dht_callback_t callback);
void dht_init(void);


#endif /* DHT_H_ */
