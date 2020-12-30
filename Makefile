all: bootboot.bin bootboot.sym refresh

bootboot.bin: bootboot.asm
	@echo "  src		x86_64-bios (MultiBoot / BIOS)"
	@cat bootboot.asm | grep -v "^public" | grep -v "format ELF64" >bb.asm
	@fasm bb.asm bootboot.bin >/dev/null
	@rm bb.asm

bootboot.sym: bootboot.asm
	@echo "  sym		bootboot.sym"
	@fasm bootboot.asm bootboot.elf >/dev/null
	@nm bootboot.elf | sort | sed 's/\ A\ /\ /g' > bootboot.sym
	@printf "fffffffff8000000 mmio\nfffffffffc000000 fb\nffffffffffe00000 bootboot\nffffffffffe01000 environment\nffffffffffe02000 _start\n" >>bootboot.sym
	@rm bootboot.elf

disk-x86.img: disk-x86.img.gz
	@gzip -d -k disk-x86.img.gz

refresh: bootboot.bin disk-x86.img
	@echo "  dd		bootboot.bin to disk-x86.img"
	@dd if=bootboot.bin of=disk-x86.img bs=1 seek=120832 conv=notrunc 2>/dev/null

clean:
	@rm bootboot.bin bootboot.sym disk-x86.img >/dev/null 2>/dev/null || true

