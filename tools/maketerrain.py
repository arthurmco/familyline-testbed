#!/usr/bin/env python

# -*- coding: utf-8 -*-

# Makes a terrain from a bump map image
# Copyright (C) 2017 Arthur M

# Parameters: maketerrain SOURCE DESTINY

import sys, os, stat, struct
from PIL import Image

if len(sys.argv) < 3:
    print('%s: converts an image (as a heightmap) to a Tribalia terrain' %
    sys.argv[0])
    print(' see docs/terrain_contrib.md for more info')
    print('Usage: %s IMG_SOURCE DESTINY\n' % sys.argv[0])
    sys.exit(1)


source = sys.argv[1]
destiny = sys.argv[2]

try:
    # Check if source exists
    st_source = os.stat(source)
except OSError as err:
    print('ERROR: while opening {0} ({1})'.format(source, err))
    sys.exit(1)

# Check if source is a file    
if stat.S_ISDIR(st_source.st_mode):
    print('ERROR: %s is a directory' % source)
    sys.exit(1)

try:
    source_img = Image.open(source)
except:
    print('ERROR: while opening {0} as an image'.format(source))
    print('Maybe this isn\'t an image :( ')
    sys.exit(0)


width = source_img.width
height = source_img.height
print(' -> {0} is a {1}x{2} image. This will be the size of the terrain'.format(
    source,width,height))

source_pixeldata = source_img.load()

# Now we open the damn file
try:
    destiny_file = open(destiny, 'wb')
    
except IOError as err:
    print('I/O error occurred while creating file {0} ({1})'.format(
        destiny, err))
except:
    print('Error happened while creating file {0}'.format(destiny))


# Write the file header.
# First, magicheader, then version, then game and then thdr_offset
destiny_file.write(struct.pack('IIII',0x42545254,1,0,16))

# Write the terrain header
# width, height, name_off, author_off, next terrain header off and data off
# Name and author are currently unimplemented.
destiny_file.write(struct.pack('IIIIII', width, height, 0, 0, 0, 40))
destiny_file.flush()
destiny_data = []

# Create the terrain data
# We're going to use only the red channel. and limit ourselves to 0..255
# height values.

for y in range(height):
    for x in range(width):
        destiny_data.append(source_pixeldata[x, y][0])


# Now we write the appropriate values
for terrainitem in destiny_data:
    destiny_file.write(struct.pack('hH', terrainitem, 0))

destiny_file.close()
source_img.close()
