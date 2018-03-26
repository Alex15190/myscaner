#ifndef SCANER_H
#define SCANER_H

#include "../include/abstract_scaner.h"
#include "../include/error_count.h"
#include "../include/location.h"
#include <string>"

enum Lexem_code : unsigned short {
    None,                    Unknown,            String,               
    Char,                    Kw_byte,            Kw_uint8,             
    Kw_uint16,               Kw_uint32,          Kw_uint64,            
    Kw_uint128,              Kw_long,            Kw_read,              
    Kw_float,                Kw_float32,         Kw_float64,           
    Kw_float80,              Kw_float128,        Kw_return,            
    Kw_endif,                Kw_print,           Kw_for,               
    Kw_if,                   Kw_val,             Kw_else,              
    Kw_elif,                 Kw_true,            Kw_const,             
    Kw_bool,                 Kw_bool8,           Kw_bool16,            
    Kw_bool32,               Kw_bool64,          Kw_false,             
    Kw_array,                Kw_module,          Kw_void,              
    Kw_var,                  Kw_enum,            Kw_repeat,            
    Kw_while,                Kw_export,          Kw_continue,          
    Kw_match,                Kw_char,            Kw_char8,             
    Kw_char16,               Kw_char32,          Kw_ref,               
    Kw_string,               Kw_string8,         Kw_string16,          
    Kw_string32,             Kw_struct,          Kw_type,              
    Kw_then,                 Kw_int,             Kw_int8,              
    Kw_int16,                Kw_int32,           Kw_int64,             
    Kw_int128,               Comma,              Semicolon,            
    Colon,                   Dot,                Open_round_bracket,   
    Closed_round_bracket,    Open_square_bracket,Closed_square_bracket,
    At_sign,                 Tuple_begin,        Tuple_end,            
    Conditional_op,          Logical_or,         Logical_or_not,       
    Logical_xor,             Logical_xor_not,    Logical_and,          
    Logical_and_not,         Logical_not,        Less_than,            
    Greater_than,            Lower_or_equals,    Greater_or_equals,    
    Not_equals,              Bitwise_xor,        Bitwise_or,           
    Bitwise_xor_not,         Bitwise_or_not,     Open_curly_bracket,   
    Closed_curly_bracket,    Bitwise_and,        Bitwise_and_not,      
    Left_shift,              Right_shift,        Signed_right_shift,   
    Bitwise_not,             Plus,               Minus,                
    Mul,                     Div,                Fdiv,                 
    Mod,                     Power,              Fpower,               
    Sizeof,                  Dimension_size,     Allocated_array_open, 
    Allocated_array_close,   Module_name_prefix, Scope_resolution,     
    Assign_to_field,         Label_prefix,       Label_suffix,         
    Assign,                  Kw_short,           Kw_uint,              
    Kw_fn,                   Kw_until,           Kw_break,             
    Conditional_and_apostrof,Ident
};

struct Lexem_info{
    Lexem_code code;
    union{
        size_t    ident_index;
        size_t    string_index;
        char32_t  c;
unsigned __int128 int_val;
__float128 float_val;
    };
};

class Scaner : public Abstract_scaner<Lexem_info> {
public:
    Scaner() = default;
    Scaner(Location* location, const Errors_and_tries& et) :
        Abstract_scaner<Lexem_info>(location, et) {};
    Scaner(const Scaner& orig) = default;
    virtual ~Scaner() = default;
    virtual Lexem_info current_lexem();
private:
    enum Automaton_name{
        A_start,     A_unknown, A_idKeyword, 
        A_delimiter, A_number,  A_string
    };
    Automaton_name automaton; /* current automaton */

   typedef bool (Scaner::*Automaton_proc)();
    /* This is the type of pointer to the member function that implements the
     * automaton that processes the lexeme. This function must return true if
     * the lexeme is not yet parsed, and false otherwise. */

    typedef void (Scaner::*Final_proc)();
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
__int128 integer_part;
__int128 fractional_part;
size_t exponent;
ssize_t exp_sign;
size_t frac_part_num_digits;
bool is_float;
char32_t precision;
char32_t char_code;
};
#endif
