from __future__ import absolute_import, division, print_function
from NuRadioReco.utilities import units
from scipy import constants
import numpy as np
import matplotlib.pyplot as plt
from scipy import signal

# Logging level
import logging
logging.basicConfig(level=logging.INFO)

data = np.genfromtxt("filter.csv",delimiter=",",skip_header=0, names=['freq','gain','phase'],dtype=None)
ff = data['freq']*units.MHz

w_low = 80*units.MHz
w_high = 500*units.MHz
b, a = signal.butter(2, w_low, 'highpass', analog=True)
w, h = signal.freqs(b,a,ff)
b1, a1 = signal.butter(10, w_high,'lowpass',analog=True)
w1, h1 = signal.freqs(b1,a1,ff)

filt = h * h1
gain = np.abs(filt)
phase = np.angle(filt)
unwrap = np.unwrap(phase)


filename = "custom_filter.csv.butterworth.txt"
file = open(filename,"w")
for x in range(len(data['freq'])):
 	to_write='%.2f, %.3f,\n'%(data['freq'][x],gain[x])
 	file.write(to_write)
file.close()

fig, (ax, ax2) = plt.subplots(1, 2)
ax.plot(ff/units.MHz, gain)
df = ff[1] - ff[0]
ax.set_xlabel("frequency [MHz]")
ax.set_ylabel("magnitude")
ax2.plot(ff/units.MHz, phase)
ax2.plot(ff/units.MHz, unwrap)
ax2.set_xlabel("frequency [MHz]")
ax2.set_ylabel("phase")
fig.tight_layout()
fig.suptitle("cust filter file")
fig.subplots_adjust(top=0.9)
#ax.set_xlim(1, 800)
fig.savefig("custom_filter_butterworth.png")


