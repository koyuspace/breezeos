### Navigate

**Overview**

*   [Introduction](#introduction)
*   [Tools needed](#toolsneeded)

**Example**

*   [Source code](#sourcecode)
*   [Building](#building)
*   [Explanation](#explanation)

**System calls**

*   [Introduction](#syscallintro)
*   [Disk](#syscalldisk)
*   [Keyboard](#syscallkeyboard)
*   [Math](#syscallmath)
*   [Misc](#syscallmisc)
*   [Ports](#syscallports)
*   [Screen](#syscallscreen)
*   [String](#syscallstring)
*   [Sound](#syscallsound)
*   [BASIC](#syscallbasic)

**Extra**

*   [Help](#help)
*   [License](#license)

The BreezeOS Assembly App Developer Handbook
==========================================

### For version 4.5, 21 December 2014 - (C) BreezeOS Developers

This documentation file explains how write software for BreezeOS in assembly language. It shows you the tools you need, how BreezeOS programs work, and how to use the system calls included in the kernel. If you have any questions, see [the BreezeOS website](http://BreezeOS.berlios.de) for contact details and mailing list information.

Click the links on the left to navigate around this guide.

  

* * *

Overview
--------

### Introduction

Many BreezeOS programs are written in 16-bit, real mode assembly language. (The OS also includes a BASIC interpreter.) Because BreezeOS and its programs live in a single 64K memory segment, you do not need to concern yourself with segment registers.

BreezeOS programs are loaded at the **32K point** (32768) in the segment, and have a maximum size of 32K. Consequently, your programs will need to begin with these directives:

BITS 16
ORG 32768

There are many system calls available in BreezeOS for controlling the screen, getting input, manipulating strings, loading/saving files, and more. All parameters to BreezeOS system calls are passed in registers, and not on the stack. To use them in your programs, you need this line:

%INCLUDE "dev.inc"

This doesn't include any code, but a list of **equ** directives that point to system call vectors in the kernel. So, by including this file you can call, for instance, the BreezeOS **os\_print\_string** routine without having to know exactly where it is in the kernel. **dev.inc** is included in the **programs/** directory of the BreezeOS download -- it also provides a quick reference to the system calls.

If a BreezeOS program is run from the command line, and one or more parameters was provided with the command, they are passed to the program via the SI register as a zero-terminated string.

  

### Tools needed

To write BreezeOS programs you need:

*   **NASM** -- A powerful, free and open source assembler
*   **dev.inc** -- The system call vectors described above
*   A way to add programs to the floppy disk

Let's clarify this: BreezeOS uses NASM for its programs and kernel source code, hence why we recommend it here. You can, of course, use any other assembler that can output plain binary files (ie with no header) and accept 16-bit code. NASM is available in most Linux distro repositories, or you can download the Windows version [from here](http://www.nasm.us/pub/nasm/releasebuilds/) (get the 'win32' file).

For the second point, copy **programs/dev.inc** so that it's alongside your program's source code for inclusion.

For the third point, if you've written BreezeOS to a real floppy disk, you can just copy your **.BIN** programs onto that disk (root directory only!), boot BreezeOS and test them out. If you're working with the virtual **BreezeOS.flp** disk image, see the _Copying files_ section of the User Handbook.

  

* * *

Example
-------

### Source code

Here is an example BreezeOS program, in NASM format, which prints a string to the screen and waits for the user to press a key before finishing:

	BITS 16
	ORG 32768
	%INCLUDE 'dev.inc'

start:
	mov si, mystring
	call os\_print\_string

	call os\_wait\_for\_key

	ret

	mystring db 'My first BreezeOS program!', 0

  

### Building

Save the above code as **testapp.asm**, and enter this command to assemble it (works on both Linux and Windows):

nasm -f bin -o testapp.bin testapp.asm

Using the '-f bin' option we tell NASM that we just want a plain binary file: no header or sections. The resulting executable file is **testapp.bin** that we can copy to our floppy disk or add to the virtual disk image as described in _Copying files_ in the User Handbook. Then we can boot BreezeOS and run the program.

  

### Explanation

This is a very short program, but we'll explain exactly how it works for complete clarity. The first three lines will not be assembled to machine code instructions, but are just directives to tell NASM that we want to operate in 16 bit mode, our code is written to be executed from the 32K mark, and we want to use system calls from the BreezeOS API.

Next we have the 'start:' label, which is not essential but good practice to make things clear. We put the location of a zero-terminated string into the **SI** register, then call the BreezeOS **os\_print\_string** routine which we can access via the vectors listed in **dev.inc**. After that we pause program until a the user presses a key.

All BreezeOS programs must end with a **ret** instruction, which passes control back to the OS. After that instruction we have the data for our string. And that's it!

  

* * *

System calls
------------

### Introduction

The BreezeOS kernel includes system calls for outputting to the screen, taking keyboard input, manipulating strings, producing PC speaker sounds, math operations and disk I/O. You can get a quick reference to the registers that these calls take and pass back by looking at **dev.inc**, and see practical use of the calls in the **programs/** directory.

Here we have a detailed API explanation with examples. You can see the full source code behind these system calls in the **source/features/** directory in BreezeOS. Each aspect of the API below is contained within an assembly source file, so you can enhance the system calls as per the BreezeOS System Developer Handbook.

  

* * *

Disk
----

### os\_get\_file\_list

**Generate comma-separated string of files on floppy**

*   IN/OUT: AX = location to store zero-terminated filename string

Example:

	mov ax, .filestring
	call os\_get\_file\_list

	; Now .filestring will contain something like
	; 'HELLO.BIN,FOO.BAR,THREE.TXT', 0

	.filestring	times 256 db 0

  

### os\_load\_file

**Load file into RAM**

*   IN: AX = location of zero-terminated filename string, CX = location in RAM to load file
*   OUT: BX = file size (in bytes), carry set if file not found

Example:

	mov ax, filename
	mov cx, 36960		; 4K after where external program loads
	call os\_load\_file

	...

	filename db 'README.TXT', 0

  

### os\_write\_file

**Save (max 64K) file to disk**

*   IN: AX = filename, BX = data location, CX = bytes to write
*   OUT: Carry clear if OK, set if failure

Example:

	; For this example, there's some text stored in .data

	mov ax, .filename
	mov bx, .data
	mov cx, 4192
	call os\_write\_file

	.filename	db 'HELLO.TXT', 0
	.data		times 4192 db 0

  

### os\_file\_exists

**Check for presence of file on the floppy**

*   IN: AX = filename location
*   OUT: carry clear if found, set if not

Example:

	mov ax, .filename
	call os\_file\_exists
	jc .not\_found

	...

.not\_found:
	; Print error message here

	.filename	db 'HELLO.TXT', 0

  

### os\_create\_file

**Creates a new 0-byte file on the floppy disk**

*   IN: AX = location of filename
*   OUT: Nothing

Example:

	mov ax, .filename
	call os\_create\_file

	...

	.filename	db 'HELLO.TXT', 0

  

### os\_remove\_file

**Deletes the specified file from the filesystem**

*   IN: AX = location of filename to remove
*   OUT: Nothing

  

### os\_rename\_file

**Change the name of a file on the disk**

*   IN: AX = filename to change, BX = new filename (zero-terminated strings)
*   OUT: carry set on error

Example:

	mov ax, .filename1
	mov bx, .filename2
	call os\_rename\_file

	jc .error

	...

.error:
	; File couldn't be renamed (may already exist)

	.filename1	db 'ORIG.TXT', 0
	.filename2	db 'NEW.TXT', 0

  

### os\_get\_file\_size

**Get file size information for specified file**

*   IN: AX = filename
*   OUT: BX = file size in bytes (up to 64K) or carry set if file not found

  

* * *

Keyboard
--------

### os\_wait\_for\_key

**Waits for keypress and returns key**

*   IN: Nothing
*   OUT: AX = key pressed, other regs preserved

Example:

.loop:
	call os\_wait\_for\_key
	cmp al, 'y'
	je .yes
	cmp al, 'n'
	je .no
	jmp .loop

  

### os\_check\_for\_key

**Scans keyboard for input, but doesn't wait**

*   IN: Nothing
*   OUT: AX = 0 if no key pressed, otherwise scan code

Example: see code above

  

* * *

Math
----

### os\_bcd\_to\_int

**Converts binary coded decimal number to an integer**

*   IN: AL = BCD number
*   OUT: AX = integer value

Example:

	mov al, 00010110b	; 0001 0110 = 16 (decimal) or 10h in BCD
	call os\_bcd\_to\_int 
     
	; AX now contains the 16 bit-integer 00000000 00010000b

  

### os\_long\_int\_negate

**Multiply value in DX:AX by -1**

*   IN: DX:AX = long integer
*   OUT: DX:AX = -(initial DX:AX)

Example:

	mov dx, 01h
	mov ax, 45h
	call os\_long\_int\_negate

	; DX now contains 0xFFFF
	; and AX 0xFEBB

  

### os\_get\_random

**Get a random integer between high and low values (inclusive)**

*   IN: AX = low integer, BX = high
*   OUT: CX = random number between AX and BX

  

* * *

Misc
----

### os\_get\_api\_version

**Return current version of BreezeOS API**

*   IN: Nothing
*   OUT: AL = API version number

Example:

	call os\_get\_api\_version
	; AL now contains version number (eg 8)

  

### os\_pause

**Delay execution for specified 10ths of second**

*   IN: AX = number of 10ths of second to wait
*   OUT: nothing

Example:

	; Halt execution for 3 secs

	mov ax, 30
	call os\_pause

  

### os\_fatal\_error

**Display error message and halt execution**

*   IN: AX = error message string location
*   OUT: nothing

Example:

	mov ax, .error\_msg
	call os\_fatal\_error

	.error\_msg	db 'Massive error!', 0

  

* * *

Ports
-----

### os\_port\_byte\_out

**Sends a byte to the specified port**

*   IN: DX = port address, AL = byte
*   OUT: Nothing

  

### os\_port\_byte\_in

**Retrieves a byte from the specified port**

*   IN: DX = port address
*   OUT: AL = byte

  

### os\_serial\_port\_enable

**Turn on the first serial port**

*   IN: AX = 0 for normal mode (9600 baud), or 1 for slow mode (1200 baud)
*   OUT: AH = bit 7 clear on success

  

### os\_send\_via\_serial

**Send a byte via the serial port**

*   IN: AL = byte to send via serial
*   OUT: AH = bit 7 clear on success

Example:

	mov al, 'a'			; Place char to transmit in AL
	call os\_send\_via\_serial
	cmp ah, 128			; If bit 7 is set, there's an error
	jnz all\_ok			; Otherwise it's all OK
	jmp oops\_error			; Deal with the error

  

### os\_get\_via\_serial

**Get a byte from the serial port**

*   IN: nothing
*   OUT: AL = byte that was received; OUT: AH = bit 7 clear on success

Example:

	call os\_get\_via\_serial
	cmp ah, 128		; If bit 7 is set, there's an error.
	jnz all\_ok		; Otherwise it's all OK
	jmp oops\_error		; Deal with the error

all\_ok:           
	mov \[rx\_byte\], al	; Store byte we retrieved

  

* * *

Screen
------

### os\_print\_string

**Displays text**

*   IN: SI = message location (zero-terminated string)
*   OUT: Nothing (registers preserved)

Example:

	mov si, .message
	call os\_print\_string

	...

	.message	db 'Hello, world', 0

  

### os\_clear\_screen

**Clears the screen to background**

*   IN/OUT: Nothing (registers preserved)

  

### os\_move\_cursor

**Moves cursor in text mode**

*   IN: DH, DL = row, column
*   OUT: Nothing (registers preserved)

Example:

	; Move to middle of screen

	mov dh, 12
	mov dl, 40
	call os\_move\_cursor

  

### os\_get\_cursor\_pos

**Return position of text cursor**

*   OUT: DH, DL = row, column

  

### os\_print\_horiz\_line

**Draw a horizontal line on the screen**

*   IN: AX = line type (1 for double, otherwise single)
*   OUT: Nothing (registers preserved)

  

### os\_show\_cursor

**Turns on cursor in text mode**

*   IN/OUT: Nothing

  

### os\_hide\_cursor

**Turns off cursor in text mode**

*   IN/OUT: Nothing

  

### os\_draw\_block

**Render block of specified colour**

*   IN: BL/DL/DH/SI/DI = colour/start X pos/start Y pos/width/finish Y pos
*   OUT: Nothing

Example:

	mov bl, 0100111b	; White on red
	mov dl, 20		; Start X position
	mov dh, 2		; Start Y position
	mov si, 40		; Width
	mov di, 23		; Finish Y position
	call os\_draw\_block

  

### os\_file\_selector

**Show a file selection dialog**

*   IN: Nothing
*   OUT: AX = location of filename string (or carry set if Esc pressed)

Example:

	call os\_file\_selector
	jc .esc\_pressed

	; Now AX points to the chosen filename
	...

.esc\_pressed:
	...

  

### os\_list\_dialog

**Show a dialog with a list of options**

*   IN: AX = comma-separated list of strings to show (zero-terminated),
*    BX = first help string, CX = second help string
*   OUT: AX = number (starts from 1) of entry selected carry set if Esc pressed

Example:

	mov ax, .list
	mov bx, .msg1
	mov cx, .msg2
	call os\_list\_dialog
	jc .esc\_pressed

	; Now AX = number (from 1) of option chosen
	...

.esc\_pressed:
	...

	.list	db 'Open,Close,Reboot', 0
	.msg1	db 'Choose an option', 0
	.msg2	db 'Or press Esc to quit', 0

  

### os\_draw\_background

**Clear screen with white top and bottom bars, containing text, and a coloured middle section**

*   IN: AX/BX = top/bottom string locations, CX = colour
*   OUT: Nothing

Example:

	mov ax, .title\_msg
	mov bx, .footer\_msg
	mov cx, 00100000b	; Colour
	call os\_draw\_background

	...

	.title\_msg	db 'File manager', 0
	.footer\_msg	db 'Choose an option...', 0

  

### os\_print\_newline

**Reset cursor to start of next line**

*   IN/OUT: Nothing (registers preserved)

  

### os\_dump\_registers

**Displays register contents in hex on the screen**

*   IN/OUT: AX/BX/CX/DX = registers to show

  

### os\_input\_dialog

**Get text string from user via a dialog box**

*   IN: AX = string location, BX = message to show
*   OUT: AX = string location

Example:

	mov bx, .filename\_msg
	mov ax, .filename\_input
	call os\_input\_dialog

	...

	.filename\_msg	'Please enter a filename:', 0
	.filename\_input	times 12 db 0

  

### os\_dialog\_box

**Print dialog box in middle of screen, with button(s)**

*   IN: AX, BX, CX = string locations (set registers to 0 for no display),
*    DX = 0 for single 'OK' dialog, 1 for two-button 'OK' and 'Cancel'
*   OUT: If two-button mode, AX = 0 for OK and 1 for cancel
*   NOTE: Each string is limited to 40 characters

Example:

	mov ax, .string1
	mov bx, .string2
	mov cx, .string3
	mov dx, 1
	call os\_dialog\_box

	cmp ax, 1
	je .first\_option\_chosen

	; Otherwise second was chosen
	...

.first\_option\_chosen:
	...

	.string1	db 'Welcome to my program!', 0
	.string2	db 'Please choose to destroy the world,', 0
	.string3	db 'or play with fluffy kittens.', 0

  

### os\_print\_space

**Print a space to the screen**

*   IN/OUT: nothing

  

### os\_dump\_string

**Dump string as hex bytes and printable characters**

*   IN: SI = points to string to dump
*   OUT: Nothing

  

### os\_print\_digit

**Displays contents of AX as a single digit (works up to base 37, ie digits 0-Z)**

*   IN: AX = "digit" to format and print
*   OUT: Nothing

  

### os\_print\_1hex

**Displays low nibble of AL in hex format**

*   IN: AL = number to format and print
*   OUT: Nothing

  

### os\_print\_2hex

**Displays AL in hex format**

*   IN: AL = number to format and print
*   OUT: Nothing

  

### os\_print\_4hex

**Displays AX in hex format**

*   IN: AX = number to format and print
*   OUT: Nothing

  

### os\_input\_string

**Take string from keyboard entry**

*   IN/OUT: AX = location of string, other regs preserved
*   (Location will contain up to 255 characters, zero-terminated)

Example:

	mov ax, .string
	call os\_input\_string

	...

	.string		times 256 db 0

  

* * *

String
------

### os\_string\_length

**Return length of a string**

*   IN: AX = string location
*   OUT AX = length (other regs preserved)

Example:

	jmp Get\_Len

	Test\_String db 'This string has 46 characters including spaces', 0

Get\_Len:
	mov ax, Test\_String
	call os\_string\_length
	; AX now contains the number 46

  

### os\_find\_char\_in\_string

**Get the position of a character in a string**

*   IN: SI = string location, AL = character to find
*   OUT: AX = location in string, or 0 if char not present

Example:

jmp Search\_Str

     Test\_String db 'This is the test string', 0
     message\_1   db 'Character not found', 0
     message\_2   db 'Character found at position', 0
     message\_3   db '  ', 0
     str\_len     dw  0

Search\_Str:

     mov ax, Test\_String           ; string to search
     mov si, ax
     xor ax, ax                    ; clear ax

     mov al, 'x'                   ; x is the character to find
     call os\_find\_char\_in\_string

     mov \[str\_len\], ax             ; store result
     cmp ax, 0                  
     jz Char\_Not\_Found 
     jmp Char\_Found

Char\_Not\_Found:

     mov si, message\_1
     call os\_print\_string     
     jmp Main\_Pgm

Char\_Found:

     mov ax, \[str\_len\]             ; convert result into string first
     mov bx, message\_3
     call os\_int\_to\_string

     mov si, message\_2
     call os\_print\_string          ; print message\_2
     call os\_print\_space           ; print a space

     mov si, message\_3
     call os\_print\_string          ; print the position at which x was found     

  

### os\_string\_reverse

**Swar order of characters in a string**

*   IN: SI = location of zero-terminated string

Example:

	mov si, mystring
	call os\_string\_reverse

	; Now mystring contains 'olleH'

	mystring db 'Hello', 0

  

### os\_string\_charchange

**Change instances of character in a string**

*   IN: SI = string location, AL = char to find, BL = char to replace with

  

### os\_string\_uppercase

**Convert zero-terminated string to upper case**

*   IN/OUT: AX = string location

  

### os\_string\_lowercase

**Convert zero-terminated string to lower case**

*   IN/OUT: AX = string location

  

### os\_string\_copy

**Copy one string into another**

*   IN/OUT: SI = source, DI = destination (programmer ensure sufficient room)

Example:

	mov si, .old\_string
	mov di, .new\_string
	call os\_string\_copy

	...

	.old\_string	db 'Hello', 0
	.new\_string	times 16 db 0

  

### os\_string\_truncate

**Chop string down to specified number of characters**

*   IN: SI = string location, AX = number of characters
*   OUT: string modified, registers preserved

Example:

	mov si, .string
	mov ax, 3
	call os\_string\_truncate

	; .string now contains 'HEL', 0

	.string		db 'HELLO', 0

  

### os\_string\_join

**Join two strings into a third string**

*   IN/OUT: AX = string one, BX = string two, CX = destination string

Example:

	mov ax, .string1
	mov bx, .string2
	mov cx, .string3
	call os\_string\_join

	; CX now points to 'HELLOYOU', 0

	.string1	db 'HELLO', 0
	.string2	db 'YOU', 0
	.string3	times 50 db 0

  

### os\_string\_chomp

**Strip leading and trailing spaces from a string**

*   IN: AX = string location
*   OUT: nothing

Example:

	mov ax, .string
	call os\_string\_chomp

	; AX now points to 'KITTEN', 0

	.string		db '  KITTEN  ', 0

  

### os\_string\_strip

**Removes specified character from a string**

*   IN: SI = string location, AL = character to remove
*   OUT: nothing

Example:

	mov si, .string
	mov al, 'U'
	call os\_string\_strip

	; .string now contains 'MOSE', 0

	.string		db 'MOUSE', 0

  

### os\_string\_compare

**See if two strings match**

*   IN: SI = string one, DI = string two
*   OUT: carry set if same, clear if different

Example:

	mov si, .string1
	mov di, .string2
	call os\_string\_compare
	jc .same

	; Strings are different

	...

.same:
	; Strings are the same

	.string1	db 'ONE', 0
	.string2	db 'TWO', 0

  

### os\_string\_strincmp

**See if two strings match up to set number of chars**

*   IN: SI = string one, DI = string two, CL = chars to check
*   OUT: carry set if same, clear if different

Example: like above, but with CL = number of characters to check

  

### os\_string\_parse

**Take string (eg "run foo bar baz") and return pointers to zero-terminated strings (eg AX = "run", BX = "foo" etc.)**

*   IN: SI = string
*   OUT: AX, BX, CX, DX = individual strings

Example:

	mov si, .string
	call os\_string\_parse

	; Now AX points to 'This',
	; BX to 'is',
	; CX to 'a',
	; and DX to 'test'

	.string		db 'This is a test', 0

  

### os\_string\_to\_int

**Convert decimal string to integer value**

*   IN: SI = string location (max 5 chars, up to '65536')
*   OUT: AX = number

Example:

	mov si, .string
	call os\_string\_to\_int

	; Now AX contains the number 1234

	.string		db '1234', 0

  

### os\_int\_to\_string

**Convert unsigned value in AX to string**

*   IN: AX = unsigned integer
*   OUT: AX = location of internal string buffer with result

Example:

	mov ax, 1234
	call os\_int\_to\_string

	; Now AX points to '1234', 0

  

### os\_sint\_to\_string

**Convert signed value in AX to string**

*   IN: AX = signed integer
*   OUT: AX = location of internal string buffer with result

Example:

	mov ax, -1234
	call os\_int\_to\_string

	; Now AX points to '-1234', 0

  

### os\_long\_int\_to\_string

**Convert value in DX:AX to string**

*   IN: DX:AX = long unsigned integer, BX = number base, DI = string location
*   OUT: DI = location of converted string

  

### os\_set\_time\_fmt

**Set time reporting format (eg '10:25 AM' or '2300 hours')**

*   IN: AL = format flag, 0 = 12-hr format
*   OUT: nothing

  

### os\_get\_time\_string

**Get current time in a string (eg '10:25')**

*   IN/OUT: BX = string location

  

### os\_set\_date\_fmt

**Set date reporting format (M/D/Y, D/M/Y or Y/M/D - 0, 1, 2)**

*   IN: AX = format flag, 0-2
*    If AX bit 7 = 1 = use name for months
*    If AX bit 7 = 0, high byte = separator character
*   OUT: nothing

  

### os\_get\_date\_string

**Get current date in a string (eg '12/31/2007')**

*   IN/OUT: BX = string location

  

* * *

Sound
-----

### os\_speaker\_tone

**Generate PC speaker tone (call os\_speaker\_off to turn off)**

*   IN: AX = note frequency
*   OUT: Nothing (registers preserved)

Example: see code below

  

### os\_speaker\_off

**Turn off PC speaker**

*   IN/OUT: Nothing (registers preserved)

Example:

; Generate "middle C" 261.626 Hz (263 Hz close enough) for 2 secs
; 2 secs = 2,000,000 uS which is 1E8480h

	jmp Play\_It

	music\_note  dw  263

Play\_It:
	mov ax, \[music\_note\]
	call os\_speaker\_tone

	mov cx, 1Eh
	mov dx, 8480h
	call os\_pause

	call os\_speaker\_off         

  

* * *

BASIC
-----

### os\_run\_basic

**Runs kernel BASIC interpreter at the specified point**

*   IN: AX = location of BASIC code, BX = size of code (in bytes)

  

* * *

Extra
-----

### Help

If you have any questions about BreezeOS, or you're developing a similar OS and want to share code and ideas, go to [the BreezeOS website](http://BreezeOS.berlios.de) and join the mailing list as described. You can then email [BreezeOS-developer@lists.berlios.de](mailto:BreezeOS-developer@lists.berlios.de) to post to the list.

  

### License

BreezeOS is open source and released under a BSD-like license (see **doc/LICENSE.TXT** in the BreezeOS **.zip** file). Essentially, it means you can do anything you like with the code, including basing your own project on it, providing you retain the license file and give credit to the BreezeOS developers for their work.

  

* * *