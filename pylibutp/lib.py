
from __future__ import print_function
from __future__ import division
from __future__ import unicode_literals
from __future__ import with_statement
from __future__ import absolute_import
from __future__ import generators
from __future__ import nested_scopes

import ctypes
import sys
from .config import Config


# load the c library
cutplib = ctypes.cdll.LoadLibrary(Config.UTPWRAPPER_LIB)


def start():

    if len(sys.argv) > 1 and sys.argv[1] == "client":
        cutplib.client(
            ctypes.c_char_p(b"127.0.0.1"),
            ctypes.c_char_p(b"8000"),
            ctypes.c_char_p(b"hi")
        )
    else:
        cutplib.serve(b"8000")
