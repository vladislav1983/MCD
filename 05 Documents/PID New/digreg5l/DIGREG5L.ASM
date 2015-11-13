;********************************************************************************
; Programm DIGREG5E.asm:  Studie fuer einen PID-REGLER mit erweitertem Regelalgorithmus
; Variante mit zusaetzlichem EEPROM zum Speichern der Einstellungen und mit WDT!
; Mit LCD-Modul-Anzeige und HEX-Tastatur
; PIC 16C71, f = 4,096 MHz; ADW mit RC-Oszillator, 
; PortA: RA0 = Analogeingang,  RA2 = E, RA3 = RS des LCD, RA4 = Alarmausgang
; PortB = Ausgang zum D/A-Wandler, zum LCD; sowie Ein- und Ausgang fuer die HEX-Tastatur
; Variante nach Aenderung fuer Elektor : DAW  AD 557, englisches Display
; Dieter Kohtz, Februar 1999	
;********************************************************************************
;		LIST	p=16C71
; Registerzuweisung
IND		equ	0
RTCC		equ	1
PC		equ	2
STAT		equ	3
FSR		equ	4
PORTA		equ	5		 
PORTB		equ	6		 
ADCON0	equ	8
ADRES		equ	9
PCLATH	equ	0A
INTCON		equ	0B
BEFEHL		equ	0C		; Steuerwort des LCD
SOLLW		equ	0D		; Sollwert
STELLG		equ	0E		; Stellgroesse
ABW		equ	0F		; Regelabweichung, aktuell
ZEIT		equ	10		; Faktor fuer Abtastzeit (Basiszeit 0,1 s)
ZAEHL		equ	11
ZAEHL1		equ	12
OGRW		equ	13		; Oberer Grenzwert
UGRW		equ	14		; Unterer Grenzwert
TASTE		equ	15		; Tastenwert
ZWISCH	equ	16
ZWISCH0	equ	17		 
ZWISCH1	equ	18
AHI		equ	19		; Faktor A, high 
ALO		equ	1A		; Faktor A, low
BLO		equ	1B		; Faktor B
ACCHI		equ	1C		; Ergebnis, high 
ACCMI		equ	1D		; Ergebnis, mid
ACCLO		equ	1E		; Ergebnis, low
EEPROM	equ	1F		; fuer Datentransfer zum/vom EEPROM
DATEIN		equ	20		; wird ins EEPROM gelesen
DATAUS	equ	21		; wird aus dem EEPROM gelesen
WORTADR	equ	22		; Wortadresse im EEPROM
ADRLCD	equ	23		; Adresse im LCD
UHI		equ	24		; Zwischenspeicher f. STELLG
UMI		equ	25		;                  "
ULO		equ	26		;                  "
Q0HI		equ	27		; Parameter Q0, high
Q0LO		equ	28		; Parameter Q0, low
ABW0		equ	29		; zugeh. Abweichung
Q1HI		equ	2A		; Parameter Q1, high
Q1LO		equ	2B		; Parameter Q1, low
ABW1		equ	2C		; zugeh. Abweichung, vorige Messung
Q2HI		equ	2D		; Parameter Q2, high
Q2LO		equ	2E		; Parameter Q2, low
ABW2		equ	2F		; zugeh. Abweichung, vorvorige Messung

OPTREG		equ	1		; OPTION-Reg. Seite 1 des Registerfiles!
TRISA		equ	5		; Seite 1 des Registerfiles!
TRISB		equ	6		; Seite 1 des Registerfiles!
ADCON1	equ	8		; Seite 1 des Registerfiles!

; Konstanten
teiladrs		equ	0A0		; Bauteiladresse des EEPROM

; Ziele
w		equ	0
f		equ	1

; bit
Z		equ	2		; Zerobit in STAT
C		equ	0		; Carrybit in STAT
TO		equ	4		; WDT-Time-Out-Bit in STAT
RP0		equ	5		; Seitenbit in STAT
DI		equ	7		; Eingangsbit in EEPROM
DO		equ	6		; Ausgangsbit in EEPROM
SDA		equ	7		; PortB,7  Datenleitung des EEPROM
SCL		equ	6		; PortB,6  Taktleitung des EEPROM
spalte1		equ	4		; Spaltenbits fuer HEX-Tastatur
spalte2		equ	5		;                       "
spalte3		equ	6		;                       "
spalte4		equ	7		;                       "


start		nop
		goto	init
