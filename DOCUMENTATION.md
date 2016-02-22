# Functions


##### `gc_adapter_initialize(gc_adapter_t** adapter)` 


    Initializes the USB interface and creates a new adapter instance.
    
    
    `adapter` - The new adapter instance.
    
    
    **Returns** - If initialization of the adapter instance succeeded.
    
    
    Defined in `libgcadapter.h`
    
    
#### `gc_adapter_update(gc_adapter_t* adapter)`


    Updates the status of the adapter (`adapter->open` and `adapter->pad`).
    
    
    This function is responsible for claiming the USB interface to the adapter and for checking if the adapter is still open.
    
    
    `adapter` - The adapter which should be updated.
    
    
    **Returns** - Nothing.
    
    
    Defined in `libgcadapter.h`
    
    
#### `gc_pad_poll(gc_adapter_t* adapter, int port, gc_pad_state_t* state)`


    Polls the state of the pad located at `port`.
    
    
    This function will always empty out the `state`, even if the adapter is not open or no pad is plugged into the specified `port`.
    
    
    This function should ideally be called on a separate thread to maximize performance.
    
    
    `adapter` - The adapter to poll from.
    
    
    `port` - The port to poll from. This should be in the range of 0 to GC_ADAPTER_PORTS-1.
    
    
    **Returns** - Nothing.
        
    
    Defined in `libgcadapter.h`
    
    
#### `gc_pad_set_rumble(gc_adapter_t* adapter, int port, uint8_t rumble)`


    Set the rumble value of the pad located at `port`.
    
    
    `adapter` - The adapter to set the rumble value on.
    
    
    `port` - The port to set the rumble value on. This should be in the range of 0 to GC_ADAPTER_PORTS-1.
    
    
    **Returns** - Nothing.
        
    
    Defined in `libgcadapter.h`
    
    
#### `gc_adapter_free(gc_adapter_t* adapter)`


    Frees all resources used by the `adapter` and closes all open USB connections.
    
    
    `adapter` - The adapter to free.
    
    
    **Returns** - Nothing.
        
    
    Defined in `libgcadapter.h`
    

# Macros


#### `GC_PAD_BTN_DOWN(state, btn)`


    Expands to a statement which determines if the specified `btn` is held down in `state`.
        
    
    Defined in `gctypes.h`
    
    
#### `GC_PAD_RESET_STATE(state)`


    Expands to a statement which resets a state to the default values.
    
    
    The default values for a state are:
    
    
        `state.buttons` = `0`
        
        
        `state.stick.x` = `GC_PAD_STICK_CENTER`
        
        
        `state.stick.y` = `GC_PAD_STICK_CENTER`
        
        
        `state.c_stick.x` = `GC_PAD_STICK_CENTER`
        
        
        `state.c_stick.y` = `GC_PAD_STICK_CENTER`
        
        
        `state.triggers.x` = `0`
        
        
        `state.triggers.y` = `0`
        
    
    Defined in `gctypes.h`
    
    
#### `GC_PAD_STATES_EQUAL(st1, st2)`


    Expands to a statement which determines if the `st1` is equal to `st2`.
        
    
    Defined in `gctypes.h`