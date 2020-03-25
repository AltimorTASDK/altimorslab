SECTION_OFFSET_START = 0x0
SECTION_ADDRESS_START = 0x48
SECTION_SIZE_START = 0x90

BSS_START = 0xD8
BSS_SIZE = 0xDC

TEXT_SECTION_COUNT = 7
DATA_SECTION_COUNT = 11
SECTION_COUNT = TEXT_SECTION_COUNT + DATA_SECTION_COUNT

PATCH_SECTION = 3

ORIGINAL_DOL_END = 0x804DEC00

def word(data, offset):
    return sum(data[offset + i] << (24 - i * 8) for i in range(4))

def word_to_bytes(word):
    return bytes((word >> (24 - i * 8)) & 0xFF for i in range(4))

def get_dol_end(data):
    def get_section_end(index):
        address = word(data, SECTION_ADDRESS_START + index * 4)
        size = word(data, SECTION_SIZE_START + index * 4)
        return address + size

    
    bss_end = word(data, BSS_START) + word(data, BSS_SIZE)
    return max(bss_end, *(get_section_end(i) for i in range(SECTION_COUNT)))

def address_to_offset(data, value):
    for i in range(0, SECTION_COUNT):
        address = word(data, SECTION_ADDRESS_START + i * 4)
        size = word(data, SECTION_SIZE_START + i * 4)
        if address <= value < address + size:
            offset = word(data, SECTION_OFFSET_START + i * 4)
            return value - address + offset

def patch_load_imm32(data, address, reg, imm):
    lis = 0x3C000000 | (reg << 21) | (imm >> 16)
    ori = 0x60000000 | (reg << 21) | (reg << 16) | (imm & 0xFFFF)
    offset = address_to_offset(data, address)
    data[offset:offset+4] = word_to_bytes(lis)
    data[offset+4:offset+8] = word_to_bytes(ori)

def patch_load_imm32_split(data, lis_addr, ori_addr, reg, imm):
    lis = 0x3C000000 | (reg << 21) | (imm >> 16)
    ori = 0x60000000 | (reg << 21) | (reg << 16) | (imm & 0xFFFF)
    lis_offset = address_to_offset(data, lis_addr)
    ori_offset = address_to_offset(data, ori_addr)
    data[lis_offset:lis_offset+4] = word_to_bytes(lis)
    data[ori_offset:ori_offset+4] = word_to_bytes(ori)

def patch_branch(data, address, target):
    delta = target - address
    if delta < 0:
        # Two's complement
        delta = ~(-delta) + 1

    offset = address_to_offset(data, address)
    data[offset:offset+4] = word_to_bytes(0x48000000 | (delta & 0x3FFFFFC))

def patch_heap(data):
    delta = get_dol_end(data) - ORIGINAL_DOL_END
    print(f"DOL virtual size delta: 0x{delta:X} bytes")

    patch_load_imm32(data, 0x80343094, 3, 0x804F0C00 + delta)
    patch_load_imm32(data, 0x803430CC, 3, 0x804EEC00 + delta)

    patch_load_imm32(data, 0x8034AC78, 0, 0x804EEC00 + delta)
    patch_load_imm32_split(data, 0x8034AC80, 0x8034AC88, 0, 0x804DEC00 + delta)

    patch_load_imm32_split(data, 0x802256D0, 0x802256D8, 3, 0x804DEC00 + delta)
    patch_load_imm32_split(data, 0x8022570C, 0x80225714, 4, 0x804EEC00 + delta)
    patch_load_imm32_split(data, 0x80225718, 0x80225720, 5, 0x804DEC00 + delta)

def apply_hooks(data):
    hooks_offset = word(data, SECTION_OFFSET_START + PATCH_SECTION * 4)
    hooks_address = word(data, SECTION_ADDRESS_START + PATCH_SECTION * 4)
    hooks_size = word(data, SECTION_SIZE_START + PATCH_SECTION * 4)

    for i in range(hooks_size // 8):
        offset = hooks_offset + i * 8

        original = word(data, offset)
        hook = word(data, offset + 4)

        # Replace the patch data with the overwritten instruction + branch back
        # to the original
        original_offset = address_to_offset(data, original)
        data[offset:offset+4] = data[original_offset:original_offset+4]
        patch_branch(data, hooks_address + i * 8 + 4, original + 4)

        patch_branch(data, original, hook)
        print(f"Hook {original:08X} -> {hook:08X}")

def apply_extra_patches(data):
    with open("patches") as f:
        while True:
            line = f.readline()
            if not line:
                return

            if line.find("#") != -1:
                line = line[:line.find("#")]

            if len(line.split()) == 0:
                continue

            address, word = [int(x, 16) for x in line.split()]
            offset = address_to_offset(data, address)
            data[offset:offset+4] = word_to_bytes(word)
            print(f"Patch {address:08X} -> {word:08X}")

def main():
    with open("bin/sys/main.dol", "rb") as f:
        data = bytearray(f.read())

    patch_heap(data)
    apply_hooks(data)
    apply_extra_patches(data)

    with open("bin/sys/main.dol", "wb") as f:
        f.write(data)

if __name__ == "__main__":
    main()