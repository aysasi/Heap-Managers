linked:
	echo "---- Linked Implementation ----"
	rm -f ./gawk-3.1.8/malloc_imp.c
	cp ./Linked/linked_list.c ./gawk-3.1.8/malloc_imp.c
	cd ./gawk-3.1.8/ && $(MAKE) check

buddy:
	echo "---- Buddy System Implementation"
	rm -f ./gawk-3.1.8/malloc_imp.c
	cp ./Buddy/buddy_imp.c ./gawk-3.1.8/malloc_imp.c
	cd ./gawk-3.1.8/ && $(MAKE) check
