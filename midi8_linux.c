#include <fcntl.h>  // no idea LOL!
#include <stdio.h>  // fprintf
#include <stdlib.h> // exit
#include <unistd.h> // usleep

#include <X11/Xlib.h>             // XOpenDisplay, XKeysimToKeycode
#include <X11/extensions/XTest.h> // XTestFakeKyEvent
#include <X11/keysym.h>           // XK_*

#include <sys/soundcard.h> // also no idea

#define MIDI_NOTE_COUNT 128
KeySym midinote_to_keysym_table[MIDI_NOTE_COUNT] = {
    [48] = XK_Z, // C3
    [49] = XK_S, // C#3
    [50] = XK_X, // D3
    [51] = XK_D, // D#3
    [52] = XK_C, // E3
    [53] = XK_V, // F3
    [54] = XK_G, // F#3
    [55] = XK_B, // G3
    [56] = XK_H, // G#3
    [57] = XK_N, // A3
    [58] = XK_J, // A#3
    [59] = XK_M, // B3
    [60] = XK_Q, // C4 (Middle C)
    [61] = XK_2, // C#4
    [62] = XK_W, // D4
    [63] = XK_3, // D#4
    [64] = XK_E, // E4
    [65] = XK_R, // F4
    [66] = XK_5, // F#4
    [67] = XK_T, // G4
    [68] = XK_6, // G#4
    [69] = XK_Y, // A4
    [70] = XK_7, // A#4
    [71] = XK_U, // B4
};

KeySym midinote_to_keysym(unsigned char note);
void simulate_key_press(Display *display, KeySym key);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <MIDI device path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    Display *display = XOpenDisplay(NULL);

    const char *midi_device = argv[1];
    int midi_fd = open(midi_device, O_RDONLY);
    if (midi_fd == -1) {
        printf("Error: cannot open %s\n", midi_device);
        exit(1);
    }

    unsigned char data[3];
    while (1) {
        ssize_t bytes_read = read(midi_fd, data, sizeof(data));
        if (bytes_read < 0) {
            perror("Error reading from MIDI device");
            close(midi_fd);
            break;
        }

        unsigned char status = data[0]; // Status byte
        if ((status & 0xF0) == 0x90) {  // Note-On message
            unsigned char note = data[1];
            unsigned char velocity = data[2];

            if (velocity > 0) {
                KeySym key = midinote_to_keysym(note);
                simulate_key_press(display, key);
            }
        }
    }

    return EXIT_SUCCESS;
}

KeySym midinote_to_keysym(unsigned char note) {
    if (note >= MIDI_NOTE_COUNT) {
        return NoSymbol;
    }

    return midinote_to_keysym_table[note];
}

void simulate_key_press(Display *display, KeySym key) {
    KeySym keycode = XKeysymToKeycode(display, key);

    XTestFakeKeyEvent(display, keycode, True, 10);
    XTestFakeKeyEvent(display, keycode, False, 0);
    XFlush(display);
}
