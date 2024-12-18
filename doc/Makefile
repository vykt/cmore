.RECIPEPREFIX:=>

RST_DIR=./rst
MAN_DIR=./groff/man/man7
MD_DIR=./md

RST_DOC=error.rst lst.rst vct.rst rbt.rst
MAN_DOC=${RST_DOC:%.rst=cmore_%.7}
MD_DOC=${RST_DOC:%.rst=%.md}


all: clean manpages markdown

manpages: $(addprefix ${MAN_DIR}/, ${MAN_DOC})

markdown: $(addprefix ${MD_DIR}/, ${MD_DOC})

${MAN_DIR}/cmore_%.7: ${RST_DIR}/%.rst
> pandoc -f rst -t man -s $< -o $@ --variable section=7 \
	--variable header="CMore Documentation" --variable footer="CMore v1.0.0"

${MD_DIR}/%.md: ${RST_DIR}/%.rst
> pandoc -f rst -t markdown -s $< -o $@

clean:
> -rm -v ${MAN_DIR}/*.7 ${MD_DIR}/*.md
