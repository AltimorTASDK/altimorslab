import os
import png
import shutil
import struct
import sys

IN_DIR = "resources"
OUT_DIR = "bin/files/lab"

GX_TF_RGBA8 = 6

def encode_png(in_path, out_path):
    with open(in_path, "rb") as in_file:
        reader = png.Reader(file=open(in_path, "rb"))
        width, height, [*data], info = reader.asRGBA8()
        
    with open(out_path, "wb") as out_file:
        # Write header with 32 byte aligned size
        header = struct.pack(">HHB", width, height, GX_TF_RGBA8)
        header_size = (len(header) + 31) & ~31;
        header += b"\x00" * (header_size - len(header))
        out_file.write(header)
        
        # Write in 4x4 pixel blocks of format:
        # ARARARARARARARAR
        # ARARARARARARARAR
        # GBGBGBGBGBGBGBGB
        # GBGBGBGBGBGBGBGB
        for block_y in range(0, height, 4):
            for block_x in range(0, width, 4):
                for offset_y in range(0, 4):
                    for offset_x in range(0, 4):
                        x = block_x + offset_x
                        y = block_y + offset_y
                        out_file.write(bytes([data[y][x * 4 + 3]])) # alpha
                        out_file.write(bytes([data[y][x * 4]]))     # red

                for offset_y in range(0, 4):
                    for offset_x in range(0, 4):
                        x = block_x + offset_x
                        y = block_y + offset_y
                        out_file.write(bytes([data[y][x * 4 + 1]])) # green
                        out_file.write(bytes([data[y][x * 4 + 2]])) # blue

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