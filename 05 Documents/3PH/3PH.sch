*version 9.2 2514671175
u 119
V? 4
R? 4
? 8
@libraries
@analysis
.TRAN 1 0 0 0
+0 0ns
+1 10m
@targets
@attributes
@translators
a 0 u 13 0 0 0 hln 100 PCBOARDS=PCB
a 0 u 13 0 0 0 hln 100 PSPICE=PSPICE
a 0 u 13 0 0 0 hln 100 XILINX=XILINX
@setup
unconnectedPins 0
connectViaLabel 0
connectViaLocalLabels 0
NoStim4ExtIFPortsWarnings 1
AutoGenStim4ExtIFPorts 1
@index
pageloc 1 0 4229 
@status
n 0 110:07:25:12:01:31;1282726891 e 
s 2832 110:07:25:14:03:20;1282734200 e 
*page 1 0 1520 970 iB
@ports
port 4 GND_ANALOG 410 390 h
port 20 GND_ANALOG 440 390 h
port 21 GND_ANALOG 470 390 h
port 5 GND_ANALOG 610 390 h
@parts
part 3 r 560 260 h
a 0 sp 0 0 0 10 hlb 100 PART=r
a 0 s 0:13 0 0 0 hln 100 PKGTYPE=RC05
a 0 s 0:13 0 0 0 hln 100 GATE=
a 0 a 0:13 0 0 0 hln 100 PKGREF=R1
a 0 ap 9 0 15 0 hln 100 REFDES=R1
part 47 r 560 320 h
a 0 sp 0 0 0 10 hlb 100 PART=r
a 0 s 0:13 0 0 0 hln 100 PKGTYPE=RC05
a 0 s 0:13 0 0 0 hln 100 GATE=
a 0 a 0:13 0 0 0 hln 100 PKGREF=R3
a 0 ap 9 0 15 0 hln 100 REFDES=R3
part 46 r 560 290 h
a 0 sp 0 0 0 10 hlb 100 PART=r
a 0 s 0:13 0 0 0 hln 100 PKGTYPE=RC05
a 0 s 0:13 0 0 0 hln 100 GATE=
a 0 a 0:13 0 0 0 hln 100 PKGREF=R2
a 0 ap 9 0 15 0 hln 100 REFDES=R2
part 2 VSIN 410 350 h
a 1 u 0 0 0 0 hcn 100 VAMPL=310
a 1 u 0 0 0 0 hcn 100 DC=0
a 1 u 0 0 0 0 hcn 100 AC=0
a 1 u 0 0 0 0 hcn 100 VOFF=0
a 1 u 0 0 0 0 hcn 100 FREQ=100
a 0 x 0:13 0 0 0 hln 100 PKGREF=Vu
a 1 xp 9 0 20 10 hcn 100 REFDES=Vu
part 14 VSIN 440 350 h
a 1 u 0 0 0 0 hcn 100 VAMPL=310
a 1 u 0 0 0 0 hcn 100 DC=0
a 1 u 0 0 0 0 hcn 100 AC=0
a 1 u 0 0 0 0 hcn 100 VOFF=0
a 1 u 0 0 0 0 hcn 100 FREQ=100
a 1 u 0 0 0 0 hcn 100 PHASE=120
a 0 x 0:13 0 0 0 hln 100 PKGREF=Vv
a 1 xp 9 0 20 10 hcn 100 REFDES=Vv
part 15 VSIN 470 350 h
a 1 u 0 0 0 0 hcn 100 VAMPL=310
a 1 u 0 0 0 0 hcn 100 DC=0
a 1 u 0 0 0 0 hcn 100 AC=0
a 1 u 0 0 0 0 hcn 100 VOFF=0
a 1 u 0 0 0 0 hcn 100 FREQ=100
a 1 u 0 0 0 0 hcn 100 PHASE=240
a 0 x 0:13 0 0 0 hln 100 PKGREF=Vw
a 1 xp 9 0 20 10 hcn 100 REFDES=Vw
part 1 titleblk 1520 970 h
a 1 s 13 0 350 10 hcn 100 PAGESIZE=B
a 1 s 13 0 180 60 hcn 100 PAGETITLE=
a 1 s 13 0 300 95 hrn 100 PAGENO=1
a 1 s 13 0 340 95 hrn 100 PAGECOUNT=1
part 62 nodeMarker 540 260 h
a 0 s 0 0 0 0 hln 100 PROBEVAR=R1:1
a 0 a 0 0 4 22 hlb 100 LABEL=2
part 66 nodeMarker 540 320 h
a 0 s 0 0 0 0 hln 100 PROBEVAR=R3:1
a 0 a 0 0 4 22 hlb 100 LABEL=4
part 64 nodeMarker 540 290 h
a 0 s 0 0 0 0 hln 100 PROBEVAR=R2:1
a 0 a 0 0 4 22 hlb 100 LABEL=3
part 102 vdiffMarker 510 260 h
a 0 s 0 0 0 0 hln 100 PROBEVAR=V(V3:+,R1:1)
a 0 a 0 0 6 20 hlb 100 LABEL=7
part 104 vdiffMarker 510 320 h
a 0 s 0 0 0 0 hln 100 PROBEVAR=V(R1:1,R1:1)
a 0 s 13 0 16 14 hlb 100 NODE=-
a 0 a 0 0 6 20 hlb 100 LABEL=7
part 68 vdiffMarker 450 260 h
a 0 s 0 0 0 0 hln 100 PROBEVAR=V(V2:+,V1:+)
a 0 a 0 0 6 20 hlb 100 LABEL=5
a 0 s 13 0 14 14 hlb 100 NODE=+
part 70 vdiffMarker 450 290 h
a 0 s 0 0 0 0 hln 100 PROBEVAR=V(V1:+,V1:+)
a 0 s 13 0 16 14 hlb 100 NODE=-
a 0 a 0 0 6 20 hlb 100 LABEL=5
part 72 vdiffMarker 490 320 h
a 0 s 0 0 0 0 hln 100 PROBEVAR=
a 0 a 0 0 6 20 hlb 100 LABEL=6
part 74 vdiffMarker 480 290 h
a 0 s 0 0 0 0 hln 100 PROBEVAR=V(V3:+,V3:+)
a 0 s 13 0 16 14 hlb 100 NODE=-
a 0 a 0 0 6 20 hlb 100 LABEL=6
@conn
w 7
a 0 up 0:33 0 0 0 hln 100 V=
s 610 260 600 260 43
s 610 260 610 290 24
a 0 up 33 0 612 280 hlt 100 V=
s 610 290 610 320 50
s 600 290 610 290 48
s 610 320 610 390 53
s 600 320 610 320 51
w 33
a 0 up 0:33 0 0 0 hln 100 V=
s 410 350 410 260 32
s 540 260 560 260 93
s 410 260 450 260 110
a 0 up 33 0 465 259 hct 100 V=
s 510 260 540 260 113
s 450 260 510 260 115
w 59
a 0 up 0:33 0 0 0 hln 100 V=
s 470 320 470 350 84
s 560 320 540 320 88
a 0 up 33 0 495 319 hct 100 V=
s 540 320 510 320 109
s 510 320 490 320 114
s 490 320 470 320 117
w 55
a 0 up 0:33 0 0 0 hln 100 V=
s 440 290 440 350 56
s 560 290 540 290 81
a 0 up 33 0 480 289 hct 100 V=
s 540 290 480 290 111
s 450 290 440 290 116
s 480 290 450 290 118
@junction
j 410 390
+ s 4
+ p 2 -
j 440 390
+ s 20
+ p 14 -
j 470 390
+ s 21
+ p 15 -
j 410 350
+ p 2 +
+ w 33
j 560 260
+ p 3 1
+ w 33
j 610 390
+ s 5
+ w 7
j 600 260
+ p 3 2
+ w 7
j 600 290
+ p 46 2
+ w 7
j 610 290
+ w 7
+ w 7
j 600 320
+ p 47 2
+ w 7
j 610 320
+ w 7
+ w 7
j 440 350
+ p 14 +
+ w 55
j 470 350
+ p 15 +
+ w 59
j 540 260
+ p 62 pin1
+ w 33
j 560 320
+ p 47 1
+ w 59
j 540 320
+ p 66 pin1
+ w 59
j 560 290
+ p 46 1
+ w 55
j 540 290
+ p 64 pin1
+ w 55
j 510 260
+ p 102 pin1
+ w 33
j 510 320
+ p 104 pin1
+ w 59
j 450 260
+ p 68 pin1
+ w 33
j 450 290
+ p 70 pin1
+ w 55
j 490 320
+ p 72 pin1
+ w 59
j 480 290
+ p 74 pin1
+ w 55
@attributes
a 0 s 0:13 0 0 0 hln 100 PAGETITLE=
a 0 s 0:13 0 0 0 hln 100 PAGENO=1
a 0 s 0:13 0 0 0 hln 100 PAGESIZE=B
a 0 s 0:13 0 0 0 hln 100 PAGECOUNT=1
@graphics