;********************************************************************************
; Tabelle der ASCII-Zeichen fuer sämtliche Textausgaben
;********************************************************************************
tab		addwf	PC,f			; (w) zum Programmzaehler addieren
		retlw	53			; S  	Text 1: SETVALUE: (Pos. 0)
		retlw	45			; E
		retlw	54			; T
		retlw	56			; V
		retlw	41			; A
		retlw	4C			; L
		retlw	55			; U
		retlw	45			; E
		retlw	3A			; : Doppelpunkt
		retlw	20			; leer
		retlw	20			; leer

		retlw	4D			; M 	Text 2: MAN.VAR.: (Pos. 0B)
		retlw	41			; A
		retlw	4E			; N
		retlw	2E			; .
		retlw	56			; V
		retlw	41			; A		
		retlw	52			; R
		retlw	2E			; . 
		retlw	3A			; : Doppelpunkt
		retlw	20			; leer
		retlw	20			; leer

		retlw	55			; U 	Text 3: OBERGRENZE: (Pos. 16)
		retlw	50			; P			
		retlw	50			; P
		retlw	45			; E
		retlw	52			; R
		retlw	20			; leer
		retlw	4C			; L
		retlw	49			; I
		retlw	4D			; M
		retlw	49			; I
		retlw	54			; T 
		retlw	3A			; Doppelpunkt

		retlw	4C			; L	 Text 4: UNT-GRENZE: (Pos. 22)
		retlw	4F			; O			
		retlw	57			; W
		retlw	45			; E
		retlw	52			; R
		retlw	20			; leer
		retlw	4C			; L
		retlw	49			; I
		retlw	4D			; M
		retlw	49			; I
		retlw	54			; T
		retlw	3A			; : Doppelpunkt

		retlw	49			; I 	 Text 5: ABTASTZEIT: (Pos. 2E)
		retlw	4E			; N
		retlw	54			; T
		retlw	45			; E
		retlw	52			; R
		retlw	56			; V			
		retlw	41			; A
		retlw	4C			; L
		retlw	3A			; : Doppelpunkt
		retlw	20			; leer
		retlw	20			; leer
		retlw	20			; leer

		retlw	51			; Q   	Text 6: Q0HI,Q0LO: (Pos. 3A)
		retlw	30			; 0
		retlw	48			; H
		retlw	49			; I
		retlw	2C			; ,
		retlw	51			; Q			
		retlw	30			; 0
		retlw	4C			; L
		retlw	4F			; O
		retlw	3A			; : Doppelpunkt
		retlw	20			; leer

		retlw	51			; Q 	Text 7: Q1HI,Q1LO: (Pos. 45)
		retlw	31			; 1
		retlw	48			; H
		retlw	49			; I
		retlw	2C			; ,
		retlw	51			; Q			
		retlw	31			; 1
		retlw	4C			; L
		retlw	4F			; O
		retlw	3A			; : Doppelpunkt
		retlw	20			; leer			(Pos. 4C)
		retlw	20			; leer
		retlw	20			; leer
		retlw	20			; leer
		retlw	20			; leer
		retlw	20			; leer

		retlw	51			; Q       Text 8: Q2HI,Q2LO: (Pos. 55)
		retlw	32			; 0
		retlw	48			; H
		retlw	49			; I
		retlw	2C			; ,
		retlw	51			; Q			
		retlw	32			; 2
		retlw	4C			; L
		retlw	4F			; O
		retlw	3A			; : Doppelpunkt
		retlw	20			; leer
		retlw	20			; leer
		retlw	20			; leer
		retlw	20			; leer
		retlw	20			; leer
		retlw	20			; leer

		retlw	41			; A  	Text 9: ACT.VALUE (Pos. 65)
		retlw	43			; C
		retlw	54			; T
		retlw	2E			; .
		retlw	56			; V
		retlw	41			; A			
		retlw	4C			; L
		retlw	55			; U 
		retlw	45			; E			
		retlw	3A			; : Doppelpunkt
		retlw	20			; leer			(Pos. 70)
		retlw	20			; leer
		retlw	20			; leer
		retlw	20			; leer
		retlw	20			; leer
		retlw	20			; leer
;********************************************************************************
; Tabelle der ASCII-Zeichen der HEX-Ziffern 0 bis F
;********************************************************************************
asci		addwf	PC,f			; (w) zum Programmzaehler addieren
		retlw	30			; 0  	
		retlw	31			; 1
		retlw	32			; 2
		retlw	33			; 3
		retlw	34			; 4
		retlw	35			; 5
		retlw	36			; 6
		retlw	37			; 7
		retlw	38			; 8
		retlw	39			; 9
		retlw	41			; A
		retlw	42			; B
		retlw	43			; C
		retlw	44			; D
		retlw	45			; E
		retlw	46			; F			
;********************************************************************************
; Die folgenden 8 Unterprogramme dienen dem Lesen aus dem bzw. Schreiben in das EEPROM
;********************************************************************************
bstart		bsf	PORTB,SDA
		bsf	STAT,RP0		; S. 1
		movlw	3F			; RB6/7 = Ausgaenge
		movwf	TRISB
		bcf	STAT, RP0		; S. 0
		bcf	PORTB,SCL
		nop
		bsf	PORTB,SCL		; Takt = 1
		nop
		nop
		nop
		nop
		nop
		bcf	PORTB,SDA		; SDA = 0 bei SCL = 1, Startbedingung
		nop
		nop
		nop				; Setup-Zeit
		nop
		nop
		bcf	PORTB,SCL		; Takt = 0
		nop
		nop
		retlw	0
;********************************************************************************
bstop		bsf	STAT,RP0		; S. 1
		movlw	3F			; RB6/7 = Ausgaenge
		movwf	TRISB
		bcf	STAT,RP0		; S. 0
		bcf	PORTB,SDA		; zur Sicherheit, dass SDA auch wirklich LOW ist
		nop
		nop
		nop
		bsf	PORTB,SCL		; Takt = 1
		nop
		nop
		nop
		bsf	PORTB,SDA
		nop
		nop
		bcf	PORTB,SCL		; Takt = 0
		nop
		nop
		nop
		retlw	0
;********************************************************************************
bitein		bsf	EEPROM,DI
		bsf	STAT,RP0		; S. 1
		movlw	0BF			; 1011 1111b, SDA = Eingang, SCL = Ausgang
		movwf	TRISB
		bcf	STAT,RP0		; S. 0
		bsf	PORTB,SDA		; SDA = 1 (?)
		bsf	PORTB,SCL		; Takt = 1
		nop
		nop
		nop
		nop
		nop
		btfss	PORTB,SDA		; SDA-pin lesen
		bcf	EEPROM,DI
		bcf	PORTB,SCL		; Takt = 0
		retlw	0
;********************************************************************************
bitaus		bsf	STAT,RP0		; S. 1
		movlw	3F			; RB6/7 = Ausgaenge
		movwf	TRISB
		bcf	STAT,RP0		; S. 0
		btfss	EEPROM,DO
		goto	bit0
		bsf	PORTB,SDA		; Ausgabe: SDA = 1
		goto	takt1			; 
bit0		bcf	PORTB,SDA		; Ausgabe: SDA = 0
takt1		bsf	PORTB,SCL		; Takt = 1
		nop
		nop
		nop
		nop
		bcf	PORTB,SCL		; Takt = 0
		retlw	0
;********************************************************************************
lies		clrf	DATEIN
		bcf	STAT,C			; damit Carry beim Linksschieben keinen Unfug macht!
		movlw	08
		movwf	ZAEHL
wdh6		rlf	DATEIN,f
		call	bitein
		btfsc	EEPROM,DI
		bsf	DATEIN,0		; Eingabe bit = 1, wenn noetig
		decfsz	ZAEHL,f
		goto	wdh6
		bsf	EEPROM,DO		; Quittierbit = 1 setzen
		call	bitaus			; um Stop zu erlauben
		retlw	0
;********************************************************************************
schreib		movlw	08
		movwf	ZAEHL
