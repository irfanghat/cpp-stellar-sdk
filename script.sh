#!/bin/bash

which xdrc
xdrc --help 2>&1 | head -20
dpkg -l | grep -i xdrpp   # or: pip show / brew list, wherever it came from

xdrc -version