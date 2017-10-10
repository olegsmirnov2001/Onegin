#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <exception>
#include <stdexcept>
#include <cstring>
#include <assert.h>

#define do_debug true
#define do_fully_debug false
#include "lib/dump_macro.h"

void Main ();
char** getLines (const char* fileName);
char* getBuf (const char* fileName);
void seekIfSignature (FILE* file);
long getNumSymbolsUntilEnd (FILE* input);
void setLines (char** lines, size_t numLines, char* text);
int getNumLines (const char* text);
char* getBufAddr (char** lines);
size_t lenLines (const char* const * lines);
void sortLines (char** lines);
int compareLines (const void* line1, const void* line2);
void printLinesToFile (char** lines, const char* fileName);
void printLinesToFile (char** lines, FILE* file);
void deleteBuf (char* & buf);
void deleteLines (char** & lines);

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
    char** lines = getLines ("res\\OneginText.txt");
    char* buf = getBufAddr (lines);

    sortLines (lines);

    printLinesToFile (lines, "res\\SortedText.txt");

    deleteLines (lines);
    deleteBuf (buf);
}

char** getLines (const char* fileName) { DEBDUMP
    assert (fileName);

    char* buf = getBuf (fileName);

    size_t numLines = getNumLines (buf);
    char** lines = new (std::nothrow) char* [numLines + 1] ();
    if (! lines)
        throw ERROR ("Failed to allocate memory");

    FULLY_DEBUG printf ("MEMORY::Allocating lines: <%p>\n", lines);

    setLines (lines, numLines, buf);

    return lines;
}

char* getBuf (const char* fileName) { DEBDUMP
    assert (fileName);

    FILE * input = fopen (fileName, "r");
    if (!input)
        throw ERROR ("Could not open file <%s>", fileName);

    seekIfSignature (input);

    long numSymbols = getNumSymbolsUntilEnd (input);

    char* buf = new (std::nothrow) char [numSymbols + 1] ();
    if (! buf)
        throw ERROR ("Failed to allocate memory");

    fread (buf, sizeof(*buf), numSymbols, input);

    FULLY_DEBUG printf ("Buf:\n<%s>\n", buf);

    fclose (input);

    return buf;
}

void seekIfSignature (FILE* file) { DEBDUMP
    assert (file);

    assert (ftell (file) == 0);

    if ((fgetc (file) == 239 /*'0xEF'*/) && (fgetc (file) == 187 /*'0xBB'*/) && (fgetc (file) == 191 /*'0xBF'*/)) {
        DEBUG printf ("Detected Signature\n");
    }
    else {
        DEBUG printf ("Didn't detect signature\n");
        fseek (file, 0, SEEK_SET);
    }
}

long getNumSymbolsUntilEnd (FILE* input) { DEBDUMP
    assert (input);

    long curr_pos = ftell (input);

    fseek (input, 0, SEEK_END);
    long numSymbols = ftell (input) - curr_pos;

    fseek (input, curr_pos, SEEK_SET);

    FULLY_DEBUG printf ("   numSymbols %ld\n   curr_pos %ld\n", numSymbols, curr_pos);

    return numSymbols;
}

void setLines (char** lines, size_t numLines, char* buf) { DEBDUMP
    assert (lines);
    assert (buf);

    FULLY_DEBUG printf ("Buf:\n<%s>\nstrlen: <%I64u>\n", buf, strlen (buf));

    char* curr = buf;
    for (int i = 0; curr; i++) {
        FULLY_DEBUG printf ("   Loop %3d. (%p,%p)", i, buf, curr);

        // The only place where 'numLines' is used is assert, hmmm...
        assert (0 <= i && i < (int) numLines);

        lines [i] = curr;

        curr = strchr (curr, '\n');
        if (curr) {
            *curr = 0;
            curr++;
        }

        FULLY_DEBUG printf ("   Lines[%3d]=<%s>\n", i, lines [i]);
    }
}

int getNumLines (const char* text) { DEBDUMP
    int numLines = 0;

    for (char* curr = strchr (text, '\n'); curr; curr = strchr (curr + 1, '\n'))
        numLines++;

    numLines += 1; // Because last line doesn't end with '\n'

    return numLines;
}

char* getBufAddr (char** lines) { DEBDUMP
    assert (lines);
    assert (lines [0]);

    return lines [0];
}

size_t lenLines (const char* const * lines) { DEBDUMP
    for (size_t i = 0; true; i++) {
        if (lines [i] == nullptr)
            return i;
    }
}

void sortLines (char** lines) { DEBDUMP
    qsort ((void*) lines, lenLines (lines), sizeof(*lines), compareLines);
}

int compareLines (const void* line1, const void* line2) { FULLY_DEBDUMP
    if (line1 == line2)
        return 0;

    typedef const char* p_char;

    // Long two hours spent to find an error...
    #define l1 (*(const p_char*) line1)
    #define l2 (*(const p_char*) line2)

    FULLY_DEBUG printf ("   Comparing <%s> and <%s>\n", l1, l2);

    for (int i = 0; true; i++) {
        /*
        if (l1[i] == 0) {
            return -1;
        }
        if (l2[i] == 0) {
            return 1;
        }
        */
        // The comparison is not alphabetical yet. :(
        if (l1[i] != l2[i] || l1[i] == 0 || l2[i] == 0) {
            return ((unsigned char) l1[i] - (unsigned char) l2[i]);
        }
    }

    #undef l1
    #undef l2
}

void printLinesToFile (char** lines, const char* fileName) { DEBDUMP
    assert (fileName);

    FILE * dest = fopen (fileName, "w"); // destination
    if (! dest)
        throw ERROR ("Failed to create a file <%s>", fileName);

    printLinesToFile (lines, dest);

    fclose (dest);
}

void printLinesToFile (char** lines, FILE* file) { DEBDUMP
    assert (lines);
    assert (file);

    for (char** line = lines; *line; line++) {
        fprintf (file, "%s\n", *line);
    }
}

void deleteBuf (char* & buf) { DEBDUMP
    assert (buf);

    delete [] buf;
    buf = NULL;
}

void deleteLines (char** & lines) { DEBDUMP
    assert (lines);

    FULLY_DEBUG printf ("MEMORY::Deleting lines: <%p>\n", lines);

    delete [] lines;
    lines = NULL;
}