wdh8		bcf	EEPROM,DO		; Ausgabebit in EEPROM loeschen
		btfsc	ZWISCH1,7		; bit 7 von ZWISCH1 lesen
		bsf	EEPROM,DO		; ist es = 1, wird Ausgabebit in EEPROM gesetzt
		call	bitaus			; sonst bleibt es 0
		rlf	ZWISCH1,f
		decfsz	ZAEHL,f
		goto	wdh8
		call	bitein			; ACKnowledge abfragen
		btfsc	EEPROM,DI
		nop				; (hier steht im Original: bsf portA,3)
		retlw	0
;*********************************************************************************
; Programmteil "Schreiben " schreibt jeweils 1 Byte aus DATAUS in den EEPROM-Speicher
;*********************************************************************************
schrb		nop
		call	bstart
		movlw	teiladrs
		movwf	ZWISCH1
		call	schreib
		movf	WORTADR,w
		movwf	ZWISCH1
		call	schreib
		movf	DATAUS,w
		movwf	ZWISCH1
		call	schreib
		call	bstop
		call	pause1			; 5 ms Schreibpause
		clrwdt
		return
;********************************************************************************
liesepr		clrf	DATEIN			; aus EEPROM lesen 
		clrf	EEPROM
		clrf	ZWISCH1
		call	bstart
		movlw	teiladrs
		movwf	ZWISCH1
		call	schreib
		movf	WORTADR,w
		movwf	ZWISCH1
		call	schreib
		call	bstart
		movlw	teiladrs
		movwf	ZWISCH1
		bsf	ZWISCH1,0		; bit 0 = 1: fuer lesen
		call	schreib
		call	lies			; aus dem EEPROM nach DATEIN
		call	bstop
		clrwdt	
		return
;****************************************************************************************************************************
; Die 2 folgenden Unterprogramme dienen der Berechnung der Stellgroesse
;*******************************************************************************
;  Programmsegment zur 8-Bit x 16-Bit- Festkomma-Multiplikation (AN 617 von Microchip)
; Vor Ausfuehrung von mult1 wird geprüft, ob Faktor A (16 Bit) bzw. Faktor B (8 Bit) negativ sind
;*******************************************************************************
mult0		btfss	AHI,7			; MSB von AHI = 1?
		goto	mult1			; Faktor A positiv: sofort zu mult1
		nop				; Faktor A negativ: positiv machen
		movf	ALO,f			; (ALO) = 0?
		btfsc	STAT,Z
		decf	AHI,f			; ja: AHI dekrementieren
		decf	ALO,f			; nein; nur ALO dekrementieren
		comf	ALO,f
		comf	AHI,f			; Faktor A positiv gemacht
						; Faktor B testen
		btfss	BLO,7			; MSB von BLO = 1?
		goto	posneg			; Faktor B positiv: Vorzeichen tauschen
		nop				; Faktor A und B negativ: beide positiv machen
		decf	BLO,f
		comf	BLO,f			; Faktor B positiv gemacht
		goto	mult1
posneg		comf	BLO,f			; Faktor B negativ gemacht
		incf	BLO,f

mult1		btfss	BLO,7			; Faktor B negativ?
		goto	pos			; nein	
		comf	BLO,f			; ja: Zweierkomplement von B und A bilden
		incf	BLO,f
		comf	ALO,f
		incf	ALO,f
		btfsc	STAT,Z
		decf	AHI,f
		comf	AHI,f
pos		clrf	ACCLO			; Ergebnisspeicher loeschen
		clrf	ACCMI
		clrf	ACCHI
		movf	AHI,w
		movwf	ZWISCH0
		movf	ALO,w
		movwf	ZWISCH1		; Faktor A in ZWISCH 0/1 speichern

		movlw	07
		movwf	ZAEHL

wdh21		rrf	BLO,f			;
		btfsc	STAT,C			; LSB von B auf 1 testen
		goto	ninull			; wenn LSB = 1, ist B nicht 0
		decfsz	ZAEHL,f
		goto	wdh21			; Schleife wird nur benutzt, um B = 0 zu testen
		clrf	AHI
		clrf	ALO
		clrwdt
		return				; Ruecksprung bei B = 0
		
wdh22		rrf	BLO,f
		btfss	STAT,C			; Bits von B auf 1 testen
		goto	wdh23			; Bit = 0: nur rechts schieben
ninull		movf	ZWISCH1,w		; Bit =1: Faktor B addieren
		addwf	ACCMI,f
		movf	ZWISCH0,w
		btfsc	STAT,C
		incfsz	ZWISCH0,w
		addwf	ACCHI,f
wdh23		rlf	ZWISCH0,w		; unbekannt, wozu
		rrf	ACCHI,f			; Ergebnis rechts schieben
		rrf	ACCMI,f
		rrf	ACCLO,f
		decfsz	ZAEHL,f
		goto	wdh22
		rlf	ZWISCH0,w
		rrf	ACCHI,f
		rrf	ACCMI,f
		rrf	ACCLO,f
		clrwdt
		return
;******************************************************************************
; Additionsprogramm nach (Appnote 617 von Microchip, S. 3	)
;******************************************************************************
add		movf	ACCLO,w
		addwf	ULO,f
		movf	ACCMI,w
		btfsc	STAT,C			; Uebertrag?
		incfsz	ACCMI,w		; ja: ACCMI erhoehen und auf 0 abfragen
		addwf	UMI,f			; kein Uebertrag: addieren
		movf	ACCHI,w		; Erhoehung fuehrt zu 0: Addition uebergehen
		btfsc	STAT,C
		incfsz	ACCHI,w
		addwf	UHI,f
		return
;*******************************************************************************
; Start des RTCC-Zeitgebers durch Laden des OPTION-Registers
;*******************************************************************************
start1		nop
		bsf	STAT,RP0		; Reg.-Seite1
		movlw	07			; RTCC vorbereiten (Intern, Vorteiler 256, d.h. 250 us-Takt)
		movwf	OPTREG			; nun laeuft er schon
		bcf	STAT,RP0		; zurueck zur Seite 0
		return
