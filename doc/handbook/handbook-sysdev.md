### Navigate

**Overview**

*   [Introduction](#introduction)
*   [Structure](#structure)
*   [Memory map](#memorymap)
*   [Code path](#codepath)

**Building**

*   [Linux](#buildlinux)
*   [Windows](#buildwindows)
*   [Others](#buildothers)

**Modifying**

*   [Overview](#modoverview)
*   [System calls](#systemcalls)
*   [Patches](#patches)

**Extra**

*   [Help](#help)
*   [License](#license)

The BreezeOS System Developer Handbook
====================================

### For version 4.5, 21 December 2014 - (C) BreezeOS Developers

This documentation file explains how to build BreezeOS from the source code, make changes to the kernel, and add new system calls. If you have any questions, see [the BreezeOS website](http://BreezeOS.berlios.de) for contact details and mailing list information.

Click the links on the left to navigate around this guide.

  

* * *

Overview
--------

### Introduction

The BreezeOS kernel is written in 16-bit x86 real mode assembly language. This provides easy access to BIOS routines for handling the keyboard, screen and floppy drive, so that we don't need complicated drivers. Therefore most of the code is focused on actual OS aspects: loading programs, system calls and so forth.

Additionally, BreezeOS avoids the real mode segmentation complications by existing in a single 64K segment. The first 32K (0 - 32767) of RAM is reserved for the kernel; the second 32K is for external program code and memory.

  

### Structure

These are the most important files and directories in the BreezeOS zip file:

*   **source/** -- Contains the entire OS source code
*   **source/bootload/** -- Source to generate BOOTLOAD.BIN, which is added to the disk image when building
*   **source/features/** -- Components of BreezeOS such as FAT12 support, string routines, the BASIC interpreter etc.
*   **source/kernel.asm** -- The core kernel source file, which pulls in other source files
*   **programs/** -- Source code for programs added to the disk image

  

### Memory map

This is the makeup of the 64K memory segment after BreezeOS has loaded:

  

  
**0 - 24575 (hex: 0h - 5FFFh)**  
24K kernel executable code  
  
\- - - - - - - - - - - - - - -  
  
**24576 - 32767 (hex: 6000h - 7FFFh)**  
8K kernel disk operation buffer  
  

**32768 - 65535 (hex: 8000h - FFFFh**  
32K space for external programs

  

So, the first 32K is devoted to the BreezeOS kernel code and its 8K buffer for performing disk operations. After that we have another 32K of memory, this time for external programs. These are loaded at the 32K point and hence need to be ORGed to 32768 as described in the App Developer Handbook.

  

### Code path

When the PC starts up, it loads the bootblock, **BOOTLOAD.BIN**, that was inserted into the first sector (512 bytes) of the floppy disk image by the build script. It loads this at memory location 31744 (7C00h in hex) and begins executing it.

**BOOTLOAD.BIN** then scans the floppy disk for **KERNEL.BIN** and loads it at memory location **2000h:0000h**. Here, the 2000h is the segment and 0000h is the offset in that segment -- you don't need to concern yourself with this, but effectively it means that the kernel is loaded at location 131072 (128K) in the PC's RAM. (You get a complete memory location by multiplying the segment by 16 and adding the offset.)

Once the bootloader has loaded the kernel, it jumps to memory location 131072 (aka 2000h:0000h) to begin executing it. After this, for simplicity we ignore segments and just use offsets (0000h to FFFFh), thereby giving us 64K of RAM to use.

At the start of the kernel we have a series of **jmp** instructions. Why are these here? Well, the system calls are in semi-random places in the kernel executable. As BreezeOS evolves, the exact locations of these system calls shifts around; an external program can't guarantee where they are. So we have a list of vectors right at the very start of the kernel which **jmp** to these calls, so an external program can **call** these vectors and know that they'll never move!

There's a **jmp** just before these vectors to skip over them, and then the main kernel execution starts, setting up various things and offering the user a choice of a program list or command line interface.

  

* * *

Building
--------

### Linux

**Build requirements:** the NASM assembler, dosfstools package, 'mkisofs' utility and root access. We need root access because we loopback-mount the floppy disk image to insert our files.

To build BreezeOS, open a terminal and switch into the expanded BreezeOS package. Enter **sudo bash** in Ubuntu-flavoured distros, or just **su** in others, to switch to the root user. Then enter:

./build-linux.sh

This will use NASM to assemble the bootloader, kernel and supplied programs, then write the bootloader to the **BreezeOS.flp** floppy disk image in the **disk\_images/** directory. (It writes the 512-byte bootloader to the first sector of the floppy disk image to create a boot sector and set up a DOS-like filesystem.) Next, the build script loopback-mounts the **BreezeOS.flp** image onto the filesystem - in other words, mounting the image as if it was a real floppy. The script copies over the kernel (**kernel.bin**) and binaries from the **programs/** directory, before unmounting the floppy image.

With that done, the script runs the 'mkisofs' utility to generate a CD-ROM ISO image of BreezeOS, injecting the floppy image as a boot section. So we end up with two files in the **disk\_images/** directory: one for floppy disks and one for CD-Rs. You can now use them in an emulator or on a real PC as described in the Running section above.

  

### Windows

Get the latest version of NASM for Windows from [this site](http://www.nasm.us/pub/nasm/releasebuilds/) (look for the 'win32' package. Then extract the **nasm.exe** file into your Windows folder (or somewhere in the path).

The ImDisk Virtual Disk Driver is needed since Windows does not have a built-in mechanism for loopback drives. Get it from [here](http://www.ltr-data.se/files/imdiskinst.exe) (or Google it if the link is outdated). After downloading run imdiskinst.exe to install. Follow the default prompts during the install. Also get [PartCopy](http://www.osdever.net/downloads/other/pcopy02.zip) for copying the bootloader on to the disk image.

To build BreezeOS, double-click on **build-win.bat** or run it from the command line. This batch file calls NASM to do the work needed to compile BreezeOS and its applications. This script mounts the floppy disk image as if it were a real disk, using:

imdisk -a -f BreezeOS.flp -s 1440K -m B:

You can use that command outside of **build-win.bat** if you want to add files to **BreezeOS.flp**, and unmount it with:

imdisk -d -m B:

Lastly, to test in the [QEMU PC emulator](http://www.omledom.com), Extract QEMU to somewhere on your computer -- **C:\\** is best. Then enter: the following to run BreezeOS under QEMU:

qemu.exe -L . -m 4 -boot a -fda BreezeOS.flp -soundhw all -localtime

Make sure you put the proper path names in! Ask on the mailing list if you have any problems.

  

### Others

Along with the scripts for building on Linux and Windows, you'll also find scripts for Mac OS X and OpenBSD. These have not been as thoroughly tested as the others, so if you find any glitches, please let us know!

If you want to make a build script for a new platform, it needs to:

1.  Assemble the bootloader and add it to the first sector of **BreezeOS.flp**
2.  Assemble the kernel and copy it onto the floppy
3.  Assemble the add-on programs and copy them onto the floppy

So you will need some way of copying the 512 byte bootsector into a floppy image, and loopback mounting the image to copy across the kernel and programs.

  

* * *

Modifying
---------

### Overview

To test out code, the simplest approach is to create a new app in the **programs/** directory, build BreezeOS and run your program. You can then be guaranteed that your code isn't interfering with the kernel code (at least at the assembling stage!). When you're happy with it, you may want to introduce it into **source/kernel.asm** or a specific file in **source/features/**.

Note that the files in **source/features/** correspond to the system calls in **programs/dev.inc** (and detailed in the App Developer Handbook), but those source files also include internal calls that are used by the kernel and are not accessible to user programs. So use 'grep' or the search tool of your choice to find specific calls if they're not available through the API.

  

### System calls

Adding new system calls is easy and fun - it extends BreezeOS! So if you want to help out, this is the best way to start. Open up **source/features/screen.asm** in a text editor and paste in the following after the header text:

; -----------------------------------------------------------------
; os\_say\_hello -- Prints 'Hello' to the screen
; IN/OUT: Nothing

os\_say\_hello:
	pusha

	mov si, .message
	call os\_print\_string

	popa
	ret

	.message db 'Hello', 0

There we have it: a new system call that prints 'Hello' to the screen. Hardly a much-needed feature, but it's a starting point. The first three lines are comments explaining what the call does, and what registers it accepts or returns (like variable passing in high-level languages). Then we have the **os\_say\_hello:** label which indicates where the code starts, before a **pusha**.

All system calls should start with **pusha** and end with **popa** before **ret**: this stores registers on the stack at the start, and then pops them off at the end, so that we don't end up changing a bunch of registers and confusing the calling program. (If you're passing back a value, say in AX, you should store AX in a temporary word and drop it back in between the **popa** and **ret**, as seen in **os\_wait\_for\_key** in **keyboard.asm**.)

The body of our code simply places the location of our message string into the SI register, then calls another BreezeOS routine, **os\_print\_string**. You can freely call other routines from your own system call.

Once we've done this, we can access this routine throughout the kernel. But what about external programs? They have no idea where this call is in the kernel! The trick we use is **vectors** - a bunch of **jmp** instructions at the start of our kernel code, which jump to these routines. Because these vectors are at the start, they never change their position, so we always know where they are.

For instance, right now, your new system call may be at 0x9B9D in the kernel. But if you add another call before it, or someone else does, it may move to 0x9FA6 in the kernel binary. We simply don't know where it's going to be. But if we put at vector at the start of our kernel, before anything else happens, we can use that as the starting point as the vector will never move!

Open up **source/kernel.asm** and scroll down to the list of system call vectors. You can see they start from 0003h. Scroll to the bottom of the list and you'll see something like this:

	jmp os\_string\_tokenize		; 00CFh

The comment here indicates where this bit of code lies in the kernel binary. Once again, it's static, and basically says: if your program wants to call **os\_string\_tokenize**, it should call 00CFh, as this jumps to the required routine and will never change position.

Let's add a vector to our new call. Add this beneath the existing vectors:

	jmp os\_say\_hello		; 00D2h

How do we know this **jmp** is at 00D2h in the kernel binary? Well, just follow the pattern in the **jmp**s above - it's pretty easy to guess. If you're unsure, you can always use **ndisasm** to disassemble the kernel and look for the location of the final **jmp** in the list.

That's all good and well, but there's one last thing: people writing external programs don't want to call an ugly number like 00C9h when they run our routine. They want to access it by name, so we need to add a line to **dev.inc** in the **programs/** directory:

os\_say\_hello	equ	00D2h	; Prints 'Hello' to screen

Now, any program that includes **dev.inc** will be able to call our routine by name. Et voila: a brand new system call for BreezeOS!

  

### Patches

If you've made some improvements or additions to BreezeOS and wish to submit them, great! If they're small changes - such as a bugfix or minor tweak - you can paste the altered code into an email. Explain what it does and where it goes in the source code, and if it's OK, we'll include it.

If your change is larger (eg a system call) and affects various parts of the code, you're better off with a patch. On UNIX-like systems such as Linux, you can use the **diff** command-line utility to generate a list of your changes. For this, you will need the original (release) source code tree of BreezeOS, along with the tree of your modified code. For instance, you may have the original code in a directory called **BreezeOS-4.2/** and your enhanced version in **new-BreezeOS-4.0/**.

Switch to the directory beneath these, and enter:

diff -ru BreezeOS-4.2 new-BreezeOS-4.2 > patch.txt

This collates the differences between the two directories, and directs the output to the file **patch.txt**. Have a look at the file to make sure it's OK (you can see how it shows which lines have changed), and then attach the file to an email.

Please post fixes and patches to the BreezeOS mailing list (see [the website](http://BreezeOS.berlios.de)).

  

* * *

Extra
-----

### Help

If you have any questions about BreezeOS, or you're developing a similar OS and want to share code and ideas, go to [the BreezeOS website](http://BreezeOS.berlios.de) and join the mailing list as described. You can then email [BreezeOS-developer@lists.berlios.de](mailto:BreezeOS-developer@lists.berlios.de) to post to the list.

  

### License

BreezeOS is open source and released under a BSD-like license (see **doc/LICENSE.TXT** in the BreezeOS **.zip** file). Essentially, it means you can do anything you like with the code, including basing your own project on it, providing you retain the license file and give credit to the BreezeOS developers for their work.

  

* * *