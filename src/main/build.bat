@echo off
gcc -m32 -I "../include/" -o main main.c "../objects/object.c" "../parser/token.c" "../parser/lexer.c" "../utils/filelib.c" "../utils/storage.c" "../objects/objectio.c" "../parser/parser.c" "../parser/error.c" "../parser/node.c" "../interpreter/interpreter.c" "../utils/run.c" "../utils/names.c"