#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <exception>
#include <stdexcept>
#include <cstring>
#include <assert.h>

#define do_debug false
#define do_fully_debug false
#include "lib/dump_macro.h"

void Main ();
void sortLines (char** lines, int numLines);
int compareLines (const void* line1, const void* line2);
void deleteBuf (void* & buf);
void deleteLines (char** & lines);
void printLines (char** lines, int numLines);
void printLinesToFile (char** lines, int numLines, const char* fileName);
char** getLines (int * numLines, void* * p_buf, const char* fileName);
void setLines (char** lines, int numLines, char* text, int numSymbols);
void cutSignature (char* & line);
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

    sortLines (lines, numLines);

    printLinesToFile (lines, numLines, "res\\SortedText.txt");

    deleteBuf (buf);
    deleteLines (lines);
}

void sortLines (char** lines, int numLines) { DEBDUMP
    qsort ((void*) lines, (size_t) numLines, sizeof(char*), compareLines);
}

int compareLines (const void* line1, const void* line2) { FULLY_DEBDUMP
    if (line1 == line2)
        return 0;

    typedef char* p_char;

    // Long two hours spent to find an error...
    #define l1 (*((const p_char*) line1))
    #define l2 (*((const p_char*) line2))

    FULLY_DEBUG printf ("   Comparing <%s> and <%s>\n", l1, l2);

    for (int i = 0; true; i++) {
        if (l1[i] == 0) {
            return -1;
        }
        if (l2[i] == 0) {
            return 1;
        }

        // The comparison is not alphabetical yet. :(
        if (l1[i] < l2[i]) {
            return -1;
        }
        if (l1[i] > l2[i]) {
            return 1;
        }
    }

    #undef l1
    #undef l2
}

void deleteBuf (void* & buf) { DEBDUMP
    assert (buf);

    free (buf);
    buf = NULL;
}

void deleteLines (char** & lines) { DEBDUMP
    assert (lines);

    FULLY_DEBUG printf ("MEMORY::Deleting lines: <%p>\n", lines);

    delete [] lines;
    lines = NULL;
}

void printLines (char** lines, int numLines) {
    printf ("\n======= LINES ========\n");

    for (int i = 0; i < numLines; i++) {
        printf ("<%s>\n", lines [i]);
    }

    printf ("======================\n\n");
}

void printLinesToFile (char** lines, int numLines, const char* fileName) { DEBDUMP
    FILE * dest = fopen (fileName, "w"); // destination
    if (! dest)
        throw ERROR ("Failed to create a file <%s>", fileName);

    for (int i = 0; i < numLines; i++) {
        fprintf (dest, "%s\n", lines [i]);
    }

    fclose (dest);
}

char** getLines (int * numLines, void* * p_buf, const char* fileName) { DEBDUMP

    long int numSymbols = 0;
    void* buf = getBuf (&numSymbols, fileName);

    *p_buf = buf;

    *numLines = getNumLines ((char*) buf, numSymbols);

    char** lines = (char**) new char* [*numLines] ();
    if (! lines)
        throw ERROR ("Failed to allocate memory");

    FULLY_DEBUG printf ("MEMORY::Allocating lines: <%p>\n", lines);

    setLines (lines, *numLines, (char*) buf, numSymbols);

    return lines;
}

void setLines (char** lines, int numLines, char* text, int numSymbols) { DEBDUMP
    char* curr = text;

    FULLY_DEBUG printf ("text:\n<%s>\n", text);

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

    if (numLines > 0)
        cutSignature (lines [0]);
}

void cutSignature (char* & line) { DEBDUMP
    FULLY_DEBUG printf ("Line: <%s>\n", line);

    if (line && (line[0] == -17 /*'0xEF'*/) && (line[1] == -69/*'0xBB'*/) && (line[2] == -65/*'0xBF'*/)) {
        DEBUG printf ("Cut Signature. line: (%p,<%s>)->(%p,<%s>)\n", line, line, line + 3, line + 3);

        line += 3;
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













