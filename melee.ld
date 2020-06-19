/*
 * Appends to the Melee DOL
 */

OUTPUT_FORMAT(binary);
OUTPUT_ARCH(powerpc:common);
EXTERN(start);
ENTRY(start);

INCLUDE GALE01.ld

SECTIONS
{
    load_base = 0x80003000;

    text0_base = 0x80003100;
    text1_base = 0x80005940;
    data0_base = 0x80005520;
    data1_base = 0x800056C0;
    data2_base = 0x803B7240;
    data3_base = 0x803B7260;
    data4_base = 0x803B7280;
    data5_base = 0x803B9840;
    data6_base = 0x804D36A0;
    data7_base = 0x804D79E0;

    text0_offset = 0x00000100;
    text1_offset = 0x00002520;
    data0_offset = 0x003B3E20;
    data1_offset = 0x003B3FC0;
    data2_offset = 0x003B4240;
    data3_offset = 0x003B4260;
    data4_offset = 0x003B4280;
    data5_offset = 0x003B6840;
    data6_offset = 0x0042E6C0;
    data7_offset = 0x004313C0;

    /* DOL header (from TITANIK's GC docs)
    */
    .header load_base : AT(0)
    {
        /*    0000-001B  Text[0..6] sections File Positions */
        LONG(text0_offset);
        LONG(text1_offset);
        LONG(text2_offset);
        LONG(hooks_offset);
        LONG(0);
        LONG(0);
        LONG(0);

        /*    001C-0047  Data[0..10] sections File Positions */
        LONG(data0_offset);
        LONG(data1_offset);
        LONG(data2_offset);
        LONG(data3_offset);
        LONG(data4_offset);
        LONG(data5_offset);
        LONG(data6_offset);
        LONG(data7_offset);
        LONG(data8_offset);
        LONG(0);
        LONG(0);

        /*    0048-0063  Text[0..6] sections Mem Address */
        LONG(text0_base);
        LONG(text1_base);
        LONG(text2_base);
        LONG(hooks_base);
        LONG(0);
        LONG(0);
        LONG(0);

        /*    0064-008F  Data[0..10] sections Mem Address */
        LONG(data0_base);
        LONG(data1_base);
        LONG(data2_base);
        LONG(data3_base);
        LONG(data4_base);
        LONG(data5_base);
        LONG(data6_base);
        LONG(data7_base);
        LONG(data8_base);
        LONG(0);
        LONG(0);

        /*    0090-00AB  Text[0..6] sections Sizes */
        LONG(SIZEOF(.text0));
        LONG(SIZEOF(.text1));
        LONG(SIZEOF(.text2));
        LONG(SIZEOF(.hooks));
        LONG(0);
        LONG(0);
        LONG(0);

        /*    00AC-00D7  Data[0..10] sections Sizes */
        LONG(SIZEOF(.data0));
        LONG(SIZEOF(.data1));
        LONG(SIZEOF(.data2));
        LONG(SIZEOF(.data3));
        LONG(SIZEOF(.data4));
        LONG(SIZEOF(.data5));
        LONG(SIZEOF(.data6));
        LONG(SIZEOF(.data7));
        LONG(SIZEOF(.data8));
        LONG(0);
        LONG(0);

        /*    00D8  BSS Mem address
         *    00DC  BSS Size */
        LONG(bss_base);
        LONG(SIZEOF(.bss));

        /*    00E0  Entry Point */
        LONG(ABSOLUTE(start));
    }

    /* original sections */
    .text0 text0_base : AT(text0_offset) { obj/text0.o }
    .data0 data0_base : AT(data0_offset) { obj/data0.o }
    .data1 data1_base : AT(data1_offset) { obj/data1.o }
    .text1 text1_base : AT(text1_offset) { obj/text1.o }
    .data2 data2_base : AT(data2_offset) { obj/data2.o }
    .data3 data3_base : AT(data3_offset) { obj/data3.o }
    .data4 data4_base : AT(data4_offset) { obj/data4.o }
    .data5 data5_base : AT(data5_offset) { obj/data5.o }
    .data6 data6_base : AT(data6_offset) { obj/data6.o }
    .data7 data7_base : AT(data7_offset) { obj/data7.o }

    /* new sections */
    text2_base = data7_base + SIZEOF(.data7);
    text2_offset = data7_offset + SIZEOF(.data7);
    .text2 text2_base : AT(text2_offset) { *(.load* .text*) }

    hooks_base = text2_base + SIZEOF(.text2);
    hooks_offset = text2_offset + SIZEOF(.text2);
    .hooks hooks_base : AT(hooks_offset) { obj/hooks.o }
    
    data8_base = hooks_base + SIZEOF(.hooks);
    data8_offset = hooks_offset + SIZEOF(.hooks);
    .data8 data8_base : AT(data8_offset) { *(.rodata* .rdata* .data* .sdata*) }

    bss_base = data8_base + SIZEOF(.data8);
    bss_offset = data8_base + SIZEOF(.data8);
    .bss bss_base : AT(bss_offset) { *(.bss* .sbss* COMMON) }

    /DISCARD/ : { *(*) }
}