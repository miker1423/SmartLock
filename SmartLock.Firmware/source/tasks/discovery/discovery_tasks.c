/*
 * discovery_task.c
 *
 *  Created on: 7 dic. 2020
 *      Author: Miguel
 */

#include "discovery_tasks.h"

#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/sys.h"

extern char *macAddress;

void discovery_task(void *args){
	struct netconn *socket;
	struct netbuf *socket_buf;
	uint8_t buffer[1];
	err_t err;
	LWIP_UNUSED_ARG(args);

	socket = netconn_new(NETCONN_UDP);
	netconn_bind(socket, IP_ADDR_ANY, 1002);

	while(1){
		err = netconn_recv(socket, &socket_buf);
		if(ERR_OK != err) continue;

		memcpy(buffer, socket_buf->p->payload, 1);
		if('d' == buffer[0]){
			socket_buf->p->len = 6;
			memcpy(socket_buf->p->payload, macAddress, 12);
		}
		netbuf_delete(socket_buf);
	}
}
