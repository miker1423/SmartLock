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

		err = netbuf_copy(socket_buf, buffer, sizeof(buffer));
		if('d' == buffer[0]){
			struct netbuf *sender = netbuf_new();
			void *ptr = netbuf_alloc(sender, 12);
			err = netbuf_ref(sender, macAddress, 12);
			err = netconn_sendto(socket, sender, &socket_buf->addr, socket_buf->port);
			netbuf_delete(sender);
		}
		netbuf_delete(socket_buf);
	}
}