;*******************************************************************************
; Unterprogramm fuer 2 feste Pausen sowie fuer variable Pausen, deren Dauer vom Inhalt von RTCC abhaengt
;*******************************************************************************
pause		movlw	38			; feste Prellpause 50 ms
		movwf	RTCC			; RTCC = 56d: 200 x 250 us = 50 ms
		goto	pause2
pause1		movlw	0EC			; EEPROM-Schreibpause 5 ms
		movwf	RTCC			; RTCC = 236d: 20 x 250 us = 5 ms
pause2		clrwdt
		movf	RTCC,w	
		btfss	STAT,Z
		goto	pause2
		return
;********************************************************************************
; Start des AD-Wandlers 
;********************************************************************************
adw		bsf	STAT,RP0		; Seite 1 des Registers
		movlw	13			; RA0-1 hochohmig 
		movwf	TRISA			; 
		movlw	02			; RA0/1 analog, Uref = UB, RA2/3 dig I/O
		movwf	ADCON1
		bcf	STAT,RP0		; Seite 0 des Registers
		movlw	0C1			; RC-Oszillator, RA0 aktiv, ADW ein
		movwf	ADCON0
		clrw				;
		movwf	INTCON			; Interrupts aus
		bsf	ADCON0,2		; Start der Wandlung
busy		btfsc	ADCON0,2		; warten, bis busy-flag geloescht
		goto	busy
		nop
		nop
		return
;*********************************************************************************
; Tastaturabfrage, wird mit (w) = 1 verlassen, wenn Taste betaetigt, sonst mit (w) = 0
;************************************************************************************************************************************
tast1		bsf	STAT,RP0		; S. 1
		bcf	OPTREG,7		; Pullup eingeschaltet
		movlw	0F0			; Definition PortB: RB0 - RB3 = Ausgang
		movwf	TRISB			; RB4 -RB7= Eingang
		bcf	STAT,RP0		; S. 0

tast		call	lesen
		bcf	STAT,Z			; Zerobit im Statusregister loeschen
		movlw	10			; w-Register wird mit 10h geladen
		subwf	TASTE,w		; Vergleich von TASTE mit 10h durch Subtraktion
		btfss	STAT,Z			; Zerobit wird gesetzt, wenn TASTE = 10h, d.h. keine Taste betaetigt
		goto	wart			; ist TASTE < 10h,  Prellpause abwarten
		retlw	0			; sonst UP mit (w) = 0 verlassen
wart		call	pause			; Pause von etwa 50 ms Dauer ueberbrueckt Prellen
		movf	TASTE,w 		; (TASTE) nach w
		movwf	ZWISCH		; (TASTE) = ZWISCH
wdh		clrwdt
		call	lesen
		movf	TASTE,w		;  Vergleich TASTE = ZWISCH?
		subwf	ZWISCH,w		; 	
		btfsc	STAT,Z			; Zerobit wird gesetzt, wenn TASTE = ZWISCH	
		goto	wdh			; Z = 1: warten bis losgelassen
		retlw	1			; UP mit (w) = 1 verlassen
;*********************************************************************************
; Unterprogramm "lesen" zur Tastaturabfrage
;*********************************************************************************
lesen		clrf	TASTE	
		movlw	0FF		
		movwf	PORTB			; Port B = 1111 1111b
		movlw	4
		movwf	UMI			; Zeilenzaehler auf 4 setzen (UMI hier frei)
		bcf	STAT,C			; Carrybit loeschen
lies1		rlf	PORTB,f			; Carrybit nach RB0
		btfss	PORTB,spalte1		; Naechsten Befehl ueberspringen, wenn RB0 = 1 
		goto	fertig			; sonst "lesen" verlassen
		incf	TASTE,f			; TASTE erhoehen
		btfss	PORTB,spalte2		; Naechsten Befehl ueberspringen, wenn RB1 = 1
		goto	fertig			; u.s.w.
		incf	TASTE,f			; Die Schleife "lies1" wird max. viermal
		btfss	PORTB,spalte3		; durchlaufen, wenn keine Taste gedrueckt ist
		goto	fertig
		incf	TASTE,f	
		btfss	PORTB,spalte4	
		goto	fertig
		incf	TASTE,f	
		decfsz	UMI,f			; Zeilenzaehler herunterzaehlen, bei 0 springen
		goto	lies1
fertig		return
;*********************************************************************************
; Taste "Set" wird auf  0 und Loslassen getestet (An RB0)
;*********************************************************************************
sets		bsf	STAT,RP0		; S. 1
		bcf	OPTREG,7		; RBPU enable
		movlw	01			; Port B,0 = Eingang
		movwf 	TRISB
		bcf	STAT,RP0		; S. 0
		btfss	PORTB,0
		goto	wdhx
		bsf	STAT,RP0		; S. 1
		clrf	TRISB
		bcf	STAT,RP0		; S. 0
		retlw	0
wdhx		call	pause
		btfss	PORTB,0
		goto	wdhx
		retlw	1
;*********************************************************************************
; hier werden die Tastatur und die Taste "S"abgefragt; nur wenn "S" gedrueckt, wird das UP verlassen
;*********************************************************************************
aendern		call	tast1			; Tastaturabfrage 
		xorlw	0	
		btfss	STAT,Z			; w = 0: zu sets
		goto	ziff1			; w = 1: 1. Ziffer bearbeiten
		call	sets			; jetzt Taste "S" fragen. Ist sie nicht betätigt, zu aendern zurück	
						; sonst zur naechsten Eingabe
		xorlw	0
		btfss	STAT,Z			; 
		retlw	0			; w = 1: UP verlassen mit (w) = 0
		clrwdt
		goto	aendern			; w = 0: Tasten abfragen

ziff1		movf	ZWISCH,w		; Tastenwert in
		movwf	ACCMI			; ACCMI speichern und (ACCMI hier benutzbar)
		swapf	ACCMI,f		; Nibbles vertauschen

aend2		clrwdt
		call	tast1			; 
		xorlw	0			; 
		btfsc	STAT,Z
		goto	aend2			; w = 0: Taste wieder abfragen
ziff2		movf	ZWISCH,w		; Tastenwert zu
		addwf	ACCMI,f		; ACCMI addieren (ACCMI = neuer HEX-Wert)
		retlw	1			; UP verlassen mit (w) = 1
