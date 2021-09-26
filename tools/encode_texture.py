import math
import os
import png
import shutil
import struct
import sys
from itertools import product

IN_DIR = "resources"
OUT_DIR = "bin/files/lab"

GX_TF_IA4 = 2
GX_TF_RGBA8 = 6

RED = 0
GREEN = 1
BLUE = 2
ALPHA = 3

FORMAT_DICT = {
    'ia4': GX_TF_IA4,
    'rgba8': GX_TF_RGBA8
}

def block_range(x, y, step=1):
    if isinstance(step, tuple):
        x = math.ceil(x / step[0]) * step[0]
        y = math.ceil(y / step[1]) * step[1]
        ranges = (range(0, y, step[1]), range(0, x, step[0]))
    else:
        x = math.ceil(x / step) * step
        y = math.ceil(y / step) * step
        ranges = (range(0, y, step), range(0, x, step))

    return ((x, y) for y, x in product(*ranges))
    
def encode_rgba8(out_file, get_pixel, width, height):
    """
    Write in 4x4 pixel blocks of format:
    ARARARARARARARAR
    ARARARARARARARAR
    GBGBGBGBGBGBGBGB
    GBGBGBGBGBGBGBGB
    """
    for block_x, block_y in block_range(width, height, 4):
        for offset_x, offset_y in block_range(4, 4):
            x = block_x + offset_x
            y = block_y + offset_y
            out_file.write(bytes([get_pixel(x, y, ALPHA)]))
            out_file.write(bytes([get_pixel(x, y, RED)]))

        for offset_x, offset_y in block_range(4, 4):
            x = block_x + offset_x
            y = block_y + offset_y
            out_file.write(bytes([get_pixel(x, y, GREEN)]))
            out_file.write(bytes([get_pixel(x, y, BLUE)]))

def encode_ia4(out_file, get_pixel, width, height):
    """
    Write 8x4 pixel blocks with lower nibble as intensity, upper nibble as alpha
    Use red channel as intensity
    """
    for block_x , block_y in block_range(width, height, (8, 4)):
        for offset_x, offset_y in block_range(8, 4):
            x = block_x + offset_x
            y = block_y + offset_y
            red   = get_pixel(x, y, RED)   >> 4
            alpha = get_pixel(x, y, ALPHA) >> 4
            out_file.write(bytes([(alpha << 4) | red]))

def encode_png(in_path, out_path, fmt):
    with open(in_path, "rb") as in_file:
        reader = png.Reader(file=open(in_path, "rb"))
        width, height, [*data], info = reader.asRGBA8()
        
    with open(out_path, "wb") as out_file:
        # Write header with 32 byte aligned size
        header = struct.pack(">HHB", width, height, fmt)
        header_size = (len(header) + 31) & ~31;
        header += b"\x00" * (header_size - len(header))
        out_file.write(header)

        def get_pixel(x, y, channel):
            if x >= width or y >= height:
                return 0
            return data[y][x * 4 + channel]
        
        { # Call corresponding encoding function
            GX_TF_RGBA8: encode_rgba8,
            GX_TF_IA4:   encode_ia4
        }[fmt](out_file, get_pixel, width, height)
        

def main():
    if len(sys.argv) < 3:
        print("Usage: encode_texture.py <in> <out>", file=sys.stderr)
        sys.exit(1)

    in_path = sys.argv[1]
    out_path = sys.argv[2]
    out_dir = os.path.dirname(out_path)

    if not os.path.exists(out_dir):
        os.makedirs(out_dir)
        
    # Get format name e.g. texture.rgba8.png, default RGBA8
    name = os.path.basename(in_path)
    components = name.split(".")
    if len(components) == 3:
        fmt = FORMAT_DICT.get(components[-2], GX_TF_RGBA8)
    else:
        fmt = GX_TF_RGBA8

    encode_png(in_path, out_path, fmt)

if __name__ == "__main__":
    main()