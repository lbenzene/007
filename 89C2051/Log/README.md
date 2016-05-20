#Logging for Code and Design#

##Hardware##
- NE555:Not finished × wait for **debug**....
- LEDdisplay:line repaired,scanning strategy confirmed.√
- Beeper:9012BJT drived,√

##MidInterface##
PortDeclear

	1. P1 LEDData
		- P3.4 LED bit High
		- P3.5         Mid
		- P3.7         Low

	2. Music Port
		- P3.0
	3. NE555 SquareWave
		- P3.2
	4. KeyPress
		- P3.3
##Software##
- 2016-05-19 :when compiler meet with huge constants multiplying or dividing,the target object ASM code appears strange commandlines...
- 2016-05-20 :OutputChannel has finished,but square detector function needed rewrite.