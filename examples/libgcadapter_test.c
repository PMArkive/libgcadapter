#include <stdbool.h>
#include <stdio.h>
#include <signal.h>
#include <libgcadapter.h>
#include <unistd.h>
#include <curses.h>
#include <string.h>
#include <stdlib.h>

bool recieved_sigint = false;

void sigint(int sig)
{
    recieved_sigint = true;
}

int main(int argc, char** argv)
{
    int port = 0;
    
    initscr();
    
    printw("libgcadapter Test Tool\n");
    
    if(argc >= 2 && !strcmp(argv[1], "--help"))
    {
        printw("  --help Show this text.\n");
        printw("  --port [1-4] The port to poll.\n");
        refresh();
        return 0;
    }
    
    if(argc == 3 && !strcmp(argv[1], "--port"))
    {
        port = atoi(argv[2]) - 1;
        if(port < 0 || port > 3)
        {
            printw("Invalid port specified (%i).\n", port + 1);
            refresh();
            return -1;
        }
    }
    
    gc_adapter_t* adapter;
    
    if(gc_adapter_initialize(&adapter))
    {
        signal(SIGINT, sigint);
        printw("GC adapter initialization successful.\n");
        
        gc_pad_state_t state;
        
        while(!recieved_sigint)
        {
            gc_adapter_update(adapter);
            
            if(adapter->open)
            {
                mvprintw(1, 0, "Adapter connected: %p %i %i\n", adapter->usb_context, adapter->usb_endpoint_read, adapter->usb_endpoint_write);
            }
            else
            {
                mvprintw(1, 0, "Waiting for adapter...\n");
            }
            
            gc_pad_poll(adapter, port, &state);
            printw("Port (%i):\n", port + 1);
            if(adapter->pad[port] == GC_PAD_NONE)
                printw("  Port is empty.\n");
            if(adapter->pad[port] == GC_PAD_WIRED)
                printw("  Port is plugged in with a wired controller.\n");
            if(adapter->pad[port] == GC_PAD_WIRELESS)
                printw("  Port is plugged in with a wireless controller.\n");
            printw("  Buttons:\n");
            printw("    A: %s\n", GC_PAD_BTN_DOWN(state, GC_PAD_BTN_A) ? "Down" : "Up");
            printw("    B: %s\n", GC_PAD_BTN_DOWN(state, GC_PAD_BTN_B) ? "Down" : "Up");
            printw("    X: %s\n", GC_PAD_BTN_DOWN(state, GC_PAD_BTN_X) ? "Down" : "Up");
            printw("    Y: %s\n", GC_PAD_BTN_DOWN(state, GC_PAD_BTN_Y) ? "Down" : "Up");
            printw("    Start: %s\n", GC_PAD_BTN_DOWN(state, GC_PAD_BTN_START) ? "Down" : "Up");
            printw("    L: %s\n", GC_PAD_BTN_DOWN(state, GC_PAD_BTN_L) ? "Down" : "Up");
            printw("    R: %s\n", GC_PAD_BTN_DOWN(state, GC_PAD_BTN_R) ? "Down" : "Up");
            printw("    Z: %s\n", GC_PAD_BTN_DOWN(state, GC_PAD_BTN_Z) ? "Down" : "Up");
            printw("    DPad Left: %s\n", GC_PAD_BTN_DOWN(state, GC_PAD_BTN_DPAD_LEFT) ? "Down" : "Up");
            printw("    DPad Right: %s\n", GC_PAD_BTN_DOWN(state, GC_PAD_BTN_DPAD_RIGHT) ? "Down" : "Up");
            printw("    DPad Up: %s\n", GC_PAD_BTN_DOWN(state, GC_PAD_BTN_DPAD_UP) ? "Down" : "Up");
            printw("    DPad Down: %s\n", GC_PAD_BTN_DOWN(state, GC_PAD_BTN_DPAD_DOWN) ? "Down" : "Up");
            printw("  Axises:\n");
            printw("    Stick: [%3i %3i]\n", state.stick.x, state.stick.y);
            printw("    CStick: [%3i %3i]\n", state.c_stick.x, state.c_stick.y);
            printw("    Triggers: [%3i %3i]\n", state.triggers.x, state.triggers.y);
            refresh();
            usleep(1000 * 30);
        }
        gc_adapter_free(adapter);
    }
    else
    {
        printw("GC adapter initialization failed.\n");
    }
    
    endwin();
    
    return 0;
}
