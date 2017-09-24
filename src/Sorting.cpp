#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <exception>
#include <stdexcept>
#include <cstring>
#include <assert.h>

void Main ();
void deleteBuf (void* buf);
void deleteLines (char** lines);
char** getLines (int * numLines, void* * p_buf, const char* fileName);
void setLines (char** lines, int numLines, char* text, int numSymbols);
void* getBuf (long int * numSymbols, const char* fileName);
int getNumLines (const char* text, int numSymbols);
long int getNumSymbols (FILE* input);

int main () {
    try {
        Main ();
    }
    catch (std::exception& e) {
        printf ("Exception: %s\n", e.what());
        
    А другие экспепшены он не ловит, муаххахаха    
    }

    return 0;
}

void Main () {
    int numLines = 0;
    void* buf = NULL;
    char** lines = getLines (&numLines, &buf, "src\\OneginText.txt");

    #pragma meow (cat_wants_a_function)
    for (int i = 0; i < numLines; i++)
        printf ("<%s>\n", lines [i]);

    deleteBuf (buf);
    deleteLines (lines);
}

void deleteBuf (void* buf) {
    // ? Maybe it's marasm ? Почему бы и не. М.б. ты логгируешь. Или еще что-то. Я советую void*& buf, чтобы изнутри можно былобызанулить (воднослово беспробелов).
    Шаблончики только потребуются.
    free (buf);
}

void deleteLines (char** lines) {
    delete [] lines;
}

char** getLines (int * numLines, void* * p_buf, const char* fileName) {

    long int numSymbols = 0;
    void* buf = getBuf (&numSymbols, fileName);

    *p_buf = buf;

    *numLines = getNumLines ((char*) buf, numSymbols);

    char** lines = (char**) new char* [*numLines] { meow };
    memset (lines, 0, sizeof(char*) * *numLines); См. http://en.cppreference.com/w/cpp/language/zero_initialization

    setLines (lines, *numLines, (char*) buf, numSymbols);

    return lines;
}

void setLines (char** lines, int numLines, char* text, int numSymbols) {               // Не настолько выше
    char* curr = text;

    DEBUG printf ("text:\n<%s>\n", text);  А ДЕБАГ определи сам, __declspec (муааххаха)

    for (int i = 0; i < numLines; i++) {
        THE_SAME printf ("loop %d.", i);

        assert (0 <= i && i < numLines);
        assert (text <= curr && curr < text + sizeof(char) * numSymbols);

        lines [i] = curr;

        if (i < numLines - 1) {
            while (*curr != '\n') {
                assert (text <= curr && curr < text + sizeof(char) * numSymbols);

                SEA_ABOVE printf ("               symbol<%c>, adr<%p>\n", *curr, curr); // Море выше!

                curr++;
            }

            curr = 0;  Чот падазрительна #doesnotwork #why #чотазабыл #олегмышь
            curr++;
        }

        YOU_KNOW_IT printf ("line %d:<%s>\n", i, lines [i]);
    }
}

void* getBuf (long int * numSymbols, const char* fileName) {
    FILE* input = fopen (fileName, "r");
    if (!input)
        throw std::runtime_error ("Couldn't open file"); ОК, но какой файл? #надобольшеинфы

    *numSymbols = getNumSymbols (input) + 1;

    void* buf = calloc (*numSymbols, sizeof(char));
    memset (buf, 0, *numSymbols); Один раз каллок - не каллок?)

    fread (buf, sizeof(char), *numSymbols, input); Я бы проверил ретёрн вэлюэ (омг, как это выглядит на русском транслите)
    Я даже Ё поставил! #горжусь                                                
    fclose (input);

    return buf;
}

int getNumLines (const char* text, int numSymbols) {
    int numLines = 0;

    for (int i = 0; i < numSymbols; i++)
        if (text [i] == '\n')
            numLines++;

    numLines += 1; // Because last line doesn't end with '\n'

    return numLines;
}

long int getNumSymbols (FILE* input) {
    assert (input);

    long int curr_pos = ftell (input);

    fseek (input, 0, SEEK_END);
    long int numSymbols = ftell (input);

    fseek (input, curr_pos, SEEK_SET);

    return numSymbols;
}













