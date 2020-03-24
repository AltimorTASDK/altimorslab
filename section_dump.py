SECTION_OFFSET_START = 0x0
SECTION_ADDRESS_START = 0x48
SECTION_SIZE_START = 0x90

TEXT_SECTION_COUNT = 7
DATA_SECTION_COUNT = 11
SECTION_COUNT = TEXT_SECTION_COUNT + DATA_SECTION_COUNT

def word(data, offset):
    return sum(data[offset + i] << (24 - i * 8) for i in range(4))

def dump(data):
    for i in range(SECTION_COUNT):
        offset = word(data, SECTION_OFFSET_START + i * 4)
        if offset == 0:
            continue

        address = word(data, SECTION_ADDRESS_START + i * 4)
        size = word(data, SECTION_SIZE_START + i * 4)

        if i < TEXT_SECTION_COUNT:
            name = f"text{i}"
        else:
            name = f"data{i - TEXT_SECTION_COUNT}"

        with open(f"sections/{name}", "wb") as f:
            f.write(data[offset:offset+size])

        with open(f"sections/{name}.S", "w") as f:
            print(f"{name} 0x{address:08X} size 0x{size:08X} offset 0x{offset:08X}")
            f.write(f".section .orig{name}\n")
            f.write(f".incbin \"sections/{name}\"\n")

def main():
    with open("boot.dol", "rb") as f:
        dump(f.read())

if __name__ == "__main__":
    main()