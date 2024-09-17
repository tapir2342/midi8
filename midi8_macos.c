#include <stdio.h>

#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreMIDI/CoreMIDI.h>

void MyMIDIReadProc(
    const MIDIPacketList *packetList, void *readProcRefCon, void *srcConnRefCon
);
void printMidiDevices(void);
void translateAndSendKeyEvent(unsigned char note, bool keyDown);
void pressKey(CGKeyCode keyCode, bool keyDown);

int main() {
    // printMidiDevices();

    MIDIClientRef midiClient;
    MIDIClientCreate(CFSTR("MIDI Client"), NULL, NULL, &midiClient);
    MIDIPortRef inputPort;
    MIDIInputPortCreate(
        midiClient, CFSTR("Input Port"), MyMIDIReadProc, NULL, &inputPort
    );
    ItemCount sourceCount = MIDIGetNumberOfSources();

    // Connect the input port to all available MIDI sources.
    // TODO: Improvement: Let user choose which device to use.
    for (ItemCount i = 0; i < sourceCount; ++i) {
        MIDIEndpointRef source = MIDIGetSource(i);
        MIDIPortConnectSource(inputPort, source, NULL);
        printf("Connected to source %lu\n", i);
    }

    printf("Listening for MIDI input...\n");
    CFRunLoopRun();
    MIDIClientDispose(midiClient);

    return 0;
}

void MyMIDIReadProc(
    const MIDIPacketList *packetList, void *readProcRefCon, void *srcConnRefCon
) {
    // Iterate over each packet in the packet list
    const MIDIPacket *packet = &packetList->packet[0];
    for (unsigned int i = 0; i < packetList->numPackets; ++i) {
        const unsigned char *data = packet->data;
        unsigned char status = data[0]; // Status byte

        // Check if the message is a Note-On or Note-Off
        if ((status & 0xF0) == 0x90) { // Note-On message
            unsigned char note = data[1];
            unsigned char velocity = data[2];

            if (velocity > 0) {
                printf("Note-On: Note %d, Velocity %d\n", note, velocity);
                translateAndSendKeyEvent(note, true);
            } else {
                // Velocity of 0 is often used to indicate Note-Off
                printf(
                    "Note-Off (via Note-On with velocity 0): Note %d\n", note
                );
                // translateAndSendKeyEvent(note, false);
            }
        } else if ((status & 0xF0) == 0x80) { // Note-Off message
            unsigned char note = data[1];
            unsigned char velocity = data[2];
            printf("Note-Off: Note %d, Release Velocity %d\n", note, velocity);
            // translateAndSendKeyEvent(note, false);
        }

        // Advance to the next packet
        packet = MIDIPacketNext(packet);
    }
}

void translateAndSendKeyEvent(unsigned char note, bool pressed) {
    CGKeyCode keyCode = 0x00;

    switch (note) {

    // C3
    case 48:
        keyCode = kVK_ANSI_Z;
        break;

    // C#3
    case 49:
        keyCode = kVK_ANSI_S;
        break;

        // D3
    case 50:
        keyCode = kVK_ANSI_X;
        break;

        // D#3
    case 51:
        keyCode = kVK_ANSI_D;
        break;

        // E3
    case 52:
        keyCode = kVK_ANSI_C;
        break;

    // F3
    case 53:
        keyCode = kVK_ANSI_V;
        break;

    // F#3
    case 54:
        keyCode = kVK_ANSI_G;
        break;

    // G3
    case 55:
        keyCode = kVK_ANSI_B;
        break;

    // G#3
    case 56:
        keyCode = kVK_ANSI_H;
        break;

    // A3
    case 57:
        keyCode = kVK_ANSI_N;
        break;

    // A#3
    case 58:
        keyCode = kVK_ANSI_J;
        break;

    // B3
    case 59:
        keyCode = kVK_ANSI_M;
        break;

    // C4
    case 60:
        keyCode = kVK_ANSI_Q;
        break;

    // C#4
    case 61:
        keyCode = kVK_ANSI_2;
        break;

    // D4
    case 62:
        keyCode = kVK_ANSI_W;
        break;

    // D#4
    case 63:
        keyCode = kVK_ANSI_3;
        break;

    // E4
    case 64:
        keyCode = kVK_ANSI_E;
        break;

    // F4
    case 65:
        keyCode = kVK_ANSI_R;
        break;

    // F#4
    case 66:
        keyCode = kVK_ANSI_5;
        break;

    // G4
    case 67:
        keyCode = kVK_ANSI_T;
        break;

    // G#4
    case 68:
        keyCode = kVK_ANSI_6;
        break;

    // A4
    case 69:
        keyCode = kVK_ANSI_Y;
        break;

    // A#4
    case 70:
        keyCode = kVK_ANSI_7;
        break;

    case 71:
        keyCode = kVK_ANSI_U;
        break;
    }

    pressKey(keyCode, keyDown);
}

void printMidiDevices(void) {
    ItemCount deviceCount = MIDIGetNumberOfDevices();
    printf("Number of MIDI devices: %lu\n", deviceCount);
    for (ItemCount i = 0; i < deviceCount; ++i) {
        MIDIDeviceRef device = MIDIGetDevice(i);
        if (device) {
            CFStringRef deviceName = NULL;
            MIDIObjectGetStringProperty(device, kMIDIPropertyName, &deviceName);
            if (deviceName) {
                char deviceNameCStr[128];
                if (CFStringGetCString(
                        deviceName,
                        deviceNameCStr,
                        sizeof(deviceNameCStr),
                        kCFStringEncodingUTF8
                    )) {
                    printf("Device %lu: %s\n", i + 1, deviceNameCStr);
                }
                CFRelease(deviceName);
            }
        }
    }
}

void pressKey(CGKeyCode keyCode, bool keyDown) {
    CGEventRef eventDown = CGEventCreateKeyboardEvent(NULL, keyCode, true);
    CGEventRef eventUp = CGEventCreateKeyboardEvent(NULL, keyCode, false);
    CGEventPost(kCGSessionEventTap, eventDown);

    // 10 ms
    usleep(10 * 1000);

    CGEventPost(kCGSessionEventTap, eventUp);
    CFRelease(eventUp);
    CFRelease(eventDown);
}
