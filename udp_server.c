#include "lwip/pbuf.h"  // Provides structures and functions for packet buffers.
#include "lwip/udp.h"   // Includes UDP-specific functionality.
#include "lwip/tcp.h"   // Used for TCP/IP stack functionality (though not required for UDP operations).

#include "stdio.h"      // Standard input/output library.
#include "udp_server.h"  // Custom header file for this server (likely contains function prototypes).

// Callback function declaration.
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

/*
1. Create UDP socket.
2. Bind the socket to the server address.
3. Wait until a datagram packet arrives from a client.
4. Process the datagram packet and send a reply to the client.
5. Repeat Step 3.
*/

// Function to initialize the UDP server.
void udpServer_init(void)
{
    // UDP Control Block structure
    struct udp_pcb *upcb;
    err_t err;

    /* 1. Create a new UDP control block */
    upcb = udp_new();

    /* 2. Bind the upcb to the local port */
    ip_addr_t myIPADDR; // Define the IP address for the server.
    IP_ADDR4(&myIPADDR, 192, 168, 8, 200); // Set IP to 192.168.8.200

    // Bind the UDP control block to the IP and port 1100.
    err = udp_bind(upcb, &myIPADDR, 1100);

    /* 3. Set a receive callback for the upcb */
    if (err == ERR_OK)
    {
        // Register the callback function for receiving data.
        udp_recv(upcb, udp_receive_callback, NULL);
    }
    else
    {
        // If binding fails, remove the UDP control block.
        udp_remove(upcb);
    }
}

/* Callback function that gets executed when the server receives data from a client.
 * Parameters:
 *  - arg: User-defined argument (not used here).
 *  - upcb: Pointer to the UDP control block.
 *  - p: Pointer to the packet buffer containing received data.
 *  - addr: Address of the remote client.
 *  - port: Port number of the remote client.
 */
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    struct pbuf *txBuf; // Packet buffer for transmitting data.

    /* Get the IP of the Client */
    char *remoteIP = ipaddr_ntoa(addr); // Convert client IP address to a readable string.

    char response[100];

    // Determine the response based on the received payload
    if (strncmp((char *)p->payload, "UDP00", 5) == 0)
    {
        sprintf(response, "Hello World\n");
    }
    else if (strncmp((char *)p->payload, "UDP01", 5) == 0)
    {
        sprintf(response, "RafalBartoszak\n");
    }
    else
    {
        sprintf(response, "ERR\n");
    }

    int len = strlen(response);

    /* Allocate a pbuf for the outgoing message from RAM */
    txBuf = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);

    /* Copy the response message into the buffer */
    pbuf_take(txBuf, response, len);

    /* Connect to the remote client */
    udp_connect(upcb, addr, port);

    /* Send a reply to the client */
    udp_send(upcb, txBuf);

    /* Disconnect the UDP connection to allow new clients */
    udp_disconnect(upcb);

    /* Free the transmit buffer */
    pbuf_free(txBuf);

    /* Free the receive buffer */
    pbuf_free(p);
}
