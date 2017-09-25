#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <exception>
#include <stdexcept>
#include <cstring>
#include <assert.h>

#define do_debug true
#include "lib/dump_macro.h"

void Main ();
void deleteBuf (void* & buf);
void deleteLines (char** lines);
void printLines (char** lines, int numLines);
char** getLines (int * numLines, void* * p_buf, const char* fileName);
void setLines (char** lines, int numLines, char* text, int numSymbols);
void* getBuf (long int * numSymbols, const char* fileName);
int getNumLines (const char* text, int numSymbols);
long int getNumSymbols (FILE* input);

int main () {
    try {
        Main ();
    }
    catch (const my_error& e) {
        printf ("Exception <my_error>: %s---line.%d---%s\n"
                "                      %s\n",
                e.file(), e.line(), e.func(), e.what());
    }
    catch (const std::exception& e) {
        printf ("Exception <std::exception>: %s\n", e.what());
    }
    catch (const char*& e) {
        printf ("Exception <const char*>: %s\n", e);
    }
    catch (const int& e) {
        printf ("Exception <int>: %d\n", e);
    }
    catch (...) {
        printf ("Exception <...>\n");
    }

    return 0;
}

void Main () { DEBDUMP
    int numLines = 0;
    void* buf = NULL;
    char** lines = getLines (&numLines, &buf, "res\\OneginText.txt");

    //sortLines (lines, numLines); // What? Why the main part of the project is commented?
                                   //"This is the question", as Shakespeare have written...

    printLines (lines, numLines);

    deleteBuf (buf);
    deleteLines (lines);
}

void deleteBuf (void* & buf) { DEBDUMP
    assert (buf);

    free (buf);
    buf = NULL;
}

void deleteLines (char** lines) { DEBDUMP
    assert (lines);

    delete [] lines;
}

void printLines (char** lines, int numLines) {
    printf ("\n======= LINES ========\n");

    for (int i = 0; i < numLines; i++) {
        printf ("<%s>\n", lines [i]);
    }

    printf ("======================\n\n");
}

char** getLines (int * numLines, void* * p_buf, const char* fileName) { DEBDUMP

    long int numSymbols = 0;
    void* buf = getBuf (&numSymbols, fileName);

    *p_buf = buf;

    *numLines = getNumLines ((char*) buf, numSymbols);

    char** lines = (char**) new char* [*numLines] ();

    setLines (lines, *numLines, (char*) buf, numSymbols);

    return lines;
}

void setLines (char** lines, int numLines, char* text, int numSymbols) { DEBDUMP
    char* curr = text;

    DEBUG printf ("text:\n<%s>\n", text);

    for (int i = 0; i < numLines; i++) {
        assert (0 <= i && i < numLines);
        assert (text <= curr && curr < text + sizeof(char) * numSymbols);

        lines [i] = curr;

        if (i < numLines - 1) {
            while (*curr != '\n') {
                assert (text <= curr && curr < text + sizeof(char) * numSymbols);

                curr++;
            }

            *curr = 0;
            curr++;
        }
    }
}

void* getBuf (long int * numSymbols, const char* fileName) { DEBDUMP
    FILE* input = fopen (fileName, "r");
    if (!input)
        throw ERROR ("Could not open file <%s>", fileName);

    *numSymbols = getNumSymbols (input) + 1;

    void* buf = calloc (*numSymbols, sizeof(char));
    if (! buf)
        throw ERROR ("Failed to allocate memory");

    fread (buf, sizeof(char), *numSymbols, input);

    fclose (input);

    return buf;
}

int getNumLines (const char* text, int numSymbols) { DEBDUMP
    int numLines = 0;

    for (int i = 0; i < numSymbols; i++)
        if (text [i] == '\n')
            numLines++;

    numLines += 1; // Because last line doesn't end with '\n'

    return numLines;
}

long int getNumSymbols (FILE* input) { DEBDUMP
    assert (input);

    long int curr_pos = ftell (input);

    fseek (input, 0, SEEK_END);
    long int numSymbols = ftell (input);

    fseek (input, curr_pos, SEEK_SET);

    return numSymbols;
}













