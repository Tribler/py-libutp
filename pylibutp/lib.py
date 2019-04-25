
import ctypes
from .config import Config

# load the c library
cutplib = ctypes.cdll.LoadLibrary(Config.UTPWRAPPER_LIB)

print(cutplib.testfunc(11))
