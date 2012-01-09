CFLAGS=-g3 -O1 -Wall -I/usr/include/c++/4.5 -I/usr/include/c++/4.5/x86_64-linux-gnu #-pthread
CPP=clang++
#CPP=g++
CPPFLAGS=${CFLAGS} 


#all: andreKildefiler/main.cpp andreKildefiler/main.h andreKildefiler/tid.cpp andreKildefiler/time.h neuroElements/axon.h neuroElements/dendrite.h neuroElements/synapse.h neuroElements/neuroElement.cpp
#	clear; echo "make all:\n\n"; g++ ${CFLAGS} andreKildefiler/main.cpp neuroElements/neuroElement.cpp -o auroNett.out


altI_en: andreKildefiler/*.cpp andreKildefiler/*.h neuroElements/*.cpp neuroElements/*.h
	echo "\n\n\n\nmake altI_en:\n\n"
	${CPP} ${CFLAGS} andreKildefiler/main.cpp neuroElements/neuroElement.cpp -o auroNett.out 

kjoerOgPlott: altI_en
	make c && sh kjør.sh -r100 -n1.5 && octave sammenligningKmedFileneFraDatafilesForEvaluation.m

altI_enGCC: andreKildefiler/*.cpp andreKildefiler/*.h neuroElements/*.cpp neuroElements/*.h
	echo "\n\n\n\nmake altI_en: i GCC\n\n"
	g++ ${CFLAGS} andreKildefiler/main.cpp neuroElements/neuroElement.cpp -o auroNett.out 

auroNett.out: main.o neuroElement.o
	echo "\n\n\n\nmake auroNett.out:\n\n"
	${CPP} ${CFLAGS} main.o neuroElement.o -o auroNett.out

neuroElement.o: andreKildefiler/time.h neuroElements/axon.h neuroElements/dendrite.h neuroElements/synapse.h neuroElements/neuroElement.cpp
	${CPP} ${CFLAGS} -c neuroElements/neuroElement.cpp -o neuroElement.o

main.o: andreKildefiler/main.cpp andreKildefiler/main.h andreKildefiler/time.h
	${CPP} ${CFLAGS} -c andreKildefiler/main.cpp 



c:
	make clean; clear; echo "\nHAR FJÆNA ALLE .out FILENE. Kjører make\n\n"; make


clean:
	rm -f *.out *.o

# *storbox og *litabox (med stjerne på slutten) lager dette, etter mi .vimrc . (med stjerne på slutten og.
