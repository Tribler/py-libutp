
import ctypes
import sys
from .config import Config


# load the c library
cutplib = ctypes.cdll.LoadLibrary(Config.UTPWRAPPER_LIB)


def start():

    if len(sys.argv) > 1 and sys.argv[1] == "client":
        cutplib.client()
    else:
        cutplib.serve()
