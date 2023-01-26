#ifndef _USB_UHCI_H_
#define _USB_UHCI_H_

#include <types.h>

/*
 *  Queue head, basically 
 *  structures that organize
 *  non-isochronous transfer 
 *  descriptors into queues.
 */

typedef struct
{
  volatile uint32_t head;     /* Next data object to be processed */
  volatile uint32_t element;  /* Link pointer to first data object in queue */

  /* Internal stuff */
  uint8_t active : 1;
} uhci_qh_t;

/*
 *  Transfer descriptor.
 *  Describes a transfer.
 */

typedef struct
{
  volatile uint32_t link;               /* Link pointer to another TD */
  volatile uint32_t control_status;     /* Control and status */
  volatile uint32_t token;              /* Packet header (TD token) */
  volatile uint32_t buffer;             /* Buffer for this transaction */
} uhci_td_t;

typedef struct
{
  uint32_t iobase;
  uint32_t* frame_list;
  uhci_qh_t* qh_pool;
  uhci_td_t* td_pool;
} uhci_controller_t;

void uhci_init(void);

#endif