;********************************************************************************
; hier werden alle Reglerparameter eingestellt; das Unterprogramm wird erst verlassen, wenn Set betaetigt ist
;********************************************************************************
einstell		movwf	ZWISCH
		movlw	8C	
		movwf	ADRLCD		; LCD-Adresse
		btfss	ULO,0			; Flag testen (Flag wird gesetzt fuer 2-Byte-Parameter)
		goto	anzg1
		incf	ADRLCD,f
		incf	ADRLCD,f
anzg1		call	anzeig			; bisherigen Parameter anzeigen
wdhy		call	aendern			; Neueingabe erwarten (Zwei HEX-Ziffern) oder "Set"
		xorlw	0
		btfsc	STAT,Z
		return				; (w) = 0, "S" ist gedrueckt: Zum naechsten Parameter gehen
		movf	ACCMI,w		; (w) = 1: neuer Parameter in ACCMI
		movwf	ZWISCH
		movlw	8C	
		movwf	ADRLCD		; LCD-Adresse
		btfss	ULO,0			; Flag testen
		goto	anzg2
		incf	ADRLCD,f
		incf	ADRLCD,f
anzg2		call	anzeig			; neuen Parameter anzeigen
		clrwdt
		goto	wdhy			; wieder aendern oder "S" erwarten
;*********************************************************************************
; Die folgenden 7 Unterprogramme dienen der Anzeige von Text bzw. Daten auf dem LCD-Modul
;*********************************************************************************
; Vorbereiten der  Ports A und B fuer  LCD-Ansteuerung 
;*********************************************************************************
lcport		bsf 	STAT,RP0		; S. 1 
		movlw	02
		movwf	ADCON1		; RA2-3 digital
		movlw	13			; 0001 0011b
		movwf	TRISA			; RA2, RA3 = Ausgang
		clrf	TRISB			; Port B = Ausgang
		bcf	STAT,RP0		; S. 0
		return
;*********************************************************************************
; laedt LCD-Adresse und bereitet Schreiben vor
;*********************************************************************************
lcads		movlw	13		
		movwf	BEFEHL			; BEFEHL = 0 												
		movf	ADRLCD,w		; LCD-Adresse
		call	ausg0
		movlw	1B			; Vorbereitung: RA3=1 (Daten schreiben)
		movwf	BEFEHL
		return
;*********************************************************************************
; loescht Port B und RA0 - RA3, ohne Einfluß auf RA4 (Alarmanschluss)
;*********************************************************************************
nulli		movlw	13
		movwf	BEFEHL			; BEFEHL = 0 (ohne Einfluss auf RA4)
		movf	PORTA,w
		andlw	12
		movwf	PORTA			; loescht nur RA0,RA2-3, RA1 u. RA4 unbeeinflusst
		clrf	PORTB
		return
;*********************************************************************************
; Ausgaberoutine fuer das LCD 
;*********************************************************************************
ausg0		movwf	PORTB			; Datenteil der Anweisung an Port B
		movf	BEFEHL,w		; Befehlsteil nach w
		movwf	PORTA			; 
		bsf	PORTA,2		; E = 1 (Freigabe)
		nop
		nop
		bcf	PORTA,2		; E = 0 
		clrwdt
		movlw	40			; 40h = 64d
		movwf	UMI			; (UMI wird sonst nur beim Rechnen gebraucht)
wdh30		decfsz	UMI,f			; von RTCC unabh. Verzoegerung von 64 x 3 = 200 us
		goto	wdh30
		return
;********************************************************************************
; zeigt (ZWISCH0) auf dem LCD an
;********************************************************************************
anz		call	lcport
		call	lcads
		movf	ZWISCH0,w
		nop		
		nop
		call 	ausg0			; und  (ZWISCH0) anzeigen
		call	nulli
		return
;*******************************************************************************
; zeigt den Inhalt eines Registers auf dem LCD an (ZWISCH bleibt erhalten)
;*******************************************************************************
anzeig		swapf	ZWISCH,w		; Nibbles vertauschen
		andlw	0F			; oberes Nibble maskieren
		call	asci			; ASCII-Code in w
		movwf	ZWISCH0
		call	anz
		clrwdt
		incf	ADRLCD,f
		movlw	0F
		andwf	ZWISCH,w		; oberes Nibble maskieren
		call	asci			; ASCII-Code in w
		movwf	ZWISCH0
		call	anz
		clrwdt
		return
;********************************************************************************
; schreibt Festtexte aus Tabelle ins LCD
;********************************************************************************
text		movwf	ZAEHL			; Tabellenanfang
		movlw	80
		movwf	ADRLCD		; LCD-Start-Adresse 1. Zeile
textx		call	lcport			; LCD zum Schreiben vorbereiten mit Start-Adresse
		call	lcads
lies2		movf	ZAEHL,w		; ZAEHL  fungiert als Tabellen-Index
		call	tab			; Auszugebendes ASCII-Zeichen in w
		nop
		nop
		call	ausg0			; hier geschieht es
		incf	ZAEHL,f
		decfsz	ZAEHL1,f		; Bytezaehler herunterzaehlen
		goto	lies2 			; zurueck zu lies2, bis alle  Bytes geschrieben sind
		call	nulli			; sicher ist sicher
		return
;********************************************************************************
alarm		bsf	STAT,RP0		; Alarm 
		movlw	03			; RA4 = Ausgang f. Alarm
		movwf	TRISA
		bcf	STAT,RP0
		bcf	PORTA,4		; RA4 = 0 (LED oder Summer) aktiv low
		movlw	0A			; Alarmzeit 10 x 50 ms = 0,5 s
		movwf	ZAEHL
wdh27		call	pause
		decfsz	ZAEHL,f
		goto	wdh27
		bsf	PORTA,4		; 
		movlw	0A			; Alarmzeit 10 x 50 ms = 0,5 s
		movwf	ZAEHL
wdh28		call	pause
		decfsz	ZAEHL,f
		goto	wdh28
		return
;********************************************************************************
istwert		movf	ADRES,w		; Istwert nach w
		movwf	ZWISCH
		movlw	0CE	
		movwf	ADRLCD		; LCD-Adresse (2. Zeile)
		call	anzeig			; Istwert anzeigen  (Port B = Ausgang)
		return
