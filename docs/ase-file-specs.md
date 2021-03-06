Aseprite File Format (.ase/.aseprite) Specifications
====================================================

> Copyright (C) 2001-2016 by David Capello

1. [References](#1-references)
2. [Introduction](#2-introduction)
3. [Header](#3-header)
4. [Frames](#4-frames)
5. [Chunk Types](#5-chunk-types)
6. [File Format Changes](#6-file-format-changes)

1. References
-------------

ASE files use Intel (little-endian) byte order.

Data Type       | Details                          |
--------------- | -------------------------------- |
BYTE            | An 8-bit unsigned integer value
WORD            | A 16-bit unsigned integer value
SIGNED WORD     | A 16-bit signed integer value
DWORD           | A 32-bit unsigned integer value
FIXED           | A 32-bit fixed point (16.16) value
BYTE[n]         | "n" bytes.
STRING          | WORD: string length (number of bytes)
                | BYTE[length]: characters (in UTF-8)
                | The \0 character is not included.
PIXEL           | One pixel, depending on the image pixel format:
                | **RGB**: Each pixel have 4 bytes in this order Red, Green, Blue, Alpha.
                | **Grayscale**: Each pixel have 2 bytes in the order Value, Alpha.
                | **Indexed**: Each pixel uses 1 byte (the index).

2. Introduction
---------------

The format is much like FLI/FLC files, but with different magic number
and differents chunks.  Also, the color depth can be 8, 16 or 32 for
Indexed, Grayscale and RGB respectively, and images are compressed
images with zlib.  Color palettes are in FLI color chunks (it could be
type=11 or type=4). For color depths more than 8bpp, palettes are
optional.

To read the sprite:

* Read the [ASE header](#3-header)
* For each frame do (how many frames? the ASE header has that information):
  + Read the [frame header](#4-frames)
  + For each chunk in this frame (how many chunks? the frame header has that information)
    - Read the chunk (it should be layer information, a cel or a palette)

3. Header
---------

A 128-byte header (same as FLC/FLI header, but with other magic number):

Field         | Details                          |
------------- | -------------------------------- |
DWORD         | File size
WORD          | Magic number (0xA5E0)
WORD          | Frames
WORD          | Width in pixels
WORD          | Height in pixels
WORD          | Color depth (bits per pixel)
              | 32 bpp = RGBA
              | 16 bpp = Grayscale
              | 8 bpp = Indexed
DWORD         | Flags:
              |   1 = Layer opacity has valid value
WORD          | Speed (milliseconds between frame, like in FLC files) **DEPRECATED:** You should use the frame duration field from each frame header
DWORD         | Set be 0
DWORD         | Set be 0
BYTE          | Palette entry (index) which represent transparent color in all non-background layers (only for Indexed sprites).
BYTE[3]       | Ignore these bytes
WORD          | Number of colors (0 means 256 for old sprites)
BYTE          | Pixel width (pixel ratio is "pixel width/pixel height"). If this or pixel height field is zero, pixel ratio is 1:1
BYTE          | Pixel height
BYTE[92]      | For future (set to zero)

4. Frames
---------

After the header come the "frames" data. Each frame has this little
header of 16 bytes:

Field       | Details                          |
----------- | -------------------------------- |
DWORD       | Bytes in this frame
WORD        | Magic number (always 0xF1FA)
WORD        | Number of "chunks" in this frame
WORD        | Frame duration (in milliseconds)
BYTE[6]     | For future (set to zero)

Then each chunk format is:

Field       | Details                          |
----------- | -------------------------------- |
DWORD       | Chunk size
WORD        | Chunk type
BYTE[]      | Chunk data

5. Chunk Types
--------------

### Old palette chunk (0x0004)

Ignore this chunk if you find the new palette chunk (0x2019) Aseprite
v1.1 saves both chunks 0x0004 and 0x2019 just for backward
compatibility.

```
  WORD          Number of packets
  + For each packet
    BYTE        Number of palette entries to skip from the last packet (start from 0)
    BYTE        Number of colors in the packet (0 means 256)
    + For each color in the packet
      BYTE      Red (0-255)
      BYTE      Green (0-255)
      BYTE      Blue (0-255)
```

### Old palette chunk (0x0011)

Ignore this chunk if you find the new palette chunk (0x2019)

```
  WORD          Number of packets
  + For each packet
    BYTE        Number of palette entries to skip from the last packet (start from 0)
    BYTE        Number of colors in the packet (0 means 256)
    + For each color in the packet
      BYTE      Red (0-63)
      BYTE      Green (0-63)
      BYTE      Blue (0-63)
```

### Layer Chunk (0x2004)

  In the first frame should be a set of layer chunks to determine the
  entire layers layout:

```
  WORD          Flags:
                  1 = Visible
                  2 = Editable
                  4 = Lock movement
                  8 = Background
                  16 = Prefer linked cels
                  32 = The layer group should be displayed collapsed
                  64 = The layer is a reference layer
  WORD          Layer type
                  0 = Normal (image) layer
                  1 = Group
  WORD          Layer child level (see NOTE.1)
  WORD          Default layer width in pixels (ignored)
  WORD          Default layer height in pixels (ignored)
  WORD          Blend mode (always 0 for layer set)
                  Normal         = 0
                  Multiply       = 1
                  Screen         = 2
                  Overlay        = 3
                  Darken         = 4
                  Lighten        = 5
                  Color Dodge    = 6
                  Color Burn     = 7
                  Hard Light     = 8
                  Soft Light     = 9
                  Difference     = 10
                  Exclusion      = 11
                  Hue            = 12
                  Saturation     = 13
                  Color          = 14
                  Luminosity     = 15
  BYTE          Opacity
                  Note: valid only if file header flags field has bit 1 set
  BYTE[3]       For future (set to zero)
  STRING        Layer name
```

### Cel Chunk (0x2005)

  This chunk determine where to put a cel in the specified
  layer/frame.

```
  WORD          Layer index (see NOTE.2)
  SIGNED WORD   X position
  SIGNED WORD   Y position
  BYTE          Opacity level
  WORD          Cel type
  BYTE[7]       For future (set to zero)

  + For cel type = 0 (Raw Cel):

    WORD        Width in pixels
    WORD        Height in pixels
    PIXEL[]     Raw pixel data: row by row from top to bottom,
                for each scanline read pixels from left to right.

  + For cel type = 1 (Linked Cel):

    WORD        Frame position to link with

  + For cel type = 2 (Compressed Image):

    WORD        Width in pixels
    WORD        Height in pixels
    BYTE[]      Compressed "Raw Cel" data. Details about the
                compression method:
                http://www.ietf.org/rfc/rfc1951.txt
```

### Cel Extra Chunk (0x2006)

Adds extra information to the latest read cel.

```
  DWORD         Flags (set to zero)
                  1 - precise bounds are set
  FIXED         Precise X position
  FIXED         Precise Y position
  FIXED         Width of the cel in the sprite (scaled in real-time)
  FIXED         Height of the cel in the sprite
  BYTE[16]      For future use (set to zero)
```

### Mask Chunk (0x2016) DEPRECATED

```
  SIGNED WORD   X position
  SIGNED WORD   Y position
  WORD          Width
  WORD          Height
  BYTE[8]       For future (set to zero)
  STRING        Mask name
  BYTE[]        Bit map data (size = height*((width+7)/8))
                Each byte contains 8 pixels (the leftmost pixels are
                packed into the high order bits)
```

### Path Chunk (0x2017)

  Never used.

### Frame Tags Chunk (0x2018)

```
  WORD          Number of tags
  BYTE[8]       For future (set to zero)
  + For each tag
    WORD        From frame
    WORD        To frame
    BYTE        Loop animation direction
                0 - Forward
                1 - Reverse
                2 - Ping-pong
    BYTE[8]     For future (set to zero)
    BYTE[3]     RGB values of the tag color
    BYTE        Extra byte (zero)
    STRING      Tag name
```

### Palette Chunk (0x2019)

```
  DWORD         New palette size (total number of entries)
  DWORD         First color index to change
  DWORD         Last color index to change
  BYTE[8]       For future (set to zero)
  + For each palette entry in [from,to] range (to-from+1 entries)
    WORD        Entry flags:
                  1 = Has name
    BYTE        Red (0-255)
    BYTE        Green (0-255)
    BYTE        Blue (0-255)
    BYTE        Alpha (0-255)
    + If has name bit in entry flags
      STRING    Color name
```

### User Data Chunk (0x2020)

Insert this user data in the last read chunk.  E.g. If we've read a
layer, this user data belongs to that layer, if we've read a cel, it
belongs to that cel, etc.

```
  DWORD         Flags
                   1 = Has text
                   2 = Has color
  + If flags has bit 1:
    STRING        Text
  + If flags has bit 2:
    BYTE          Color Red (0-255)
    BYTE          Color Green (0-255)
    BYTE          Color Blue (0-255)
    BYTE          Color Alpha (0-255)
```

### Notes

#### NOTE.1

The child level is used to show the relationship of this layer with
the last one read, for example:

```
  Layer name and hierarchy      Child Level
  -----------------------------------------------
  - Background                  0
    `- Layer1                   1
  - Foreground                  0
    |- My set1                  1
    |  `- Layer2                2
    `- Layer3                   1
```

#### NOTE.2

The layer index is a number to identify any layer in the sprite, for
example:

```
  Layer name and hierarchy      Layer index
  -----------------------------------------------
  - Background                  0
    `- Layer1                   1
  - Foreground                  2
    |- My set1                  3
    |  `- Layer2                4
    `- Layer3                   5
```

6. File Format Changes
----------------------

1. The first change from the first release of the new .ase format,
   is the new frame duration field. This is because now each frame
   can have different duration.

   How to read both formats (old and new one)?  You should set all
   frames durations to the "speed" field read from the main ASE
   header.  Then, if you found a frame with the frame-duration
   field > 0, you should update the duration of the frame with
   that value.
