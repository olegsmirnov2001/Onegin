#include <stdarg.h>
#include <cstring>
#include <stdio.h>

#if !defined do_debug
#define do_debug false
#endif
#if !defined do_fully_debug
#define do_fully_debug false
#endif

#define DEBUG if (do_debug)
#define FULLY_DEBUG if (do_fully_debug)

//TODO: dump to file also
//TODO: printf with different COLORS
#define DUMP printf ("--------------------%s---line.%-4d---%s\n", __FILE__, __LINE__, __FUNCTION__);

#define DEBDUMP DEBUG DUMP
#define FULLY_DEBDUMP FULLY_DEBUG DUMP

#define ERROR( ... ) my_error (__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

/** An exception class
 *
 *  It allows (due to macro ERROR(...) ) write:
 *  @usage
 *      try {
 *          int amount = Get_How_Many_Mices_Does_Cat_Want_To_Eat ();
 *          if (amount > 0)
 *              throw ERROR ("CAT WANTS TO EAT!!! <%d mices>", amount);
 *      }
 *      catch (const my_error& e) {
 *          printf ("Oups... Error occured (FILE<%s>, LINE<%d>, FUNCTION<%s>). <<%s>>",
                    e.file(), e.line(), e.func(), e.what());
 *      }
 */
class my_error {
    private:

    char* file_;
    int   line_;
    char* func_;
    char* msg_;

    public:

    #define BufSize 256
    my_error (const char* the_file_,
              const int   the_line_,
              const char* the_func_,
              const char* the_msg_, ... ) :
        file_ ( createLine (the_file_, "<Undefined File>"    ) ),
        line_ ( the_line_ ),
        func_ ( createLine (the_func_, "<Undefined Function>") ),
        msg_  ( (char*) new char [BufSize] () )
        {
        CheckIfNonZero (msg_);

        va_list arg; va_start (arg, the_msg_);
        int sz = _vsnprintf (msg_, BufSize - 1, the_msg_, arg);
        va_end (arg);

        if (sz + 1 < BufSize) {
            msg_ = (char*) realloc (msg_, sz + 1);
            CheckIfNonZero (msg_);
        }
    }
    #undef BufSize

    private:

    /**
     * @ingroup internal
     * Functions used only in constructor
     */
    char* createLine (const char* str, const char* strDefault) {
        assert (strDefault);

        char* dest = /*(char*)*/ new char [strlen (str ? str : strDefault) + 1] ();

        CheckIfNonZero (dest);

        strcpy (dest, str ? str : strDefault);

        return dest;
    }

    void CheckIfNonZero (char* text) {
        if (!text) {
            DUMP
            printf ("Failed to allocate memory while creating an exception\n");
            assert (0); // Something is wrong with the memory. No chances for throwing a message
        }
    }

    public:

    ~my_error() {
        delete [] file_;
        delete [] func_;
        delete [] msg_;
    }

    /**
     * Functions which are used for diagnostics
     *
     * It is guaranteed that returned statement is valid at least
     *   until the exception object is destroyed
     */

    public:

    const char* what() const {
        return msg_;
    }

    const char* file() const {
        return file_;
    }

    int line() const {
        return line_;
    }

    const char* func() const {
        return func_;
    }

    public:
    my_error(const my_error& e) :
        my_error (e.file(), e.line(), e.func(), e.what())
        {}

    /**
     * Does not have to be called
     */
    const my_error& operator = (const my_error&);
};






