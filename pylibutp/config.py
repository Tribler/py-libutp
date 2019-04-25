from packaging import version
import os


class Config:
    PATH = os.path.dirname(os.path.abspath(__file__))
    # get the lib with the newest version number
    UTPWRAPPER_LIB = os.path.join(
        PATH,
        sorted([
            i for i in os.listdir(PATH)
            if len(i.split("-")) == 3
        ], reverse=True, key=lambda i: version.parse(i.split("-")[1]))[0]
    )
