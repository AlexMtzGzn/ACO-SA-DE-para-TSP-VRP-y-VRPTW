TEXFILE=tesis
PDF=$(TEXFILE).pdf

all: $(PDF)

$(PDF): $(TEXFILE).tex
	@pdflatex $(TEXFILE)
	@bibtex $(TEXFILE) || true
	@pdflatex $(TEXFILE)
	@pdflatex $(TEXFILE)
	@echo "Compilación completada: $(PDF)"

clean:
	@rm -f *.aux *.log *.toc *.out *.bbl *.blg *.lof *.lot *.nav *.snm *.vrb *.fls *.fdb_latexmk

cleanall: clean
	@rm -f $(PDF)
