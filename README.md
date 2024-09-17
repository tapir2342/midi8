# midi8

Make pico8 work with a real MIDI keyboard.
Works by turning your MIDI keyboard into an actual keyboard by translating
MIDI messages into characters pico8 understands.

![](https://iiviigames.github.io/pico8-api/img/keyboard.png)

Image by: https://iiviigames.github.io/pico8-api/

# macOS

`make && ./midi8`

# Linux

Only supports X11, no Wayland.

How to run:

- Plug in MIDI device
- Figure out where the MIDI device is connected in `/dev`: For me, it's usually `/dev/midi2`.
- Pass MIDI device as program argument when running:

```sh
make
./midi8 <MIDI_DEVICE> # for example: /dev/midi2
```

If it stops working for some reason, try to turn your MIDI keyboard off and on again.

