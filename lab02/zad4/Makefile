gen_dump_static:
	make -C ../zad3 clean
	make -C ../zad3 client_static
	objdump -d ../zad3/program > zrzut_static.txt

gen_dump_shared:
	make -C ../zad3 clean
	make -C ../zad3 client_shared
	objdump -d ../zad3/program > zrzut_shared.txt

gen_diff:
	make gen_dump_static
	make gen_dump_shared
	diff zrzut_static.txt zrzut_shared.txt > roznice.txt

