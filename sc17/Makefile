PROJECT=main
TEX=pdflatex
BIBTEX=bibtex
BUILDTEX=$(TEX) --shell-escape $(PROJECT).tex

all: 
	$(BUILDTEX)
	$(BIBTEX) $(PROJECT)
	$(BUILDTEX)
	$(BUILDTEX)
clean-all:
	rm -f *.dvi *.log *.bak *.aux *.bbl *.blg *.idx *.ps *.eps *.pdf *.toc *.out *~

clean:
	rm -f *.log *.bak *.aux *.bbl *.blg *.idx *.toc *.out *~

PROJECT2=main2
BUILDTEX2=$(TEX) --shell-escape $(PROJECT2).tex
main2:
	$(BUILDTEX2)
	$(BIBTEX) $(PROJECT2)
	$(BUILDTEX)
	$(BUILDTEX)