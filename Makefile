# makefile pro preklad LaTeX verze Bc. prace
# (C) 2002 David Rezac
#
# asi budete chtit prejmenovat:
CO=xmensi06

all: remote 
#all: $(CO).pdf

remote:
	./show-pdf.sh


pdf: $(CO).pdf

$(CO).ps: $(CO).dvi
	dvips $(CO)

$(CO).pdf: clean
	pdflatex $(CO)
	bibtex $(CO)
	pdflatex $(CO)
	pdflatex $(CO)

$(CO).dvi: $(CO).tex $(CO).bib
	latex $(CO)
	bibtex $(CO)
	latex $(CO)
	latex $(CO)

desky:
	latex desky
	dvips desky
	dvipdf desky

clean:
	rm -f *.dvi *.log $(CO).blg $(CO).bbl $(CO).toc *.aux $(CO).out

pack:
	tar czvf bp-xjmeno.tar.gz *.tex *.bib *.cls *.pdf *.eps \
	    Makefile ChangeLog README TODO
