hardware_pin=       [11,7,4,2,1,10,5,3]
hardware_pin_ledbit=[0 ,1,2,3,4, 5,6,7]
#                    low             DP
hard_dict=dict(zip(hardware_pin,hardware_pin_ledbit))
match="""
0
1
2
3
4
5
6
7
8
9
A
b
C
d
E
F
.
P
U
T
Y
8.
N
"""
ori="""0xC0
0xF9
0xA4
0xB0
0x99
0x92
0x82
0xF8
0x80
0x90
0x88
0x83
0xC6
0xA1
0x86
0x8E
0x7F
0x82
0xC1
0xCE
0x91
0x00
0xFF
"""
cache=[]
#                   P1.7            P1.0
format_of_new_pos_pin=[1,2,3,4,5,10,7,11]
ori=ori.split()
match=match.split()
def hextobin(s):
    _cache=int(s,16)
    return bin(_cache)[2:]
for i in ori:
    highhalf=i[2:]    
    cache.append(hextobin(highhalf))
    
def check_len(i):
    l=len(i)
    if(l!=8):
        return '0'*(8-l)+i
    else:
        return i
def reformat(i):
    oriposition=list(i)
    oriposition.reverse()
    newpos=''
    for e in format_of_new_pos_pin:
        newpos=newpos+oriposition[hard_dict[e]]
    return newpos
data_ori=list(map(check_len, cache))#original binary code
data_new=list(map(reformat, data_ori))
data_new_in_hex=list(map(lambda x:hex(int(x,2)),data_new))
#print(data_new)

#               hex format ,//LED Graph
format_of_C_code="%s,//%s"
outputpair=list(zip(data_new_in_hex,match))
for i in outputpair:
    print(format_of_C_code%i)