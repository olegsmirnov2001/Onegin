#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>

// A new name is required as I had to create a function [compare(const T&, const T&)]
//   to use it in std::sort (Oh yea, i did it)
typedef std::string* p_string;

void getLines (std::vector<p_string> * destination, const char* fileName);
void printLines (std::vector<p_string> * lines);
void sortLines (std::vector<p_string> * lines);
bool compareLines (const p_string & line1, const p_string & line2);

int main () {
    // I decided not to discover a bicycle
    //  so lines are stored as strings
    //  (an array consists of pointers on string)
    std::vector<p_string> lines;
    getLines (&lines, "src\\OneginText.txt");

    sortLines (&lines);

    printLines (&lines);

    return 0;
}

void getLines (std::vector<p_string> * destination, const char* fileName) {
    std::filebuf file;

    if (! file.open (fileName,std::ios::in) )
        throw std::runtime_error ("Invalid FileName");

    std::istream input(&file);

    input.get(); // reading three first system symbols
    input.get();
    input.get();

    while (input) {
        destination->push_back ((p_string) new std::string);
        std::getline (input, *(destination->back()));
    }

    destination->pop_back(); // deleting last line which is empty

    file.close();
}

void printLines (std::vector<p_string> * lines) {
    for(p_string line : *lines)
        printf ("%s\n", line->c_str());
}

void sortLines (std::vector<p_string> * lines) {
    // Oh my God! I'm /*cacashka*/ a busy student.

    std::sort (lines->begin(), lines->end(), compareLines);
}

bool compareLines (const p_string & line1, const p_string & line2) {
    // It is NOT yet alphabetical order:
    //   for example, capitals go before literals.

    return (*line1) < (*line2);
}

