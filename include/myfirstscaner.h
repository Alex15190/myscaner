#ifndef MYFIRSTSCANER_H
#define MYFIRSTSCANER_H

#include "../include/abstract_scaner.h"
#include "../include/error_count.h"
#include "../include/location.h"
#include <string>"

enum codes : unsigned short {
    None,Unknown,k1, 
    k2,  k3,     k4, 
    k5,  k6,     k7, 
    k8,  k9,     k10,
    k11, k12,    k13,
    k14, k15,    k16,
    k17, k18,    k19,
    k20, k21,    k22,
    k23, k24,    k25,
    k26, k27,    k28,
    k29, k30,    k31,
    k32, k33,    k34,
    k35, k36,    k37,
    k38, k39,    k40,
    k41, k42,    k43,
    k44, k45,    k46,
    k47, k48,    k49,
    k50, k51,    k52,
    k53, k54,    k55,
    k56, k57,    k58,
    d1,  d2,     d3, 
    d4,  d5,     d6, 
    d7,  d8,     d9, 
    d10, d11,    d12,
    d13, d14,    d15,
    d16, d17,    d18,
    d19, d20,    d21,
    d22, d23,    d24,
    d25, d26,    d27,
    d28, d29,    d30,
    d31, d32,    d35,
    d36, d37,    d38,
    d39, d40,    d41,
    d42, d43,    d44,
    d45, d46,    d47,
    d48, d49
};

struct Lexem_info{
    codes code;
    union{
        size_t    ident_index;
        size_t    string_index;
        char32_t  c;
unsigned __int128 int_val; __float128 float_val;
    };
};

class myfirstscaner : public Abstract_scaner<Lexem_info> {
public:
    myfirstscaner() = default;
    myfirstscaner(Location* location, const Errors_and_tries& et) :
        Abstract_scaner<Lexem_info>(location, et) {};
    myfirstscaner(const myfirstscaner& orig) = default;
    virtual ~myfirstscaner() = default;
    virtual Lexem_info current_lexem();
private:
    enum Automaton_name{
        A_start,     A_unknown, A_idKeyword, 
        A_delimiter, A_number,  A_string
    };
    Automaton_name automaton; /* current automaton */

   typedef bool (myfirstscaner::*Automaton_proc)();
    /* This is the type of pointer to the member function that implements the
     * automaton that processes the lexeme. This function must return true if
     * the lexeme is not yet parsed, and false otherwise. */

    typedef void (myfirstscaner::*Final_proc)();
    /* And this is the type of the pointer to the member function that performs
     * the necessary actions in the event of an unexpected end of the lexeme. */

    static Automaton_proc procs[];
    static Final_proc     finals[];

    /* Lexeme processing functions: */
    bool start_proc();     bool unknown_proc();   
    bool idkeyword_proc(); bool delimiter_proc(); 
    bool number_proc();    bool string_proc();
    /* functions for performing actions in case of an
     * unexpected end of the token */
    void none_proc();            void unknown_final_proc();   
    void idkeyword_final_proc(); void delimiter_final_proc(); 
    void number_final_proc();    void string_final_proc();
};
#endif
