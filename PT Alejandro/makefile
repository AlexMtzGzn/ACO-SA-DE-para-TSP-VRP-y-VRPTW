# Variables
FILE = main
PDFLATEX = pdflatex
BIBTEX = bibtex
MAKEINDEX = makeindex
LATEXMK = latexmk

# Regla principal
all: $(FILE).pdf

# Regla para compilar el PDF
$(FILE).pdf: $(FILE).tex
    $(LATEXMK) -pdf $(FILE).tex

# Regla para limpiar los archivos generados
clean:
    $(LATEXMK) -c $(FILE).tex
    rm -f $(FILE).pdf $(FILE).log $(FILE).aux $(FILE).bbl $(FILE).blg $(FILE).dvi $(FILE).ps
