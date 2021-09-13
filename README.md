# Altimor's Lab

WIP Melee labbing mod. Automatically appends code to Melee's DOL using a linker
script. Also generates a linker script to define all symbols from GALE01.map.

src/hooks.S allows function hooks to be applied automatically. patch_dol.py
reads pairs of words containing the original function and hook addresses from
the generated section, creates a branch from the original, and replaces each
hook descriptor with the overwritten instruction and a branch to after the
overwrite so it can be used to call the original function.

## Shoutouts

* UnclePunch
* psiLupan
* taukhan
* Hax fucking $