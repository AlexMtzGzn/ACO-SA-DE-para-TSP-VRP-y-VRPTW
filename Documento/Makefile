TEXFILE=main
PDF=$(TEXFILE).pdf

all: clean $(PDF)

$(PDF): $(TEXFILE).tex
	@pdflatex $(TEXFILE)
	@bibtex $(TEXFILE) || true
	@pdflatex $(TEXFILE)
	@pdflatex $(TEXFILE)
	@echo "Compilación completada: $(PDF)"

clean:
	@rm -f *.aux *.log *.toc *.out *.bbl *.blg *.lof *.lot *.nav *.snm *.vrb *.fls *.fdb_latexmk *.synctex.gz $(PDF)
	@echo "Archivos temporales y PDF viejo borrados."

.PHONY: all clean