;********************************************************************************
stellgr		movf	STELLG,w		; aktuelle Stellgroesse anzeigen
		movwf	ZWISCH
		movlw	8E	
		movwf	ADRLCD		; LCD-Adresse
		call	anzeig	
		return
;********************************************************************************
; "latch" erzeugt den latch-Impuls fuer den DA-Wandler ueber RA1		
;********************************************************************************
latch		bsf	STAT,RP0		; S. 1
		movlw	03
		movwf	ADCON1		; RA0 - RA3 digital I/O
		movlw	11			; 0001 0001b
		movwf	TRISA			; RA1 = Ausgang
		clrf	TRISB			; Port B = Ausgang
		bcf	STAT,RP0		; S. 0
		movlw	1D			; 0001 1101b
		movwf	PORTA			; RA1 = 0
		nop	
		nop
		movlw	1F			; 0001 1111b
		movwf	PORTA			; RA1 = 1
		return

;********************************************************************************
; Beginn des Hauptprogrammes; Initialisierung des LCD-Moduls 
;********************************************************************************
init		clrf	ADRES	
		clrf	EEPROM
		clrf	DATEIN
		clrf	DATAUS
		clrf	ADRLCD
		call	start1			; RTCC starten
;********************************************************************************
; Fuer den Fall eines Reset infolge WDT-Time-Out wird das TO-Bit in STAT abgefragt
;********************************************************************************
		btfsc	STAT,TO		; TO = 0? WDT-Time-Out?
		goto	danull			; nein: zu danull
		nop				; ja: STELLGR = 0 und Alarm 
		clrf 	PORTB			; bedeutet Stellgroesse = 0!
		call  	latch
alarm1		call	alarm			; warten, bis normal Reset
		goto	alarm1
;********************************************************************************
; Ausgang des DA-Wandlers = 0 setzen
;********************************************************************************
danull		clrf	PORTB
		call	latch
;********************************************************************************
lcdini		call	lcport			; Initialisierung des LCD mit Dauerwerten
		clrf	PORTB
		call pause	;Verzoegerung ca. 50ms
		nop				; Daten bleiben so lange erhalten,
		movlw	10			; bis sie durch andere Befehle ueberschrieben werden
		movwf	BEFEHL			; RA0 bis RA3 = 0 (Befehl schreiben)
		movlw 30	;init LCD
		call ausg0
		call pause1	;Verzoegerung ca. 5ms
		movlw 30	;init lcd
		call ausg0
		call pause1	;Verzoegerung ca. 5ms
		movlw 30	;init lcd
		call ausg0
		call pause1	;Verzoegerung ca. 5ms
		movlw	01			; Steuerwort fuer "Display Loeschen"
		call	ausg0
		call pause1	;Verzoegerung ca. 5ms

		movlw	0C			; Steuerwort fuer Einschalten, Cursor aus
		call	ausg0
		movlw	06			; Steuerwort fuer Nicht Schieben, Adr. erhoehen
		call	ausg0
		movlw	38			; Steuerwort fuer Zeichenformat
		call	ausg0
;********************************************************************************
; Einlesen der im EEPROM gespeicherten Werte in die zugehoerigen Register
;********************************************************************************
		clrf	WORTADR		; EEPROM-Adresse 0 fuer Sollwert
		call	liesepr			; Sollwert aus EEPROM lesen 
		movf	DATEIN,w
		movwf	SOLLW 
		incf	WORTADR,f
		call	liesepr			; Stellgroesse aus EEPROM lesen 
		movf	DATEIN,w
		movwf	STELLG
		incf	WORTADR,f
		call	liesepr			; OGRW aus EEPROM lesen 
		movf	DATEIN,w
		movwf	OGRW
		incf	WORTADR,f
		call	liesepr			; UGRW aus EEPROM lesen 
		movf	DATEIN,w
		movwf	UGRW
		incf	WORTADR,f
		call	liesepr			; Abtastzeit aus EEPROM lesen 
		movf	DATEIN,w
		movwf	ZEIT 
		incf	WORTADR,f
		call	liesepr			; Q0HI aus EEPROM lesen 
		movf	DATEIN,w
		movwf	Q0HI 
		incf	WORTADR,f
		call	liesepr			; Q0LO aus EEPROM lesen 
		movf	DATEIN,w
		movwf	Q0LO 
		incf	WORTADR,f
		call	liesepr			; Q1HI aus EEPROM lesen 
		movf	DATEIN,w
		movwf	Q1HI 
		incf	WORTADR,f
		call	liesepr			; Q1LO aus EEPROM lesen 
		movf	DATEIN,w
		movwf	Q1LO 
		incf	WORTADR,f
		call	liesepr			; Q2HI aus EEPROM lesen 
		movf	DATEIN,w
		movwf	Q2HI 
		incf	WORTADR,f
		call	liesepr			; Q2LO aus EEPROM lesen 
		movf	DATEIN,w
		movwf	Q2LO 
;********************************************************************************
; Einstellen der Reglerparameter: Erst wird der jeweilige Text angezeigt, danach erwartet das Programm
; die Eingabe der Parameter ueber die HEX-Tastatur
;********************************************************************************
; Text 1: SETVALUE
;********************************************************************************
neu		movlw	09			; Bytezaehler = 09d
		movwf	ZAEHL1			; ZAEHL1 ist dafuer frei
		clrw				; ZAEHL = 0 fuer Text 1
		call	text			
;********************************************************************************
		movf	SOLLW,w		; Sollwert eingeben
		call	einstell
		movf	ZWISCH,w		; erst SOLLW im EEPROM speichern
		movwf	SOLLW
		movwf	DATAUS
		clrf	WORTADR		; Sollwertadresse im EEPROM
		call	schrb
;********************************************************************************
; Text 2: MAN.VAR.
;********************************************************************************
		movlw	0B			; Bytezaehler = 09d
		movwf	ZAEHL1			
		movlw	0B			; Startpunkt in Festtext-Tabelle
		call	text	
;********************************************************************************
		movf	STELLG,w		; Stellgroesse eingeben
		call	einstell
		movf	ZWISCH,w		; erst STELLG im EEPROM speichern
		movwf	STELLG
		movwf	DATAUS
		incf	WORTADR,f		; Stellgroessenadresse im EEPROM
		call	schrb
