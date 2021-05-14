/* adamite main header file that includes all other header files. */

/* memory management */
#include "memory.h"

/* lexer and parser objects */
#include "token.h"
#include "node.h"

/* interpreter objects */
#include "object.h"

/* lexer */
#include "lexer.h"

/* parser */
#include "parser.h"

/* interpreter */
#include "interpreter.h"

/* errors */
#include "error.h"

/* utilites */
#include "filelib.h" /* wrapper for reading files */
#include "datatypes.h" /* simple data types such as int which is dependent on system version */
#include "os.h" /* determine stuff like compiler and target os information */
#include "run.h" /* run file */

/* object storage */
#include "storage.h"
#include "names.h"

/* io */
#include "objectio.h"