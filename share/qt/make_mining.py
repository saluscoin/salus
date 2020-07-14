#!/usr/bin/env python
# W.J. van der Laan, 2011
# Make frame based .mng animation from a .png
# Requires imagemagick 6.7+
from __future__ import division
from os import path
from PIL import Image
from subprocess import Popen

SRCPATH='../../src/qt/res/icons/'
SRC=['mining_active.png', 'mining_active2.png', 'mining_active3.png', 'mining_active4.png']
DST='../../src/qt/res/movies/mining.mng'
TMPDIR='/tmp'
TMPNAME='tmp-%03i.png'
FRAMERATE=10.0
CONVERT='convert'
DSIZE=(32,32)

def frame_to_filename(frame):
    return path.join(TMPDIR, frame)

frame_files = []
for frame in SRC:
    im_src = Image.open(path.join(SRCPATH, frame))
    im_src.thumbnail(DSIZE, Image.ANTIALIAS)
    outfile = frame_to_filename(frame)
    im_src.save(outfile, 'png')
    frame_files.append(outfile)

p = Popen([CONVERT, "-delay", str(FRAMERATE), "-dispose", "2"] + frame_files + [DST])
p.communicate()



