#include <stdbool.h>
#include <gctypes.h>

#ifndef LIBGCADAPTER_H
#define LIBGCADAPTER_H

typedef enum
{
	GC_PAD_NONE = 0,
	GC_PAD_WIRED = 1,
	GC_PAD_WIRELESS = 2
}
gc_pad_t;

#define GC_ADAPTER_USB_VID 0x057e
#define GC_ADAPTER_USB_PID 0x0337
#define GC_ADAPTER_PORTS 4
#define GC_ADAPTER_PAYLOAD_SIZE 37

typedef struct
{
	bool open;
	gc_pad_t pad[GC_ADAPTER_PORTS];
	uint8_t rumble[GC_ADAPTER_PORTS];
	uint8_t usb_recv_data[GC_ADAPTER_PAYLOAD_SIZE];
	void* usb_context;
	void* usb_device_handle;
	uint8_t usb_endpoint_read;
	uint8_t usb_endpoint_write;
}
gc_adapter_t;

bool gc_adapter_initialize(gc_adapter_t** adapter);
void gc_adapter_update(gc_adapter_t* adapter);
void gc_pad_poll(gc_adapter_t* adapter, int port, gc_pad_state_t* state);
void gc_pad_set_rumble(gc_adapter_t* adapter, int port, uint8_t rumble);
void gc_adapter_free(gc_adapter_t* adapter);

#endif
