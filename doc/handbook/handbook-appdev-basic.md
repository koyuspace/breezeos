### Navigate

**Overview**

*   [Features](#features)
*   [Example](#example)
*   [Assignment](#assignment)
*   [Memory](#memory)
*   [Keywords](#keywords)
*   [The editor](#theeditor)

**Instructions**

*   [ALERT](#alert)
*   [ASKFILE](#askfile)
*   [BREAK](#break)
*   [CALL](#call)
*   [CASE](#case)
*   [CLS](#cls)
*   [CURSOR](#cursor)
*   [CURSCHAR](#curschar)
*   [CURSCOL](#curscol)
*   [CURSPOS](#curspos)
*   [DELETE](#delete)
*   [DO](#do)
*   [ELSE](#else)
*   [END](#end)
*   [FILES](#files)
*   [FOR](#for)
*   [GETKEY](#getkey)
*   [GOSUB](#gosub)
*   [GOTO](#goto)
*   [IF](#if)
*   [INCLUDE](#include)
*   [INK](#ink)
*   [INPUT](#input)
*   [LEN](#len)
*   [LISTBOX](#listbox)
*   [LOAD](#load)
*   [MOVE](#move)
*   [NEXT](#next)
*   [NUMBER](#number)
*   [PAGE](#page)
*   [PAUSE](#pause)
*   [PEEK](#peek)
*   [POKE](#poke)
*   [PORT](#port)
*   [PRINT](#print)
*   [RAND](#rand)
*   [READ](#read)
*   [REM](#rem)
*   [RENAME](#rename)
*   [RETURN](#return)
*   [SAVE](#save)
*   [SERIAL](#serial)
*   [SIZE](#size)
*   [SOUND](#sound)
*   [STRING](#string)
*   [WAITKEY](#waitkey)

**Samples**

*   [Hex dumper](#hexdumper)
*   [MikeTron](#miketron)
*   [File uppercaser](#fileuppercaser)

**Extra**

*   [Help](#help)
*   [License](#license)

The BreezeOS BASIC App Developer Handbook
=======================================

### For version 4.5, 21 December 2014 - (C) BreezeOS Developers

This documentation file explains how to write software for BreezeOS using the BASIC interpreter built into the operating system. If you have any questions, see [the BreezeOS website](http://BreezeOS.berlios.de) for contact details and mailing list information.

Click the links on the left to navigate around this guide.

  

* * *

Overview
--------

### Features

The BreezeOS BASIC interpreter runs a simple dialect of the BASIC programming language. There are commands for taking input, handling the screen, performing nested loops, loading/saving files, and so forth. You will find a full list of the included instructions later in this document. Here are the essentials you need to know.

*   **Numeric variables** -- These are **A** to **Z**, each storing one positive integer word (ie 0 to 65535). The **R** and **S** variables have special roles with **LOAD** and **SAVE** commands, as explained in the instruction list below.
*   **String variables** -- These are **$1** to **$8**, each 128 bytes long.
*   **Arrays** -- You can use string variables as arrays via the **PEEK** and **POKE** commands. For instance, **X = & $1** places the memory location of the $1 string variable into X. You can then put data into it with eg **POKE 77 X** (put 77 into the memory location pointed to by X).
*   **Labels** -- Such as **box\_loop:** etc. Used by **GOTO** and **GOSUB**, they must have a trailing colon and be at the start of a line.
*   **Ending** -- Programs must finish with an **END** statement.

**GOSUB** calls can be nested up to ten times. **FOR** loops can be nested using different variables (eg you can have a **FOR X = 1 TO 10 ... NEXT X** loop surrounding a **FOR Y = 30 TO 50** loop). You can enter code in uppercase or lowercase -- the interpreter doesn't mind either way. However, labels are case-sensitive.

If a BreezeOS BASIC program is run from the command line, and one or more parameters was provided with the command, they are copied into the $1 string when the program starts.

  

### Example

Here's a small example program demonstrating various common features:

rem \*\*\* BreezeOS BASIC example program \*\*\*

alert "Welcome to the example!"
cls

print "Let's skip the next 'print' line..."
goto jump

print "This line will never be printed :-("

jump:
print "Righto, now enter a number:"
input x

if x > 10 then print "X is bigger than 10! Wow!"
if x < 10 then print "X is quite small actually."
if x = 10 then gosub equalsroutine

a = 7
b = a / 2
print "Variable A is seven here. Divided by two you get:"
print b
print "And the remainder of that is:"
b = a % 2
print b

print "A quick loop here..."
for c = 1 to 10
  print c
next c

print "Righto, that's the end! Bye!"
end

equalsroutine:
  print "Awesome, a perfect 10! Give me your name so I can high-five you!"
  input $1
  print "Top work, " ;
  print $1
return

This example should be mostly self-explanatory. You can see that the subroutine is indented with spaces, but that's not necessary if you don't want it. You can follow **IF ... THEN** with any other instruction. Regarding this part:

  print "Top work, " ;
  print $1

The space and semi-colon character (;) after the quoted string tells the interpreter not to print a newline after the string. So here, we print the user's name on the same line. You can do this with numerical variables as well, eg **PRINT X ;** etc.

See the Samples section at the end for more demonstration programs.

  

### Assignment

The following are valid ways to assign numeric variables in BreezeOS BASIC:

a = 10
a = b
a = b + 10
a = b + c
a = b - 10
a = b - c
a = b \* 10
a = b \* c
a = b / 10
a = b / c
a = b % 10
a = b % c

So you can use combinations of numbers and variables. Note that you can perform multiple operations in the same line:

a = b + c \* 2 / 3

But note that there is no operator precedence; the calculation is simply worked out one step at a time from left to right. For string variables:

$1 = "Hello"
$2 = $1

You can get the location of a string variable to use as an array:

x = & $3

You can get variables from the user with **INPUT** like this:

input x
input $1

  

### Memory

BreezeOS and its programs exist in a single 64K memory segment. The first 32K is occupied by the kernel and disk cache. Your BASIC program may be loaded at the start of the second 32K, ie 32768, if it is run directly from the program menu or CLI. However, if it is run from a standalone program such as the EDIT.BIN editor, its starting point will be elsewhere. Use **PROGSTART** and **RAMSTART** to find out what memory you can use:

x = PROGSTART

This assigns to the X variable the location at which the BASIC program was loaded. In other words, this is where the execution of the BASIC program by the interpreter starts. We also have:

x = RAMSTART

This is one byte on from the end of the BASIC code. Therefore this is empty memory which you can use, up until the 65536 mark. You can LOAD and SAVE files in here, PEEK and POKE around, and do what you want without it impacting the operating system or BASIC program.

To retrieve the location in RAM where the BreezeOS BASIC variables are stored, use this:

x = VARIABLES

  

### Keywords

You can do this to get the BreezeOS API version number:

x = VERSION

Or you can retrieve the lower word of the system clock like this:

x = TIMER

To get the current ink colour, use:

x = INK

  

### The editor

You can run your .BAS programs by selecting them from the program menu or typing them into the command line interface -- eg entering **example** will run **EXAMPLE.BAS**. Use the BreezeOS file manager, **FILEMAN.BIN**, to make backup copies of your work and delete old files.

BreezeOS includes a text editor, **EDIT.BIN** Which you can use to edit your BASIC programs. It lets you run your code straight from inside the editor by pressing the **F8** key. Note that if your program has an infinite loop or major bug, it may hang execution -- so it's worth hitting **F2** before running your code to save it to the disk. Hit **F5** to delete whole lines quickly, and **Esc** to exit (you'll see a reminder of these keybindings in the bottom bar of the editor).

The BreezeOS text editor only handles Unix-style text files, and curently does not support horizontal scrolling of lines, so keep them less than 80 characters! You can use the Delete and Backspace keys to erase characters, but note that only Delete can be used to remove newlines.

  

* * *

Instructions
------------

### ALERT

Show a dialog box on the screen with a string, and an OK button triggered by the Enter key. Example:

alert "File saved correctly."

$1 = "Welcome to my program"
alert $1

  

### ASKFILE

Show a dialog box on the screen with a string with a list of files on the disk, and store the selected file in the specified variable. Example:

askfile $1

  

### BREAK

Halts execution of the BASIC program and prints the line number in the BASIC file. Useful for debugging.

  

### CALL

Moves machine code execution to the specified point in RAM (using the x86 **call** instruction). The code must be terminated with a **ret** (C3 hex, 195 decimal) instruction. In this example, we simply add a **ret** instruction into RAM location 40000 and call it, which returns control straight back to the BASIC interpreter:

poke 195 40000
call 40000

  

### CASE

Changes the contents of a string to all upper-case or lower-case.

case lower $1
case upper $2

  

### CLS

Clears the screen and returns the cursor to the top-left corner of the screen. Example:

cls

  

### CURSOR

Determines whether to show the text cursor or not. Example:

cursor off
print "The cursor is off for five seconds!"
pause 50
cursor on
print "And now it's back on."

  

### CURSCHAR

Stores the character underneath the cursor location into the specified variable. Example:

move 0 0
print "Hello world"

move 0 0
curschar x

rem \*\*\* The next command will print 'H' \*\*\*
print chr x

move 1 0
curschar x

rem \*\*\* The next command will print 'e' \*\*\*
print chr x

  

### CURSCOL

Get the colour of the character under the cursor. Example:

move 20 15
curscol x

  

### CURSPOS

Get the position of the cursor. Example:

rem \*\*\* First is column, then row \*\*\*
curspos a b

  

### DELETE

Removes a file from the disk. Returns 0 in the R variable if the operation was a success, 1 if the delete operation couldn't be completed, or 2 if the file doesn't exist. Example:

delete "myfile.txt"

  

### DO

Perform a loop until a condition is met (UNTIL or WHILE). You can also set up an infinite loop with LOOP ENDLESS at the end. Example:

do
  rem \*\*\* Code goes here \*\*\*
loop until x = 10

  

### ELSE

Executes code if the previous IF condition didn't match. Example:

x = 1
if x = 1 then print "Hello"
else print "Goodbye"

  

### END

Terminates execution of the BASIC program and hands control back to the operating system.

  

### FILES

Prints a list of files that are on the disk on the screen.

  

### FOR

Begins a loop, counting upwards using a variable. The loop must be finished with a NEXT instruction and the relevant variable. Example:

for x = 1 to 10
  print "In a loop! X is " ;
  print x
next x

  

### GETKEY

Checks the keyboard buffer for a key, and if one has been pressed, places it into the specified variable.

loop:
  print "Infinite loop until m or Esc is pressed..." ;
  getkey x
  if x = 'm' then goto done
  goto loop

done:
  print "Finished loop!"

  

### GOSUB

Takes a label. It executes a subroutine, which must be finished with a RETURN instruction. You can nest GOSUB routines up to 10 times. Example:

print "About to go into a subroutine..."
gosub mylabel
print "Subroutine done!"
end

mylabel:
  print "Inside a GOSUB here!"
return

  

### GOTO

Takes a label, and jumps to that label in the code. Example:

print "Going to miss the next 'PRINT' line of code..."
goto skippy

print "This'll never be printed."

skippy:
print "And now we're back home"

  

### IF

Executes a command depending on a condition (or multiple conditions with AND). After stating the condition (eg whether one number is bigger than another, or whether two strings match) you must use THEN and follow with another instruction. Examples:

if x = 10 then print "X is 10! Woohoo"

if x = y then print "X is the same as Y"

if x = 'm' then print "X contains the letter m"

if x < y then print "Now X is less than Y"

if x > y then goto xbiggerthany

if $1 = "quit" then end

if $1 = $2 then gosub stringsmatch

  

### INCLUDE

Appends another BASIC file onto the end of the current one, so that you can call routines in it. RAMSTART is updated accordingly. Example:

include "mbpp.bas"

  

### INK

Changes the colour of printed text, using a number or variable. Example:

ink 2
print "This text is green"

  

### INPUT

Gets input from the user and stores the result into a numeric or string variable. Examples:

input x
input $1

  

### LEN

Stores the length of a string variable in a numeric variable. Example:

$1 = "Hello world"
len $1 x

  

### LISTBOX

Displays an dialog box containing a list of options, specified by a comma-separated list in the first string. The second and third strings provide help text, and the chosen option (from 1) is stored in the specified variable (or zero if the Esc key is pressed). Example:

cls

$1 = "Hex dumper,MikeTron"
$2 = "Choose a program to run,"
$3 = "Or press Esc to exit"

listbox $1 $2 $3 a

if a = 1 then goto runhex
if a = 2 then goto runmiketron

rem \*\*\* The following will be executed if Esc was pressed \*\*\*
end

  

### LOAD

Loads the specified file into RAM at the specified point. The first argument is the filename, and the second the location into which it should be loaded. If the file cannot be found or loaded, the R variable contains 1 after the instruction; otherwise it contains 0 and the S variable contains the file size. Examples:

load "example.txt" 40000
if r = 1 then goto fail
print "File size is:"
print s
end

fail:
print "File couldn't be loaded"
end

$1 = "example.txt"
x = 40000

load $1 x
if r = 1 then goto fail
print "File size is:"
print s
end

fail:
print "File couldn't be loaded"
end

  

### MOVE

Moves the cursor position on the screen. Examples:

move 20 15

x = 20
y = 15
move x y

  

### NEXT

Continues the FOR loop specified previously, and must be followed by a variable. See FOR above. Example:

next x

  

### NUMBER

Converts strings to numbers and vice versa. Examples:

number $1 a

number a $1

  

### PAUSE

Delays execution of the program by the specified 10ths of a second. This ultimately results in a BIOS call and may be slower or faster in some PC emulators. Try it on real hardware to be sure. Example:

print "Now let's wait for three seconds..."
pause 30

print "Hey, and one more, this time with a variable..."
x = 10
pause x

  

### PAGE

Switch between working and active (display) pages. Example:

page 1 0

  

### PEEK

Retrieve the byte stored in the specifed location in RAM. Examples:

peek a 40000
print "The number stored in memory location 40000 is..."
print a

x = 32768
peek a x

**Note:** you can use PEEKINT to work with words instead of bytes (up to 65536).

  

### POKE

Insert the byte (0 to 255) value into the specified location in RAM. Example:

print "Putting the number 126 into location 40000 in memory..."
poke 126 40000

print "Now doing the same, but using variables..."
x = 126
y = 40000
poke x y

**Note:** you can use POKEINT to work with words instead of bytes (up to 65536).

  

### PORT

Sends and receives bytes from the specified port. Examples:

x = 1
port out 1234 x
port out 1234 15
port in 1234 x

  

### PRINT

Displays text or the contents of a variable onto the screen. This will also move the cursor onto a new line after printing, unless the command is followed by a space and semi-colon. Example:

print "Hello, world!"

$1 = "Some text"
print $1

x = 123
print x

$2 = "Mike"
print "No newlines here, " ;
print $2

Note that for numerical variables, the PRINT command also supports two extra keywords:

x = 109
print x
print chr x
print hex x

In the first print command, the output is 109. In the second, the output is the ASCII character for 109 -- that is, 'm'. And in the third command, it shows the hexadecimal equivalent of 109.

  

### RAND

Generate a random integer number between two values (inclusive) and store it in a variable. Example:

rem \*\*\* Make X a random number between 50 and 2000 \*\*\*

rand x 50 2000

  

### READ

Read data bytes from a label, first specifying the offset and a variable into which to read. For instance, in the following example we read a small program and poke it into memory locations 50000 to 50012. We then call that location to run the program:

y = 1

for x = 50000 to 50012
  read mydata y a
  poke a x
  y = y + 1
next x

call 50000

waitkey x
end

mydata:
190 87 195 232 173 60 195 89 111 33 13 10 0

  

### REM

A remark -- ie a comment that the BASIC interpreter ignores. Example:

rem \*\*\* This routine calculates the cuteness of kittens \*\*\*

  

### RENAME

Renames one file to another. Literal strings and string variables are allowed. Afterwards, R contains 0 if the operation was a success, 1 if the file was not found, 2 if the write operation failed, or 3 if the target file already exists. Example:

$1 = "myfile.dat"
rename "oldfile.txt" $1

  

### RETURN

Switches execution back to the position of the prior GOSUB statement. See GOSUB above for more information. Example:

return

  

### SAVE

Saves data from memory to a file on the disk. The first parameter is the filename, the second the location in RAM where the data starts, and the third the number (in bytes) the amount of data to save. After execution, the R variable contains 1 if the file save operation failed (eg if it's a write-only medium), 2 if the file already exists, or 0 if it was successful. Examples:

save "myfile.txt" 40000 256

$1 = "myfile.txt"
x = 40000
y = 256
save $1 x y

  

### SERIAL

Turns on serial port 1, sends a byte, or receives a byte depending on the following command. There are two modes: 1200 baud and 9600 baud. Examples:

print "Enabling the serial port at 1200 baud..."
serial on 1200

print "Enabling the serial port at 9600 baud..."
serial on 9600

print "Sending the number 20 to the serial port, then 50..."
x = 20
serial send x
serial send 50

print "Receiving a byte from the serial port..."
serial rec x

  

### SIZE

Stores the size of the specified file in the S variable and sets R to 0, or sets R to 1 if the file doesn't exist. Example:

size "kernel.bin"
print s

  

### SOUND

Generate a note from the PC speaker at the specified frequency, for the specified length of time in 10ths of a second. Example:

print "Playing a low-middle-ish C for two seconds..."
sound 4000 20

**Note:** some quick frequencies. 4000 = C. 3600 = D. 3200 = E. 3000 = F. 2700 = G. 2400 = A. 2100 = B.

  

### STRING

Get or set bytes in a string variable, specified by an offset. Examples:

$1 = "Hello world"

rem \*\*\* 121 = ASCII for "y" character \*\*\*
b = 121

string set $1 5 b

rem \*\*\* Now $1 contains "Helly world" \*\*\*
print $1

  

### WAITKEY

Pauses program execution, awaiting a keypress, and stores the key in the specified variable. Note some special values: 1 for the Up cursor key; 2 for Down; 3 for Left; 4 for Right; 13 for Enter; 27 for Esc.

waitkey x
if x = 'm' then print "m was pressed"
if x = 4 then print "You hit the Right cursor key"

  

* * *

Samples
-------

### Hex dumper

Asks for a filename, loads the file data into the start of empty RAM, then loops over it, printing the value of each byte in hexadecimal format. Note that the **S** variable is filled with the file size after the **LOAD** instruction, so we use it as a counter (counting down) to find out when we've finished the file.

rem \*\*\* Hex dumper \*\*\*

print "Enter a filename:"
input $1

x = RAMSTART

load $1 x
if r = 1 then goto error

loop:
  peek a x
  print hex a ;
  print "  " ;
  x = x + 1
  s = s - 1
  if s = 0 then goto finish
  goto loop

finish:
print ""
end

error:
print "Could not load file!"
end

  

### MikeTron

A one-player version of the classic Tron game -- see the game help text below...

rem \*\*\* MikeTron \*\*\*

cls

print "You control a vehicle leaving a trail behind it."
print ""
print "It is always moving, and if it crosses any part"
print "of the trail or border (+ characters), the game"
print "is over. Use the Q and A keys to change the direction"
print "to up and down, and O and P for left and right."
print "See how long you can survive! Score at the end."
print ""
print "NOTE: May run at incorrect speed in emulators!"
print ""
print "Hit a key to begin..."

waitkey x


cls
cursor off


rem \*\*\* Draw border around screen \*\*\*

gosub setupscreen


rem \*\*\* Start in the middle of the screen \*\*\*

x = 40
y = 12

move x y


rem \*\*\* Movement directions: 1 - 4 = up, down, left, right \*\*\*
rem \*\*\* We start the game moving right \*\*\*

d = 4


rem \*\*\* S = score variable \*\*\*

s = 0


mainloop:
  print "+" ;

  pause 1

  getkey k

  if k = 'q' then d = 1
  if k = 'a' then d = 2
  if k = 'o' then d = 3
  if k = 'p' then d = 4

  if d = 1 then y = y - 1
  if d = 2 then y = y + 1
  if d = 3 then x = x - 1
  if d = 4 then x = x + 1

  move x y

  curschar c
  if c = '+' then goto finish

  s = s + 1
  goto mainloop


finish:
cursor on
cls

print "Your score was: " ;
print s
print "Press Esc to finish"

escloop:
  waitkey x
  if x = 27 then end
  goto escloop


setupscreen:
  move 0 0
  for x = 0 to 78
    print "+" ;
  next x

  move 0 24
  for x = 0 to 78
    print "+" ;
  next x

  move 0 0
  for y = 0 to 24
    move 0 y
    print "+" ;
  next y

  move 78 0
  for y = 0 to 24
    move 78 y
    print "+" ;
  next y

  return

  

### File uppercaser

Asks for a filename, loads the file data, converts all the lowercase characters to uppercase, asks for a new filename, and saves the changed data to that new file. Note that the **S** variable is filled with the file size after the **LOAD** instruction; we save that until the **SAVE** operation later so that we save the correct number of bytes. However, we make a copy of it into the **C** variable to use as a counter as we move through the file.

rem \*\*\* Text file uppercaser \*\*\*

print "Enter a filename:"
input $1

x = RAMSTART

load $1 x
if r = 1 then goto error

c = s

loop:
  peek a x

  if a < 97 then goto skip
  if a > 122 then goto skip

  a = a - 32
  poke a x

skip:
  x = x + 1
  c = c - 1

  if c = 0 then goto finish
  goto loop


finish:
print "Enter filename to save to:"
input $2

x = RAMSTART

save $2 x s
if r = 1 then goto error
print "File uppercased and saved!"
end

error:
print "File error!"
end

  

* * *

Extra
-----

### Help

If you have any questions about BreezeOS, or you're developing a similar OS and want to share code and ideas, go to [the BreezeOS website](http://BreezeOS.berlios.de) and join the mailing list as described. You can then email [BreezeOS-developer@lists.berlios.de](mailto:BreezeOS-developer@lists.berlios.de) to post to the list.

  

### License

BreezeOS is open source and released under a BSD-like license (see **doc/LICENSE.TXT** in the BreezeOS **.zip** file). Essentially, it means you can do anything you like with the code, including basing your own project on it, providing you retain the license file and give credit to the BreezeOS developers for their work.

  

* * *