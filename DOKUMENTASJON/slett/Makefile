kompilerAlt: *tex rapport.bbl bibliografi.bib
	latex rapport.tex
	bibtex rapport
	latex rapport.tex
	latex rapport.tex

opneMedEnGang__dvi: rapport.tex rapport.aux bibliografi.bib
	make kompilerAlt
	xdvi rapport.dvi

opneMedEnGang__pdf: rapport.tex rapport.aux bibliografi.bib
	make rapport.pdf 
	evince rapport.pdf

rapport.pdf:  rapport.tex rapport.bbl  bibliografi.bib
		bibtex rapport
		pdflatex rapport.tex
	    pdflatex rapport.tex

rapport.bbl:  bibliografi.bib 
		pdflatex rapport.tex
	    bibtex rapport

c: 
	rm rapport.pdf rapport.aux
	make opneMedEnGang__heil
