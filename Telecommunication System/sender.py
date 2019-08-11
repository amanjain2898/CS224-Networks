from matplotlib import pyplot as plt
from matplotlib.patches import Rectangle
import copy
import sched, time,os
from graphics import *

def DisplayMessage(message):
	for i in range(0, len(message)):
		if message[i] == '0':
			win.setBackground("red")
			time.sleep(1)
			win.setBackground("black")
			time.sleep(0.4)
		else:
			win.setBackground("blue")
			time.sleep(1)
			win.setBackground("black")
			time.sleep(0.4)

def xor(a, b):
	l = len(a)
	res = ""
	for i in range(0, len(a)):
		if(a[i] == b[i]):
			res += "0"
		else:
			res += "1"
	return res

def Not(a):
	if (a == '0'):
		return '1'
	else: return '0'

def rightshift(str, p):
	s = ""
	zeros = ""
	n = len(str)
	for i in range(0,p):
		zeros+="0"
	temp = str[0:n-p]
	s = zeros + temp
	return s

def divide(dividend, divisor):
	remainder = copy.deepcopy(dividend)
	div = copy.deepcopy(divisor).ljust(len(dividend), '0')
	i = 0
	while len(remainder.lstrip('0')) >= len(divisor) :
		remainder = xor(remainder, div)
		if (remainder.lstrip('0') == ""): break
		while remainder[i] == '0':
			div = rightshift(div,1)
			i = i + 1
	return remainder


message = input("Enter the message : ")
print("Enter the position of errors : ",end = '')
errors = [int(x) for x in input().split()]

len_errors = len(errors)

crc = "100101"
message = message.ljust(len(message) + len(crc) - 1, '0')
encrypted_message = xor(message, divide(message, crc))

final_message = ""


for i in range(len(encrypted_message)):
	flag = False
	for j in range(len(errors)):
		if(i == errors[j]-1):
			final_message = final_message + Not(encrypted_message[i])
			flag = True
	if(flag == False): final_message = final_message + encrypted_message[i]
    

win = GraphWin('Message', 1900, 1900)
win.setBackground('white')
time.sleep(3)
DisplayMessage(final_message)
win.setBackground('white')
time.sleep(2)

