# Makefile for Codexium Magnus Documentation
# Generates HTML, PDF, and DOCX from AsciiDoc sources

.PHONY: all html pdf docx diagrams clean help install-deps

# Directories
SOURCE_DIR = asciidoc
BUILD_DIR = build
DIAGRAMS_DIR = $(SOURCE_DIR)/diagrams
OUTPUT_DIAGRAMS = $(BUILD_DIR)/diagrams
HTML_DIR = $(BUILD_DIR)/html
PDF_DIR = $(BUILD_DIR)/pdf
DOCX_DIR = $(BUILD_DIR)/docx

# Tools
ASCIIDOC = asciidoctor
ASCIIDOC_PDF = asciidoctor-pdf
MMDC = mmdc
PANDOC = pandoc

# Source files
ADOC_FILES = $(wildcard $(SOURCE_DIR)/*.adoc)
MMD_FILES = $(wildcard $(DIAGRAMS_DIR)/*.mmd)

# Default target
all: diagrams html pdf

help:
	@echo "Documentation Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all          - Build all outputs (diagrams, HTML, PDF)"
	@echo "  html         - Generate HTML documentation"
	@echo "  pdf          - Generate PDF documentation"
	@echo "  docx         - Generate DOCX documentation (requires pandoc)"
	@echo "  diagrams     - Generate diagrams from Mermaid sources"
	@echo "  clean        - Remove all generated files"
	@echo "  install-deps - Install required dependencies (macOS/Linux)"
	@echo ""

install-deps:
	@echo "Installing dependencies..."
	@echo "Checking for AsciiDoctor..."
	@which gem > /dev/null && gem install asciidoctor asciidoctor-pdf || echo "Ruby/gem not found. Install Ruby first."
	@echo "Checking for Mermaid CLI..."
	@which npm > /dev/null && npm install -g @mermaid-js/mermaid-cli || echo "Node.js/npm not found. Install Node.js first."
	@echo "Checking for Pandoc (optional, for DOCX)..."
	@which pandoc > /dev/null || echo "Pandoc not found. Install for DOCX support."

diagrams:
	@echo "Generating diagrams..."
	@mkdir -p $(OUTPUT_DIAGRAMS)
	@if [ -z "$(MMD_FILES)" ]; then \
		echo "No Mermaid files found in $(DIAGRAMS_DIR)"; \
	else \
		for file in $(MMD_FILES); do \
			base=$$(basename $$file .mmd); \
			echo "  Processing $$base..."; \
			$(MMDC) -i $$file -o $(OUTPUT_DIAGRAMS)/$$base.svg -b transparent 2>/dev/null || \
			$(MMDC) -i $$file -o $(OUTPUT_DIAGRAMS)/$$base.png -b transparent 2>/dev/null || \
			echo "    Warning: Failed to generate diagram for $$base"; \
		done; \
		echo "Diagrams generated in $(OUTPUT_DIAGRAMS)"; \
	fi

html: diagrams
	@echo "Generating HTML documentation..."
	@mkdir -p $(HTML_DIR)
	@for file in $(ADOC_FILES); do \
		base=$$(basename $$file .adoc); \
		echo "  Processing $$base..."; \
		$(ASCIIDOC) -D $(HTML_DIR) \
			-a imagesdir=../diagrams \
			-a stylesdir=../stylesheets \
			-a stylesheet=codexium-magnus.css \
			$$file || echo "    Error processing $$base"; \
	done
	@echo "HTML documentation generated in $(HTML_DIR)"

pdf: diagrams
	@echo "Generating PDF documentation..."
	@mkdir -p $(PDF_DIR)
	@for file in $(ADOC_FILES); do \
		base=$$(basename $$file .adoc); \
		echo "  Processing $$base..."; \
		$(ASCIIDOC_PDF) -D $(PDF_DIR) \
			-a imagesdir=../diagrams \
			-a pdf-stylesdir=../themes \
			-a pdf-style=pdf-theme \
			$$file || echo "    Error processing $$base"; \
	done
	@echo "PDF documentation generated in $(PDF_DIR)"

docx: html
	@echo "Generating DOCX documentation..."
	@mkdir -p $(DOCX_DIR)
	@if ! which $(PANDOC) > /dev/null; then \
		echo "Error: Pandoc not found. Install Pandoc for DOCX support."; \
		exit 1; \
	fi
	@for file in $(HTML_DIR)/*.html; do \
		base=$$(basename $$file .html); \
		echo "  Processing $$base..."; \
		$(PANDOC) $$file -o $(DOCX_DIR)/$$base.docx || echo "    Error processing $$base"; \
	done
	@echo "DOCX documentation generated in $(DOCX_DIR)"

clean:
	@echo "Cleaning generated files..."
	rm -rf $(BUILD_DIR)
	@echo "Clean complete"
