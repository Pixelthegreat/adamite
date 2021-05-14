/* see run.h for documentation */

#include "run.h" /* our header */
#include "filelib.h" /* file stuff */
#include "lexer.h" /* lexer */
#include "parser.h" /* parser */
#include "interpreter.h" /* interpreter */
#include "storage.h" /* storage handling */
#include "object.h" /* object stuff */

#include <stdio.h> /* printf */
#include <stdlib.h> /* free */

#ifdef __cplusplus /* c++ check */
extern "C" {
#endif

int run(const char *fname) {
	/* error code */
	int code = 0; /* 'ok', will be 1 if error was found */
	/* create a new file */
	file *f = open(fname, "r");

	/* failed to find file */
	if (f == NULL) {
		printf("File not found: %s\n", fname); /* print error */
		return 2; /* return */
	}

	/* get file text */
	char *s = read(f);

	/* create a lexer */
	lexer *l = LEXER_NewLexer(s);

	/* make our tokens */
	LEXER_MakeTokens(l);

	/* break early if no tokens were made */
	if (l->n_of_tokens <= 1) {
		LEXER_FreeLexer(l);
		return 0; /* default exit break code */
	}

	/* no error */
	if (!l->err) {
		/* create new parser */
		parser *p = PARSER_NewParser(l->tokens, l->n_of_tokens);
		/* parse tokens */
		PARSER_Parse(p);

		/* error found */
		if (p->e != NULL) {
			/* get error string */
			char *cs = ERROR_AsString(p->e);

			/* print error string */
			printf("%s\n", cs);

			/* free error string */
			free(cs);

			code = 1; /* was error */
		}
		/* no error */
		else {
			/* memory error */
			if (p->newNode == NULL) {
				printf("Memory Error\n");
				code = 1; /* was error */
			}
			/* otherwise */
			else {
				/* new interpreter */
				interpreter *i = INTERPRETER_NewInterpreter();

				/* visit node */
				object *o = INTERPRETER_Visit(i, p->newNode);

				/* visit method not found */
				if (o == NULL && i->e == NULL) {
					printf("Unknown visit method for type: %d\n", p->newNode->type); /* print error message */
					code = 1; /* was error */
				}
				/* error */
				else if (i->e != NULL) {
					/* get error string */
					char *cs = ERROR_AsString(i->e);

					/* print error string */
					printf("%s\n", cs);

					/* free error string */
					free(cs);

					code = 1; /* was error */
				}
				/* otherwise */
				else {
					/* free object if not registered */
					if (!STORAGE_Find(o)) OBJECT_FreeObject(o);
				}

				/* free interpreter */
				INTERPRETER_FreeInterpreter(i);
			}
		}

		/* free parser */
		PARSER_FreeParser(p);
	}

	/* free lexer */
	LEXER_FreeLexer(l);

	/* return exit code */
	return code;
}

#ifdef __cplusplus /* c++ check */
}
#endif