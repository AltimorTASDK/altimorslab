import os
import png
import shutil
import struct
import sys

IN_DIR = "resources"
OUT_DIR = "bin/files/lab"

GX_RGB8 = 1
GX_RGBA8 = 5

def encode_png(in_path, out_path):
    with open(in_path, "rb") as in_file:
        reader = png.Reader(file=open(in_path, "rb"))
        _, _, _, info = reader.asDirect()

        reader = png.Reader(file=open(in_path, "rb"))
        if info['alpha']:
            fmt = GX_RGBA8
            width, height, [*data], info = reader.asRGBA8()
        else:
            fmt = GX_RGB8
            width, height, [*data], info = reader.asRGB8()
        
    with open(out_path, "wb") as out_file:
        out_file.write(struct.pack("<HHB", width, height, fmt))
        out_file.write(b"".join(data))

def main():
    if len(sys.argv) < 3:
        print("Usage: copy_resource.py <in> <out>", file=sys.stderr)
        sys.exit(1)

    in_path = sys.argv[1]
    out_path = sys.argv[2]
    out_dir = os.path.dirname(out_path)

    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    if in_path.endswith(".png"):
        encode_png(in_path, out_path)
    else:
        shutil.copy(in_path, out_path)

if __name__ == "__main__":
    main()