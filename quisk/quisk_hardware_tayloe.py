# Quisk hardware file for simple Tayloe-based SDR receiver.
#
# For serial.serial, use things like "COM10" on windows and "/dev/ttyACM0"
# on Linux

from __future__ import absolute_import
import serial

# Looks like this is set from the quisk config GUI.
#sample_rate = 96000

from quisk_hardware_model import Hardware as BaseHardware

class Hardware(BaseHardware):
  def __init__(self, app, conf):
    BaseHardware.__init__(self, app, conf)
    try:
       self.sp = serial.Serial("/dev/ttyACM0",115200,timeout=2)
    except:
       print("serial port open failed")

    self.first = True
    self.vfo = self.conf.fixed_vfo_freq		# Fixed VFO frequency in Hertz
    #self.tune = self.vfo + 10000		# Current tuning frequency in Hertz
    self.tune = 1300000
    print("init complete")

  def ChangeFrequency(self, tune, vfo, source='', band='', event=None):
    # Change and return the tuning and VFO frequency.  See quisk_hardware_model.py.
    self.tune = tune
    print(tune)
    if (tune>(self.vfo + sample_rate/2) or tune<(self.vfo - sample_rate/2)):
      self.vfo = tune - 10000
      str_to_send = str(self.vfo) + "\r"
      self.sp.write(str_to_send.encode(encoding='ascii'))
      resp = self.sp.readline()
      self.vfo = int(resp)
      print("new vfo: ", self.vfo)
    return tune, self.vfo

   # def ReturnFrequency(self):
   #  if self.first == True:
   #    print("Initial tune: ", self.tune, " vfo: ", self.vfo)
   #    self.first = False
   #    return self.tune, self.vfo
   #  else:
   #    return None, None
