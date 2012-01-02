kompilerAlt: *tex 
	latex DOKUMENTASJON.tex
	latex DOKUMENTASJON.tex

opneMedEnGang__dvi: rapport.tex rapport.aux bibliografi.bib
	make kompilerAlt
	xdvi DOKUMENTASJON.dvi

c: 
	rm rapport.pdf rapport.aux
	make opneMedEnGang__heil