;********************************************************************************
; Text 3: UPPER LIMIT
;********************************************************************************
		movlw	0C			; Bytezaehler = 12d
		movwf	ZAEHL1		
		movlw	16			; Startpunkt in Festtext-Tabelle
		call	text	
;********************************************************************************
		movf	OGRW,w		; Obergrenze eingeben
		call	einstell
		movf	ZWISCH,w		; erst Obergrenze im EEPROM speichern
		movwf	OGRW
		movwf	DATAUS
		incf	WORTADR,f		; Obergrenzenadresse im EEPROM
		call	schrb
;********************************************************************************
; Text 4: LOWER LIMIT
;********************************************************************************
		movlw	0C			; Bytezaehler = 12d
		movwf	ZAEHL1
		movlw 	22			; Startpunkt in Festtext-Tabelle
		call	text	
;********************************************************************************
		movf	UGRW,w		; Untergrenze eingeben
		call	einstell
		movf	ZWISCH,w		; erst Untergrenze im EEPROM speichern
		movwf	UGRW
		movwf	DATAUS
		incf	WORTADR,f		; Untergrenzenadresse im EEPROM
		call	schrb
;********************************************************************************
; Text 5: INTERVAL
;********************************************************************************
		movlw	0C			; Bytezaehler = 12d
		movwf	ZAEHL1
		movlw 	2E			; Startpunkt in Festtext-Tabelle
		call	text	
;********************************************************************************
		movf	ZEIT,w			; Abtastzeitfaktor eingeben
		call	einstell
		movf	ZWISCH,w		; erst Abtastzeitfaktor im EEPROM speichern
		movwf	ZEIT
		movwf	DATAUS
		incf	WORTADR,f
		call	schrb
;********************************************************************************
; Text 6: Q0HI,Q0LO
;********************************************************************************
		movlw	0A			; Bytezaehler = 10d
		movwf	ZAEHL1
		movlw 	3A			; Startpunkt in Festtext-Tabelle
		call	text	
;********************************************************************************
		movf	Q0HI,w			; Q0,high eingeben
		call	einstell
		movf	ZWISCH,w		; erst Q0HI im EEPROM speichern
		movwf	Q0HI
		movwf	DATAUS
		incf	WORTADR,f
		call	schrb
		bsf	ULO,0			; ULO wird hier als Flag benutzt fuer 2-Byte-Parameter
		movf	Q0LO,w			; Q0,low eingeben
		call	einstell	
		movf	ZWISCH,w		; erst Q0LO im EEPROM speichern
		movwf	Q0LO
		movwf	DATAUS
		incf	WORTADR,f
		call	schrb
		clrf	ULO			; Flag loeschen
;********************************************************************************
; Text 7: Q1HI,Q1LO
;********************************************************************************
		movlw	10			; Bytezaehler = 16d
		movwf	ZAEHL1
		movlw 	45			; Startpunkt in Festtext-Tabelle
		call	text	
;********************************************************************************
		movf	Q1HI,w			; Q1,high eingeben
		call	einstell	
		movf	ZWISCH,w		; erst Q1HI im EEPROM speichern	
		movwf	Q1HI
		movwf	DATAUS
		incf	WORTADR,f
		call	schrb
		bsf	ULO,0			; ULO wird hier als Flag fuer 2-Byte-Parameter benutzt
		movf	Q1LO,w			; Q1,low eingeben
		call	einstell
		movf	ZWISCH,w		; erst Q1LO im EEPROM speichern
		movwf	Q1LO
		movwf	DATAUS
		incf	WORTADR,f
		call	schrb
		clrf	ULO			; Flag loeschen
;********************************************************************************
; Text 8: Q2HI,Q2LO
;********************************************************************************
		movlw	10			; Bytezaehler = 16d
		movwf	ZAEHL1
		movlw 	55			; Startpunkt in Festtext-Tabelle
		call	text	
;****************************************************************************************************************************
		movf	Q2HI,w			; Q2,high eingeben
		call	einstell
		movf	ZWISCH,w		; erst Q2HI im EEPROM speichern
		movwf	Q2HI
		movwf	DATAUS
		incf	WORTADR,f
		call	schrb
		bsf	ULO,0			; ULO wird hier als Flag fuer 2-Byte-Parametr benutzt
		movf	Q2LO,w			; Q2,low eingeben
		call	einstell
		movf	ZWISCH,w		; erst Q2LO im EEPROM speichern
		movwf	Q2LO
		movwf	DATAUS
		incf	WORTADR,f
		call	schrb
		clrf	ULO			; Flag loeschen

; Ende der Parameter-Eingabe
;*********************************************************************************
; Text 2 und Text 9 anzeigen , Anfangs-Stellgroesse ausgeben und Erreichen des Sollwertes erwarten
;*********************************************************************************
stell0		movlw	0B			; Bytezaehler = 11d f. Text 2: MAN.VAR.
		movwf	ZAEHL1			
		movlw	0B			; Startpunkt in Festtext-Tabelle
		call	text	
		movlw	06			; 8 Leerzeichen, um Vorhandenes zu loeschen
		movwf	ZAEHL1
		movlw	70			; Startpunkt fuer Leerzeichen in tab
		movwf	ZAEHL
		movlw	8B
		movwf	ADRLCD		; LCD-Adresse in 1. Zeile
		call	textx

		movlw	10			; Bytezaehler = 16d f. Text  9: ACT.VALUE
		movwf	ZAEHL1			
		movlw	65			; Startpunkt in tab
		movwf	ZAEHL
		movlw	0C0
		movwf	ADRLCD		; LCD-Adresse in 2. Zeile
		call	textx
;********************************************************************************
		call	stellgr			; Stellgroesse anzeigen
		movf	STELLG,w
		movwf	PORTB			; Anfangs-Stellgroesse an Port B ausgeben
		call	latch

		movlw	0D8			; 10 ms Grundzeit vorbereiten
		movwf	RTCC
wand		call	adw			; adw aufrufen: Istwert messen
		call	istwert			; Istwert anzeigen (Port B = Ausgang)
		movf	ADRES,w		; Istwert nach w
		subwf	SOLLW,w
		btfss	STAT,C			; Sollwert erreicht?
		goto	regel			; ja: regeln
		nop				; nein: Pause und Messung wiederholen
