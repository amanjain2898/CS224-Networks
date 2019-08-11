from matplotlib import pyplot as plt
from matplotlib.patches import Rectangle
import time
import copy

def DisplayColor(clr):
	plt.figure()
	mng = plt.get_current_fig_manager()
	mng.resize(*mng.window.maxsize())
	currentAxis = plt.gca()
	currentAxis.add_patch(Rectangle((0, 0), 5, 5, facecolor=clr))
	plt.show(block = False)
	time.sleep(5)
	plt.close()

def xor(a, b):
	l = len(a)
	res = ""
	for i in range(0, len(a)):
		if(a[i] == b[i]):
			res += "0"
		else:
			res += "1"
	return res

def divide(dividend, divisor):
	remainder = copy.deepcopy(dividend)
	div = copy.deepcopy(divisor).ljust(len(dividend), '0')
	while len(remainder.lstrip('0')) >= len(divisor):
		remainder = xor(remainder, div)
		i = 0
		remainder = remainder.lstrip('0')
		div =  copy.deepcopy(divisor).ljust(len(remainder), '0')
	return remainder.rjust(len(dividend),'0')

def decoding(str,divisor):
	dividend = ""
	n = len(str)
	k = len(divisor)
	for i in range(0,n):
		if str[i] == "r" or str[i] == "R":
			dividend+="0"
		elif str[i] == "b" or str[i] == "B": dividend+="1"
	remainder = divide(dividend,divisor)
	if (remainder.lstrip('0') == ""):
		msg = dividend[:(len(dividend)-k+1)]
		print(msg)
		return True
	else: return False

dividend = input("Enter the codeword: ")
divisor = input("Enter the crc polynomial: ")

if decoding(dividend,divisor):
	DisplayColor("green")
else: DisplayColor("yellow")