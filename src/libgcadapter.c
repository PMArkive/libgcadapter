#include <libgcadapter.h>
#include <libusb-1.0/libusb.h>
#include <stdlib.h>
#include <stdio.h>

bool gc_adapter_initialize(gc_adapter_t** adapter)
{
    *adapter = malloc(sizeof(gc_adapter_t));
    (*adapter)->open = false;
    (*adapter)->usb_device_handle = 0;
    (*adapter)->usb_endpoint_read = 0;
    (*adapter)->usb_endpoint_write = 0;
    
    for(int i = 0; i < GC_ADAPTER_PORTS; i++)
    {
        (*adapter)->pad[i] = GC_PAD_NONE;
        (*adapter)->rumble[i] = 0;
    }
    
    for(int i = 0; i < GC_ADAPTER_PAYLOAD_SIZE; i++)
        (*adapter)->usb_recv_data[i] = 0;
        
    void** context = &(*adapter)->usb_context;
    
    int ret_libusb = libusb_init((libusb_context**)context);
    
    if(ret_libusb < 0)
    {
        free(*adapter);
        return false;
    }
    
    return true;
}

bool gc_is_adapter(libusb_device* device)
{
    struct libusb_device_descriptor descriptor;
    
    libusb_get_device_descriptor(device, &descriptor);
        
    if(descriptor.idVendor != GC_ADAPTER_USB_VID || descriptor.idProduct != GC_ADAPTER_USB_PID)
    {
        return false;
    }
        
    return true;
}


void gc_adapter_close_usb(gc_adapter_t* adapter)
{
    for(int i = 0; i < GC_ADAPTER_PORTS; i++)
    {
        adapter->pad[i] = GC_PAD_NONE;
        adapter->rumble[i] = 0;
    }
    adapter->usb_endpoint_read = adapter->usb_endpoint_write = 0;
#ifdef __linux__
    libusb_release_interface(adapter->usb_device_handle, 0);
    libusb_attach_kernel_driver(adapter->usb_device_handle, 0);
#endif
    libusb_close(adapter->usb_device_handle);
    adapter->open = false;
}

void gc_try_open_usb(gc_adapter_t* adapter, libusb_device* device)
{    
    int ret_libusb = libusb_open(device, (libusb_device_handle**)(&adapter->usb_device_handle));
    if (ret_libusb == 0) 
    {
        libusb_device_handle* handle = adapter->usb_device_handle;
        
#ifdef __linux__
        if(libusb_kernel_driver_active(handle, 0) != 0)
        {
            ret_libusb = libusb_detach_kernel_driver(handle, 0);
            if(ret_libusb != 0 && ret_libusb != -5)
            {
                gc_adapter_close_usb(adapter);
                return;
            }
        }
        ret_libusb = libusb_claim_interface(handle, 0);
        if(ret_libusb < 0)
        {
            gc_adapter_close_usb(adapter);
            return;
        }
        libusb_set_configuration(handle, 1);
#endif
        struct libusb_config_descriptor* config_descriptor;
        libusb_get_config_descriptor(device, 0, &config_descriptor);

        const struct libusb_interface* interface = &config_descriptor->interface[0];    
        
        for(int i = 0; i < interface->altsetting[0].bNumEndpoints; i++)
        {
            const struct libusb_endpoint_descriptor* endpoint = &interface->altsetting[0].endpoint[i];
            if((endpoint->bEndpointAddress & LIBUSB_ENDPOINT_IN) == LIBUSB_ENDPOINT_IN)
                adapter->usb_endpoint_read = endpoint->bEndpointAddress;
            if((endpoint->bEndpointAddress & LIBUSB_ENDPOINT_OUT) == LIBUSB_ENDPOINT_OUT)
                adapter->usb_endpoint_write = endpoint->bEndpointAddress;
        }
        
        libusb_free_config_descriptor(config_descriptor);
        
        if(adapter->usb_endpoint_read == 0 || adapter->usb_endpoint_write == 0)
        {
            gc_adapter_close_usb(adapter);
            return;
        }
        
        int tmp = 0;
        uint8_t send = 0x13;
        libusb_interrupt_transfer(handle, adapter->usb_endpoint_write, &send, sizeof(send), &tmp, 100);
        
        adapter->open = true;
    }
}