wdh18		movf	RTCC,w			; innere Abfragezeitschleife: Wartezeit  = 10 ms
		btfss	STAT,Z
		goto	wdh18
		clrwdt
		movlw	0D8			; 10 ms um: RTCC neu laden
		movwf	RTCC
		call	start1			; Wiederholen des RTCC-Starts
		goto	wand			; neue Messung 
;********************************************************************************
; Hier beginnt die Regelung, nachdem der Sollwert erstmals erreicht ist
;********************************************************************************

regel		clrf	ABW0
		clrf	ABW1
		clrf	ABW2
regel1		movlw	0D8			; RTCC fuer 10 ms Grundzeit initialisieren
		movwf	RTCC	
		movlw	0A
		movwf	ZAEHL1			; ZAEHL1 mit 10d laden fuer 100 ms
		movf	ZEIT,w
		movwf	EEPROM		; hier als Zwischenspeicher fuer ZEIT benutzbar

mess		call	start1
		clrf	UHI
		clrf	UMI
		clrf	ULO
		movf	ABW1,w		; Registerinhalte verschieben
		movwf	ABW2
		movf	ABW0,w
		movwf	ABW1
		call	adw			; adw aufrufen
		movf	ADRES,w		; Istwert nach w
		subwf	OGRW,w		; Ueberschreiten des oberen Grenzwertes testen
		btfss	STAT,C
		goto	alarm2
		movf	ADRES,w
		subwf	UGRW,w		; Unterschreiten des unteren Grenzwertes testen
		btfsc	STAT,C
		goto	alarm3
		movf	ADRES,w
		subwf	SOLLW,w		; Regelabweichung bilden
		movwf	ABW			; dies ist die Regelabweichung

; nun wird ABW getestet, ob positiv oder negativ und auf  Ueber- bzw. Unterschreiten 
; von +127 bzw. -127d  (7-Bit-Zahl  mit Vorzeichen)

		btfsc	STAT,C			; ABW positiv?
		goto	test1			; ja: 
		btfsc	ABW,7			; nein: Bit 7 auf 0 testen
		goto	weit1			; Bit 7 nicht 0: o.k.
		movlw	80
		movwf	ABW			; Grenzwert -127d fuer ABW einsetzen
		goto	weit1
test1		btfss	ABW,7			; bei pos. ABW Bit 7 auf 1 testen
		goto	weit1			; Bit 7 = 0: o.k.
		movlw	7F
		movwf	ABW			; Grenzwert +127d fuer ABW einsetzen
weit1		movf	ABW,w
		movwf	ABW0			; aktuellen Wert in ABW0 einsetzen
		movlw	03
		movwf	DATEIN	 		; hier fuer den Zweck als Schleifenzaehler frei
		movlw	30			; Startwert f. indirekte Adressierung laden 
		movwf	FSR			; (1 ueber hoechster RAM-Adresse 2F)
wdh19		decf	FSR,f
		movf	IND,w			; ABW0, ABW1 bzw. ABW2
		movwf	BLO			; nach BLO
		decf	FSR,f
		movf	IND,w			; Q0LO, Q1LO bzw. Q2LO 
		movwf	ALO			; nach ALO
		decf	FSR,f
		movf	IND,w			; Q0HI, Q1HI bzw. Q2HI
		movwf	AHI			; nach AHI
		call	mult0			; Multiplikation ausfuehren
		call	add			; Addition ausfuehren
		decfsz	DATEIN,f
		goto	wdh19

; aus (UHI, UMI, ULO) durch Addition von (STELLG) zu UMI  die neue Stellgroesse ableiten 

		movf 	STELLG,w
		addwf	UMI,f
		btfsc	STAT,C			; Uebertrag?
		incf	UHI,f			; ja: UHI inkrementieren

; Das Ergebnis dieser Operation ist die neue Stellgroesse

		btfsc	UHI,7			; Ergebnis negativ?
		goto	setz0			; ja: STELLG = 0
		movf	UHI,f			; nein: 
		btfss	STAT,Z			; (UHI) > 0 ?
		goto	setzff			; ja: STELLG = FF
		nop				; nein: also (UHI) = 0
		btfss	ULO,7			;  (ULO) = 0,5?
		goto	weit2			; nein: weiter
		incf	UMI,f			; ja; UMI aufrunden
		btfsc	STAT,Z			; Uebertrag?
		goto	setzff			; ja: STELLG = FF
weit2		movf	UMI,w			; nein: (UMI) nach STELLG laden
		movwf 	STELLG	
		goto	ausg	
setz0		clrf	STELLG
		goto	ausg
setzff		movlw	0FF
		movwf	STELLG

; Ergebnisse: Istwert und neue Stellgroesse ausgeben

ausg		nop
		movf	STELLG,w
		movwf	PORTB			; Anfangs-Stellgroesse an Port B ausgeben
		call	latch
		call	istwert			; Istwert anzeigen  (Port B = Ausgang)
		call	stellgr			; Stellgroesse anzeigen  (Port B = Ausgang)

wdh20		movf	RTCC,w			; innere Abfragezeitschleife: Wartezeit  = 10 ms
		btfss	STAT,Z
		goto	wdh20
		clrwdt
		movlw	0D8			; 10 ms um
		movwf	RTCC
		decfsz	ZAEHL1,f
		goto	wdh20
		movlw	0A			; 100 ms um
		movwf	ZAEHL1
		decfsz	EEPROM,f
		goto	wdh20
		movf	ZEIT,w			; Ende Abtastzeit, abh. von (ZEIT)
		movwf	EEPROM		; Eingabe in ZEIT uebernehmen
		goto	mess			; naechste Messung

alarm2		clrf	 STELLG			; Stellgroesse = 0
		movf	STELLG,w		; und ausgeben
		movwf	PORTB
		call	latch
		goto	alarm4

alarm3		movlw	0FF			; Stellgroesse = FFh
		movwf	 STELLG
		movf	STELLG,w		; und ausgeben
		movwf	PORTB
		call	latch

alarm4		call 	alarm
		goto	regel1			; Solange, bis keine Ueber- bzw. Unterschreitung mehr
	
		org	3FF
		goto	start
		end



