;===============================================================================
; class FileInputStream
;===============================================================================

	; includes -> needs "../../include/dev.inc"
	; ----------------------------------------------------------------------

	; private static constants
	; ----------------------------------------------------------------------
	FileInputStream.MEMORY_LOCATION_TO_LOAD	equ 48768

	; public properties
	; ----------------------------------------------------------------------
	fileInputStream.file.name	db "________.___", 0
	fileInputStream.file.size	dw 0

	; private properties
	; ----------------------------------------------------------------------
	fileInputStream.index		dw 0

	; public methods
	; ----------------------------------------------------------------------

		fileInputStream.setFileName:
		; --------------------------------------------------------------
		; si = memory location of file name string
				mov	cx, 12
				mov	di, fileInputStream.file.name
			fileInputStream.setFileName.nextChar:
				mov	al, [si]
				mov	byte [di], al
				inc	si
				inc	di
				loop	fileInputStream.setFileName.nextChar
				mov	byte [di], 0
				ret

		fileInputStream.openFile:
		; --------------------------------------------------------------
		; ah = 0 - file not found
		;      1 - success
				mov	word [fileInputStream.index], 0
				mov	ax, fileInputStream.file.name
				call	os_get_file_size
				mov	word [fileInputStream.file.size], bx
				mov	cx, FileInputStream.MEMORY_LOCATION_TO_LOAD
				call	os_load_file
				jc	fileInputStream.openFile.notFound
			fileInputStream.openFile.success:
				mov	ah, 1
				ret
			fileInputStream.openFile.notFound:
				mov	ah, 0
				ret

		fileInputStream.getNextChar:
		; --------------------------------------------------------------
		; al = next ASCII char code
		; ah = 0 - end of file
		;      1 - success
				call	fileInputStream.seeNextChar
				cmp	ah, 1
				je	fileInputStream.getNextChar.success
			fileInputStream.getNextChar.endOfFile:
				ret
			fileInputStream.getNextChar.success:
				inc	word [fileInputStream.index]
				ret

		fileInputStream.seeNextChar:
		; --------------------------------------------------------------
		; description: return next char without increment index
		; al = next ASCII char code
		; ah = 0 - end of file
		;      1 - success
				mov	bx, [fileInputStream.index]
				mov	cx, [fileInputStream.file.size]
				cmp	bx, cx
				jg	fileInputStream.seeNextChar.endOfFile
				add	bx, FileInputStream.MEMORY_LOCATION_TO_LOAD
				mov	al, [bx]
			fileInputStream.seeNextChar.success:
				mov	ah, 1
				ret
			fileInputStream.seeNextChar.endOfFile:
				mov	ah, 0
				ret

;===============================================================================

