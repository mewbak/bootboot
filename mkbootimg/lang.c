/*
 * mkbootimg/lang.c
 *
 * Copyright (C) 2017 - 2021 bzt (bztsrc@gitlab)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * This file is part of the BOOTBOOT Protocol package.
 * @brief Multilanguage support
 *
 */

#include "lang.h"

/**
 * Translations
 */
char *dict[NUMLANGS][NUMTEXTS + 1] = {
    {
        "en",

        "memory allocation error",
        "unable to read initrd image",
        "initrd not specified in json",
        "initrd type not specified in json",
        "invalid initrd type",
        "initrd-only type",
        "Accepted values",
        "unable to read BOOTBOOT configuration from",
        "BOOTBOOT configuration file is bigger than 4095 bytes",
        "partitions array or boot partition's type not specified in json",
        "boot partition's size not specified in json",
        "invalid architecture in kernel",
        "more than one loadable segment in kernel",
        "entry point is not in text segment",
        "invalid kernel executable format. ELF64 or PE32+ only",
        "not in the higher half top -1G",
        "not %d bytes aligned",
        "not page aligned",
        "address collusion",
        "invalid size",
        "segment is bigger than 16M",
        "unable to write",
        "unable to locate kernel",
        "unable to read kernel from",
        "unable to read configuration json from",
        "doesn't have a valid type",
        "or any non-zero GUID in the form",
        "doesn't have a name",
        "unable to read partition image",
        "stage2 is not 2048 byte sector aligned",
        "constructed file system is bigger than partition size",
        "size must be specified",
        "file too big",
        "too many entries in directory",
        "must use valid static addresses",
        "valid dynamic addresses",
        "Validates ELF or PE executables for being BOOTBOOT compatible, otherwise",
        "creates a bootable hybrid image or Option ROM image for your hobby OS",
        "Usage",
        "configuration json",
        "output disk image name",
        "Examples",
        "writing",
        "saved"
    },
    {
        "hu",

        "mem??ria foglal??si hiba",
        "nem lehet az initrd k??pet beolvasni",
        "initrd nincs megadva a json-ben",
        "initrd type nincs megadva a json-ben",
        "??rv??nytelen initrd type",
        "csak initrd-n??l haszn??lhat?? type",
        "Lehets??ges ??rt??kek",
        "nem tudom beolvasni a BOOTBOOT konfigur??ci??t innen",
        "a BOOTBOOT konfigur??ci?? t??bb, mint 4095 b??jt",
        "partitions t??mb vagy a boot part??ci?? type-ja nincs megadva a json-ben",
        "a boot part??ci?? m??rete (size) nincs megadva a json-ben",
        "??rv??nytelen kernel architekt??ra",
        "t??bb, mint egy bet??ltend?? kernel szegmens",
        "bel??p??si pont nem a k??dszegmensen bel??lre mutat",
        "??rv??nytelen kernel futtathat??. ELF64 vagy PE32+ lehet csak",
        "nincs a fels?? mem??ria -1G tartom??ny??ban",
        "nem %d b??jtra igaz??tott",
        "nincs laphat??rra igaz??tva",
        "c??m??tk??z??s",
        "hib??s m??ret",
        "szegmens nagyobb, mint 16M",
        "nem tudom ??rni",
        "nem tal??lom a kernelt benne",
        "nem tudom bet??lteni a kernelt innen",
        "nem tudom beolvasni a konfigur??ci??s json-t innen",
        "nincs ??rv??nyes type t??pusa",
        "vagy b??rmilyen nem csupa nulla GUID ilyen form??tumban",
        "nincs n??v megadva a name-ben",
        "nem tudom a part??ci??s k??pet beolvasni",
        "stage2 nincs 2048 b??jtos szektorhat??rra igaz??tva",
        "az ??ssze??ll??tott f??jlrendszer nagyobb, mint a megadott part??ci?? m??ret a size-ban",
        "a m??retet k??telez?? megadni a size-ban",
        "t??l nagy f??jl",
        "t??l sok k??nyvt??rbejegyz??s",
        "helyes statikus c??meket kell haszn??lnia",
        "??rv??nyes dinamikus c??mek",
        "Ellen??rzi, hogy az ELF vagy PE futtathat?? BOOTBOOT kompat??bilis-e, illetve",
        "hibrid ind??t?? lemez k??pet vagy Option ROM k??pet gener??l a hobbi OS-edhez",
        "Haszn??lat",
        "konfigur??ci??s json",
        "kimeneti lemezk??p neve",
        "P??ld??k",
        "ki??r??s",
        "lementve"
    },
    {
        "pl",

        "b????d alokacji pami??ci",
        "nie uda??o si?? za??adowa?? initrd",
        "initrd nie jest opisany w pliku json",
        "typ initrd nie jest opisany w pliku json",
        "niepoprawny typ initrd",
        "nie ma poprawnego typu",
        "Akceptowane warto??ci",
        "nie uda??o si?? za??adowa?? konfiguracji BOOTBOOT z",
        "plik z konfiguracj?? BOOTBOOT jest wi??kszy ni?? 4095 bajt??w",
        "tablica partycji lub boot-owalna partycja nie jest opisana w pliku json",
        "wielko???? boot-owalnej partycji nie jest opisana w pliku json",
        "nieprawid??owa architektura w j??drze",
        "wi??cej ni?? jeden segment do za??adowania w j??drze",
        "entry point nie znajduje si?? w segmencie \"text\"",
        "niepoprawny format j??dra. Poprawnymi s?? ELF64 i PE32+",
        "nie znajduje si?? w \"higher half top -1G\"",
        "brak wyr??wnania do %d bajt??w",
        "brak wyr??wnania do stron",
        "zmowa adresowa",
        "nieprawid??owy rozmiar",
        "segment jest wi??kszy ni?? 16Mb",
        "nie mo??e zapisa??",
        "nie mo??e znale???? j??dra",
        "nie mo??e za??adowa?? j??dra z",
        "nie mo??e za??adowa?? pliku konfiguracyjnego json z",
        "nie ma poprawnego typu",
        "lub niewyzerowanego GUID w formacie",
        "nie ma nazwy",
        "nie mo??e za??adowa?? obrazu partycji",
        "etap2 nie jest wyr??wnany do 2048 bajt??w",
        "zkonstruowany system plik??w jest wi??kszy ni?? rozmiar partycji",
        "rozmiar musi by?? okre??lony",
        "plik jest za du??y",
        "za du??o wpis??w w folderze",
        "musi u??ywa?? poprawnych statycznych adres??w",
        "poprawny dynamiczny adres",
        "Sprawdza czy plik wykonywalny ELF lub PE jest kompatybilny z BOOTBOOT, w przeciwnym wypadku",
        "tworzy boot-owalny obraz hybrydoway lub \"Option ROM image\" z twoim hobbistycznym OS",
        "Sposoby u??ytkowania",
        "plik konfiguracyjny json",
        "nazwa wyj??ciowego pliku obrazu dysku",
        "Przyk??adowo",
        "zapisywanie",
        "zapisane"
    }
};