void gc_adapter_update(gc_adapter_t* adapter)
{
    libusb_context* context = (libusb_context*)adapter->usb_context;
    
    libusb_device** devices;
    ssize_t n_devices = libusb_get_device_list(context, &devices);
    bool found = false;
    if(n_devices > 0)
    {
        for(int i = 0; i < n_devices; i++)
        {
            if(gc_is_adapter(devices[i]))
            {
                found = true;
                if(!adapter->open)
                    gc_try_open_usb(adapter, devices[i]);
            }
        }
    }
    libusb_free_device_list(devices, 1);
    
    if(!found && adapter->open)
    {
        gc_adapter_close_usb(adapter);
    }
    
    for(int i = 0; i < GC_ADAPTER_PORTS; i++)
    {
        if(adapter->pad[i] != GC_PAD_WIRED)
            adapter->rumble[i] = 0;
    }
}

void gc_pad_poll(gc_adapter_t* adapter, int port, gc_pad_state_t* state)
{
    if(port < 0 || port >= GC_ADAPTER_PORTS)
        return;
    if(adapter->open)
    {
        int transferred;
        libusb_interrupt_transfer(adapter->usb_device_handle, adapter->usb_endpoint_read, adapter->usb_recv_data, GC_ADAPTER_PAYLOAD_SIZE, &transferred, 100);
        gc_pad_t type = adapter->usb_recv_data[1 + (9 * port)] >> 4;
        adapter->pad[port] = type;
        if(adapter->pad[port] != GC_PAD_NONE)
        {
            uint8_t buttons_p1 = adapter->usb_recv_data[1 + (9 * port) + 1];
            uint8_t buttons_p2 = adapter->usb_recv_data[1 + (9 * port) + 2];
            
            state->buttons = GC_PAD_BTN_NONE;

            if (buttons_p1 & (1 << 0)) state->buttons |= GC_PAD_BTN_A;
            if (buttons_p1 & (1 << 1)) state->buttons |= GC_PAD_BTN_B;
            if (buttons_p1 & (1 << 2)) state->buttons |= GC_PAD_BTN_X;
            if (buttons_p1 & (1 << 3)) state->buttons |= GC_PAD_BTN_Y;

            if (buttons_p1 & (1 << 4)) state->buttons |= GC_PAD_BTN_DPAD_LEFT;
            if (buttons_p1 & (1 << 5)) state->buttons |= GC_PAD_BTN_DPAD_RIGHT;
            if (buttons_p1 & (1 << 6)) state->buttons |= GC_PAD_BTN_DPAD_DOWN;
            if (buttons_p1 & (1 << 7)) state->buttons |= GC_PAD_BTN_DPAD_UP;

            if (buttons_p2 & (1 << 0)) state->buttons |= GC_PAD_BTN_START;
            if (buttons_p2 & (1 << 1)) state->buttons |= GC_PAD_BTN_Z;
            if (buttons_p2 & (1 << 2)) state->buttons |= GC_PAD_BTN_R;
            if (buttons_p2 & (1 << 3)) state->buttons |= GC_PAD_BTN_L;

            state->stick.x = adapter->usb_recv_data[1 + (9 * port) + 3];
            state->stick.y = adapter->usb_recv_data[1 + (9 * port) + 4];
            state->c_stick.x = adapter->usb_recv_data[1 + (9 * port) + 5];
            state->c_stick.y = adapter->usb_recv_data[1 + (9 * port) + 6];
            state->triggers.x = adapter->usb_recv_data[1 + (9 * port) + 7];
            state->triggers.y = adapter->usb_recv_data[1 + (9 * port) + 8];
        }
        else
        {
            GC_PAD_RESET_STATE((*state));
        }
    }
    else
    {
        GC_PAD_RESET_STATE((*state));
    }    
}

void gc_pad_set_rumble(gc_adapter_t* adapter, int port, uint8_t rumble)
{
    if(port < 0 || port >= GC_ADAPTER_PORTS)
        return;
    if(adapter->open && rumble != adapter->rumble[port] && adapter->pad[port] == GC_PAD_WIRED)
    {
        adapter->rumble[port] = rumble;
        int tmp;
        uint8_t rumble_cmd[5] = { 0x11, adapter->rumble[0], adapter->rumble[1], adapter->rumble[2], adapter->rumble[3] };
        libusb_interrupt_transfer(adapter->usb_device_handle, adapter->usb_endpoint_write, rumble_cmd, 5, &tmp, 100);
    }
}

void gc_adapter_free(gc_adapter_t* adapter)
{
    if(adapter->open)
        gc_adapter_close_usb(adapter);
    libusb_exit(adapter->usb_context);
    free(adapter);
}
