#include "../include/scaner.h"
#include "../include/get_init_state.h"
#include "../include/search_char.h"
#include "../include/belongs.h"
#include <set>
#include <string>
#include <vector>
#include "../include/operations_with_sets.h"

  #include <quadmath>

  size_t digit2int(char32_t ch) {
    size_t v = ch - U'0';
    return (v<=9)? v : (v&0b1101'1111) - 7;
  }
  __int128 setexp(char32_t ch) {
    return (ch == '-')? -1 : 1;
  }

  __float128 Scaner::build_float(){
    return integer_part + fractional_part*powq(10,-frac_part_num_digits)+exp_sign*exponent;
  }

  Lexem_code precision2code(char32_t ch){
    switch (ch) {
      case U'S': case U's':
        return Float32;
        break;
      case U'D': case U'd':
        return Float64;
        break;
      case U'E': case U'e':
        return Float80;
        break;
      case U'Q': case U'q':
        return Float128;
        break;
      default:
        return Float32;
        break;
    }
  }

Scaner::Automaton_proc Scaner::procs[] = {
    &Scaner::start_proc(),     &Scaner::unknown_proc(),   
    &Scaner::idkeyword_proc(), &Scaner::delimiter_proc(), 
    &Scaner::number_proc(),    &Scaner::string_proc()
};

Scaner::Final_proc Scaner::finals[] = {
    &Scaner::none_proc(),            &Scaner::unknown_final_proc(),   
    &Scaner::idkeyword_final_proc(), &Scaner::delimiter_final_proc(), 
    &Scaner::number_final_proc(),    &Scaner::string_final_proc()
};

enum Category {
    SPACES,          DELIMITER_BEGIN, 
    STRING0,         STRING_BEGIN,    
    STRING1,         STRING2,         
    STRING10,        STRING3,         
    STRING4,         STRING5,         
    STRING6,         STRING7,         
    STRING8,         STRING9,         
    STRING11,        NUMBER0,         
    NUMBER1,         NUMBER2,         
    IDKEYWORD_BEGIN, IDKEYWORD0,      
    IDKEYWORD1,      IDKEYWORD2,      
    IDKEYWORD3,      Other
};

static const std::map<char32_t, uint32_t> categories_table = {
    {'\0', 1},       {'\X01', 1},     {'\X02', 1},     {'\X03', 1},     
    {'\X04', 1},     {'\X05', 1},     {'\X06', 1},     {'\a', 1},       
    {'\b', 1},       {'\t', 1},       {'\n', 1},       {'\v', 1},       
    {'\f', 1},       {'\r', 1},       {'\X0e', 1},     {'\X0f', 1},     
    {'\X10', 1},     {'\X11', 1},     {'\X12', 1},     {'\X13', 1},     
    {'\X14', 1},     {'\X15', 1},     {'\X16', 1},     {'\X17', 1},     
    {'\X18', 1},     {'\X19', 1},     {'\X1a', 1},     {'\X1b', 1},     
    {'\X1c', 1},     {'\X1d', 1},     {'\X1e', 1},     {'\X1f', 1},     
    {' ', 1},        {'!', 2},        {", 12},         {'#', 2},        
    {'$', 24},       {'%', 2},        {'&', 2},        {', 32},         
    {'(', 2},        {')', 2},        {'*', 2},        {'+', 131074},   
    {',', 2},        {'-', 131074},   {'.', 65538},    {'/', 2},        
    {'0', 2106304},  {'1', 2122624},  {'2', 2122496},  {'3', 2122496},  
    {'4', 2122496},  {'5', 4219648},  {'6', 2122496},  {'7', 4219648},  
    {'8', 2122240},  {'9', 4219392},  {':', 2},        {';', 2},        
    {'<', 2},        {'>', 2},        {'?', 2},        {'@', 2},        
    {'A', 4988928},  {'B', 4989952},  {'C', 4988928},  {'D', 5021696},  
    {'E', 5021696},  {'F', 4988928},  {'G', 4980736},  {'H', 4980736},  
    {'I', 4980736},  {'J', 4980736},  {'K', 4980736},  {'L', 4980736},  
    {'M', 4980736},  {'N', 4980736},  {'O', 4980736},  {'P', 4980736},  
    {'Q', 5013504},  {'R', 4980736},  {'S', 5013504},  {'T', 4980736},  
    {'U', 4980736},  {'V', 4980736},  {'W', 4980736},  {'X', 4982784},  
    {'Y', 4980736},  {'Z', 4980736},  {'[', 2},        {']', 2},        
    {'^', 2},        {'_', 2883584},  {'`', 2},        {'a', 3416064},  
    {'b', 5514240},  {'c', 3416064},  {'d', 2924544},  {'e', 3448832},  
    {'f', 3416064},  {'g', 2883584},  {'h', 2883584},  {'i', 3407872},  
    {'j', 4980736},  {'k', 2883584},  {'l', 3407872},  {'m', 3407872},  
    {'n', 2883584},  {'o', 2887680},  {'p', 3407872},  {'q', 5013504},  
    {'r', 3407872},  {'s', 3440640},  {'t', 3407872},  {'u', 3407872},  
    {'v', 5505024},  {'w', 5505024},  {'x', 2885632},  {'y', 2883584},  
    {'z', 4980736},  {'{', 2},        {'|', 2},        {'}', 2},        
    {'~', 2},        {'Ё', 4980736}, {'А', 4980736}, {'Б', 4980736}, 
    {'В', 4980736}, {'Г', 4980736}, {'Д', 4980736}, {'Е', 4980736}, 
    {'Ж', 4980736}, {'З', 4980736}, {'И', 4980736}, {'Й', 4980736}, 
    {'К', 4980736}, {'Л', 4980736}, {'М', 4980736}, {'Н', 4980736}, 
    {'О', 4980736}, {'П', 4980736}, {'Р', 4980736}, {'С', 4980736}, 
    {'Т', 4980736}, {'У', 4980736}, {'Ф', 4980736}, {'Х', 4980736}, 
    {'Ц', 4980736}, {'Ч', 4980736}, {'Ш', 4980736}, {'Щ', 4980736}, 
    {'Ъ', 4980736}, {'Ы', 4980736}, {'Ь', 4980736}, {'Э', 4980736}, 
    {'Ю', 4980736}, {'Я', 4980736}, {'а', 4980736}, {'б', 4980736}, 
    {'в', 5505024}, {'г', 4980736}, {'д', 4980736}, {'е', 2883584}, 
    {'ж', 4980736}, {'з', 4980736}, {'и', 4980736}, {'й', 4980736}, 
    {'к', 4980736}, {'л', 4980736}, {'м', 4980736}, {'н', 4980736}, 
    {'о', 4980736}, {'п', 4980736}, {'р', 4980736}, {'с', 4980736}, 
    {'т', 4980736}, {'у', 4980736}, {'ф', 4980736}, {'х', 4980736}, 
    {'ц', 4980736}, {'ч', 4980736}, {'ш', 4980736}, {'щ', 2883584}, 
    {'ъ', 4980736}, {'ы', 4980736}, {'ь', 4980736}, {'э', 4980736}, 
    {'ю', 4980736}, {'я', 4980736}, {'ё', 4980736}
};


uint64_t get_categories_set(char32_t c){
    auto it = categories_table.find(c);
    if(it != categories_table.end()){
        return it->second;
    }else{
        return 1ULL << Other;
    }
}
bool Scaner::start_proc(){
    bool t = true;
    state = -1;
    /* For an automaton that processes a token, the state with the number (-1) is
     * the state in which this automaton is initialized. */
    if(belongs(SPACES, char_categories)){
        loc->current_line += U'\n' == ch;
        return t;
    }
    lexem_begin_line = loc->current_line;
    if(belongs(DELIMITER_BEGIN, char_categories)){
        (loc->pcurrent_char)--; automaton = A_delimiter;
        state = -1;
        return t;
    }

    if(belongs(STRING_BEGIN, char_categories)){
        (loc->pcurrent_char)--; automaton = A_string;
        state = 0;
        buffer.clear();
        return t;
    }

    if(belongs(STRING5, char_categories)){
        (loc->pcurrent_char)--; automaton = A_number;
        state = 0;
        int_val = 0;
          float_val = 0;
          is_float = false;
          integer_part = 0;
          fractional_part = 0;
          exponent = 1;
          exp_sign = 1;
          frac_part_num_digits = 0;
          token.code = Integer;
        return t;
    }

    if(belongs(IDKEYWORD_BEGIN, char_categories)){
        (loc->pcurrent_char)--; automaton = A_idKeyword;
        state = 0;
        return t;
    }

    automaton = A_unknown;
    return t;
}

bool Scaner::unknown_proc(){
    return belongs(Other, char_categories);
}

struct Keyword_list_elem{
    std::u32string keyword;
    Lexem_code kw_code;
};

static const Keyword_list_elem kwlist[] = {
    {U"array", Kw_array},         {U"bool", Kw_bool},           
    {U"bool16_t", Kw_bool16},     {U"bool32_t", Kw_bool32},     
    {U"bool64_t", Kw_bool64},     {U"bool8_t", Kw_bool8},       
    {U"break", Kw_break},         {U"byte", Kw_byte},           
    {U"char", Kw_char},           {U"char16_t", Kw_char16},     
    {U"char32_t", Kw_char32},     {U"char8_t", Kw_char8},       
    {U"const", Kw_const},         {U"continue", Kw_continue},   
    {U"elif", Kw_elif},           {U"else", Kw_else},           
    {U"endif", Kw_endif},         {U"enum", Kw_enum},           
    {U"export", Kw_export},       {U"false", Kw_false},         
    {U"float", Kw_float},         {U"float128_t", Kw_float128}, 
    {U"float32_t", Kw_float32},   {U"float64_t", Kw_float64},   
    {U"fn", Kw_fn},               {U"for", Kw_for},             
    {U"if", Kw_if},               {U"int", Kw_int},             
    {U"int128_t", Kw_int128},     {U"int16_t", Kw_int16},       
    {U"int32_t", Kw_int32},       {U"int64_t", Kw_int64},       
    {U"int8_t", Kw_int8},         {U"long", Kw_long},           
    {U"match", Kw_match},         {U"module", Kw_module},       
    {U"print", Kw_print},         {U"read", Kw_read},           
    {U"ref", Kw_ref},             {U"repeat", Kw_repeat},       
    {U"return", Kw_return},       {U"short", Kw_short},         
    {U"string", Kw_string},       {U"string16_t", Kw_string16}, 
    {U"string32_t", Kw_string32}, {U"string8_t", Kw_string8},   
    {U"struct", Kw_struct},       {U"then", Kw_then},           
    {U"true", Kw_true},           {U"type", Kw_type},           
    {U"uint", Kw_uint},           {U"uint128_t", Kw_uint128},   
    {U"uint16_t", Kw_uint16},     {U"uint32_t", Kw_uint32},     
    {U"uint64_t", Kw_uint64},     {U"uint8_t", Kw_uint8},       
    {U"until", Kw_until},         {U"val", Kw_val},             
    {U"var", Kw_var},             {U"void", Kw_void},           
    {U"while", Kw_while},         {U"вещ80", Kw_float80}
};

#define NUM_OF_KEYWORDS 62

#define THERE_IS_NO_KEYWORD (-1)

static int search_keyword(const std::u32string& finded_keyword){
    int result      = THERE_IS_NO_KEYWORD;
    int low_bound   = 0;
    int upper_bound = NUM_OF_KEYWORDS - 1;
    int middle;
    while(low_bound <= upper_bound){
        middle             = (low_bound + upper_bound) / 2;
        auto& curr_kw      = kwlist[middle].keyword;
        int compare_result = finded_keyword.compare(curr_kw);
        if(0 == compare_result){
            return middle;
        }
        if(compare_result < 0){
            upper_bound = middle - 1;
        }else{
            low_bound   = middle + 1;
        }
    }
    return result;
}

static const std::set<size_t> final_states_for_idkeywords = {
    1
};

bool Scaner::idkeyword_proc(){
    bool t             = true;
    bool there_is_jump = false;
    switch(state){
        case 0:
            if(belongs(IDKEYWORD0, char_categories)){
                state = 1;
                there_is_jump = true;
            }
             else if(belongs(IDKEYWORD1, char_categories)){
                buffer += ch;
                state = 1;
                there_is_jump = true;
            }

            break;
        case 1:
            if(belongs(IDKEYWORD2, char_categories)){
                buffer += ch;
                state = 1;
                there_is_jump = true;
            }
             else if(belongs(IDKEYWORD3, char_categories)){
                state = 1;
                there_is_jump = true;
            }

            break;
        default:
            ;
    }

    if(!there_is_jump){
        t = false;
        if(!is_elem(state, final_states_for_idkeywords)){
            printf("At line %zu unexpectedly ended identifier or keyword.", loc->current_line);
            en->increment_number_of_errors();
        }
        
        int search_result = search_keyword(buffer);
        if(search_result != THERE_IS_NO_KEYWORD) {
            token.code = kwlist[search_result].kw_code;
        }
    }

    return t;
}

static const State_for_char init_table_for_delimiters[] ={
    {21, U'!'}, {54, U'#'}, {53, U'%'}, {31, U'&'}, {14, U'('}, 
    {16, U')'}, {48, U'*'}, {46, U'+'}, {3, U','},  {47, U'-'}, 
    {5, U'.'},  {51, U'/'}, {6, U':'},  {4, U';'},  {33, U'<'}, 
    {38, U'>'}, {17, U'?'}, {56, U'@'}, {0, U'['},  {2, U']'},  
    {29, U'^'}, {57, U'`'}, {12, U'{'}, {18, U'|'}, {13, U'}'}, 
    {42, U'~'}
};

struct Elem {
    /** A pointer to a string of characters that can be crossed. */
    char32_t*       symbols;
    /** A lexeme code. */
    Lexem_code code;
    /** If the current character matches symbols[0], then the transition to the state
     *  first_state;
     *  if the current character matches symbols[1], then the transition to the state
     *  first_state + 1;
     *  if the current character matches symbols[2], then the transition to the state
     *  first_state + 2, and so on. */
    uint16_t        first_state;
};

static const Elem delim_jump_table[] = {
    {const_cast<char32_t*>(U":"), Open_square_bracket, 1},     
    {const_cast<char32_t*>(U""), Allocated_array_open, 0},     
    {const_cast<char32_t*>(U""), Closed_square_bracket, 0},    
    {const_cast<char32_t*>(U""), Comma, 0},                    
    {const_cast<char32_t*>(U""), Semicolon, 0},                
    {const_cast<char32_t*>(U""), Dot, 0},                      
    {const_cast<char32_t*>(U")]>:="), Colon, 7},               
    {const_cast<char32_t*>(U""), Tuple_end, 0},                
    {const_cast<char32_t*>(U""), Allocated_array_close, 0},    
    {const_cast<char32_t*>(U""), Label_suffix, 0},             
    {const_cast<char32_t*>(U""), Scope_resolution, 0},         
    {const_cast<char32_t*>(U""), Assign, 0},                   
    {const_cast<char32_t*>(U""), Open_curly_bracket, 0},       
    {const_cast<char32_t*>(U""), Closed_curly_bracket, 0},     
    {const_cast<char32_t*>(U":"), Open_round_bracket, 15},     
    {const_cast<char32_t*>(U""), Tuple_begin, 0},              
    {const_cast<char32_t*>(U""), Closed_round_bracket, 0},     
    {const_cast<char32_t*>(U""), Conditional_op, 0},           
    {const_cast<char32_t*>(U"|:"), Bitwise_or, 19},            
    {const_cast<char32_t*>(U""), Logical_or, 0},               
    {const_cast<char32_t*>(U""), Module_name_prefix, 0},       
    {const_cast<char32_t*>(U"|^&="), Logical_not, 22},         
    {const_cast<char32_t*>(U"|"), Unknown, 23},                
    {const_cast<char32_t*>(U"^"), Unknown, 24},                
    {const_cast<char32_t*>(U"&"), Unknown, 25},                
    {const_cast<char32_t*>(U""), Not_equals, 0},               
    {const_cast<char32_t*>(U""), Logical_or_not, 0},           
    {const_cast<char32_t*>(U""), Logical_xor_not, 0},          
    {const_cast<char32_t*>(U""), Logical_and_not, 0},          
    {const_cast<char32_t*>(U"^"), Bitwise_xor, 30},            
    {const_cast<char32_t*>(U""), Logical_xor, 0},              
    {const_cast<char32_t*>(U"&"), Bitwise_and, 32},            
    {const_cast<char32_t*>(U""), Logical_and, 0},              
    {const_cast<char32_t*>(U"=<-:"), Less_than, 34},           
    {const_cast<char32_t*>(U""), Lower_or_equals, 0},          
    {const_cast<char32_t*>(U""), Left_shift, 0},               
    {const_cast<char32_t*>(U""), Assign_to_field, 0},          
    {const_cast<char32_t*>(U""), Label_prefix, 0},             
    {const_cast<char32_t*>(U"=>"), Greater_than, 39},          
    {const_cast<char32_t*>(U""), Greater_or_equals, 0},        
    {const_cast<char32_t*>(U">"), Right_shift, 41},            
    {const_cast<char32_t*>(U""), Signed_right_shift, 0},       
    {const_cast<char32_t*>(U"|^&"), Bitwise_not, 43},          
    {const_cast<char32_t*>(U""), Bitwise_or_not, 0},           
    {const_cast<char32_t*>(U""), Bitwise_xor_not, 0},          
    {const_cast<char32_t*>(U""), Bitwise_and_not, 0},          
    {const_cast<char32_t*>(U""), Plus, 0},                     
    {const_cast<char32_t*>(U""), Minus, 0},                    
    {const_cast<char32_t*>(U"*"), Mul, 49},                    
    {const_cast<char32_t*>(U"."), Power, 50},                  
    {const_cast<char32_t*>(U""), Fpower, 0},                   
    {const_cast<char32_t*>(U"."), Div, 52},                    
    {const_cast<char32_t*>(U""), Fdiv, 0},                     
    {const_cast<char32_t*>(U""), Mod, 0},                      
    {const_cast<char32_t*>(U"#"), Sizeof, 55},                 
    {const_cast<char32_t*>(U""), Dimension_size, 0},           
    {const_cast<char32_t*>(U""), At_sign, 0},                  
    {const_cast<char32_t*>(U"?"), Unknown, 58},                
    {const_cast<char32_t*>(U""), Conditional_and_apostrof, 0}
};

bool Scaner::delimiter_proc(){
    bool t = false;
    if(-1 == state){
        state = get_init_state(ch, init_table_for_delimiters,
                               sizeof(init_table_for_delimiters)/sizeof(State_for_char));
        token.code = delim_jump_table[state].code;
        t = true;
        return t;
    }
    Elem elem = delim_jump_table[state];
    token.code = delim_jump_table[state].code;
    int y = search_char(ch, elem.symbols);
    if(y != THERE_IS_NO_CHAR){
        state = elem.first_state + y; t = true;
    }
    return t;
}

static const std::set<size_t> final_states_for_numbers = {
    1, 2, 3, 4, 5, 6, 7, 8
};

bool Scaner::number_proc(){
    bool t             = true;
    bool there_is_jump = false;
    switch(state){
        case 0:
            if(belongs(STRING10, char_categories)){
                integer_part = integer_part * 10 + digit2int(ch);
                state = 8;
                there_is_jump = true;
            }
             else if(belongs(STRING11, char_categories)){
                integer_part = integer_part * 10 + digit2int(ch);
                state = 7;
                there_is_jump = true;
            }

            break;
        case 1:
            if(belongs(STRING5, char_categories)){
                fractional_part = fractional_part / 10 + digit2int(ch); frac_part_num_digits += 1;
                state = 1;
                there_is_jump = true;
            }
             else if(belongs(NUMBER0, char_categories)){
                precision = ch; is_float = true;
                state = 15;
                there_is_jump = true;
            }

            break;
        case 2:
            if(belongs(STRING2, char_categories)){
                state = 1;
                there_is_jump = true;
            }
             else if(belongs(STRING5, char_categories)){
                fractional_part = fractional_part / 10 + digit2int(ch); frac_part_num_digits += 1;
                state = 1;
                there_is_jump = true;
            }
             else if(belongs(NUMBER0, char_categories)){
                precision = ch; is_float = true;
                state = 15;
                there_is_jump = true;
            }

            break;
        case 3:
            if(belongs(STRING2, char_categories)){
                state = 9;
                there_is_jump = true;
            }
             else if(belongs(STRING3, char_categories)){
                integer_part = (integer_part << 1) + digit2int(ch);
                state = 3;
                there_is_jump = true;
            }

            break;
        case 4:
            if(belongs(STRING2, char_categories)){
                state = 10;
                there_is_jump = true;
            }
             else if(belongs(STRING4, char_categories)){
                integer_part = (integer_part << 3) + digit2int(ch);
                state = 4;
                there_is_jump = true;
            }

            break;
        case 5:
            if(belongs(STRING2, char_categories)){
                state = 12;
                there_is_jump = true;
            }
             else if(belongs(STRING5, char_categories)){
                exponent = exponent * 10 + digit2int(ch);
                state = 5;
                there_is_jump = true;
            }

            break;
        case 6:
            if(belongs(STRING2, char_categories)){
                state = 14;
                there_is_jump = true;
            }
             else if(belongs(STRING9, char_categories)){
                integer_part = (integer_part << 4) + digit2int(ch);
                state = 6;
                there_is_jump = true;
            }

            break;
        case 7:
            if(belongs(STRING2, char_categories)){
                state = 13;
                there_is_jump = true;
            }
             else if(belongs(NUMBER1, char_categories)){
                is_float = true;
                state = 11;
                there_is_jump = true;
            }
             else if(belongs(STRING5, char_categories)){
                integer_part = integer_part * 10 + digit2int(ch);
                state = 7;
                there_is_jump = true;
            }
             else if(belongs(NUMBER0, char_categories)){
                precision = ch; is_float = true;
                state = 15;
                there_is_jump = true;
            }

            break;
        case 8:
            if(belongs(STRING2, char_categories)){
                state = 13;
                there_is_jump = true;
            }
             else if(belongs(NUMBER1, char_categories)){
                is_float = true;
                state = 11;
                there_is_jump = true;
            }
             else if(belongs(STRING5, char_categories)){
                integer_part = integer_part * 10 + digit2int(ch);
                state = 7;
                there_is_jump = true;
            }
             else if(belongs(STRING6, char_categories)){
                state = 9;
                there_is_jump = true;
            }
             else if(belongs(NUMBER0, char_categories)){
                precision = ch; is_float = true;
                state = 15;
                there_is_jump = true;
            }
             else if(belongs(STRING7, char_categories)){
                state = 14;
                there_is_jump = true;
            }
             else if(belongs(STRING8, char_categories)){
                state = 10;
                there_is_jump = true;
            }

            break;
        case 9:
            if(belongs(STRING3, char_categories)){
                integer_part = (integer_part << 1) + digit2int(ch);
                state = 3;
                there_is_jump = true;
            }

            break;
        case 10:
            if(belongs(STRING4, char_categories)){
                integer_part = (integer_part << 3) + digit2int(ch);
                state = 4;
                there_is_jump = true;
            }

            break;
        case 11:
            if(belongs(STRING5, char_categories)){
                fractional_part = fractional_part / 10 + digit2int(ch); frac_part_num_digits += 1;
                state = 2;
                there_is_jump = true;
            }

            break;
        case 12:
            if(belongs(STRING5, char_categories)){
                exponent = exponent * 10 + digit2int(ch);
                state = 5;
                there_is_jump = true;
            }

            break;
        case 13:
            if(belongs(STRING5, char_categories)){
                integer_part = integer_part * 10 + digit2int(ch);
                state = 7;
                there_is_jump = true;
            }

            break;
        case 14:
            if(belongs(STRING9, char_categories)){
                integer_part = (integer_part << 4) + digit2int(ch);
                state = 6;
                there_is_jump = true;
            }

            break;
        case 15:
            if(belongs(NUMBER2, char_categories)){
                exp_sign = setexp(ch);
                state = 12;
                there_is_jump = true;
            }
             else if(belongs(STRING5, char_categories)){
                exponent = exponent * 10 + digit2int(ch);
                state = 5;
                there_is_jump = true;
            }

            break;
        default:
            ;
    }

    if(!there_is_jump){
        t = false;
        if(!is_elem(state, final_states_for_numbers)){
            printf("At line %zu unexpectedly ended the number.", loc->current_line);
            en->increment_number_of_errors();
        }
        
          if(is_float){
            token.float_val=build_float();
            token.code = precision2code(precision);
          } else {
            token.int_val=integer_part;
            token.code = Integer;
          }
    }

    return t;
}

static const std::set<size_t> final_states_for_strings = {
    0, 1, 2,  3, 4, 5, 6, 7, 
    8, 9, 10
};

bool Scaner::string_proc(){
    bool t             = true;
    bool there_is_jump = false;
    switch(state){
        case 0:
            if(belongs(STRING0, char_categories)){
                state = 21;
                there_is_jump = true;
            }
             else if(belongs(STRING1, char_categories)){
                state = 16;
                there_is_jump = true;
            }

            break;
        case 1:
            if(belongs(STRING1, char_categories)){
                state = 16;
                there_is_jump = true;
            }
             else if(belongs(STRING2, char_categories)){
                buffer += char_code;
                state = 11;
                there_is_jump = true;
            }
             else if(belongs(STRING3, char_categories)){
                buffer += char_code;
                state = 1;
                there_is_jump = true;
            }

            break;
        case 2:
            if(belongs(STRING1, char_categories)){
                state = 16;
                there_is_jump = true;
            }
             else if(belongs(STRING2, char_categories)){
                buffer += char_code;
                state = 13;
                there_is_jump = true;
            }
             else if(belongs(STRING4, char_categories)){
                buffer += char_code;
                state = 2;
                there_is_jump = true;
            }

            break;
        case 3:
            if(belongs(STRING1, char_categories)){
                state = 16;
                there_is_jump = true;
            }
             else if(belongs(STRING2, char_categories)){
                buffer += char_code;
                state = 15;
                there_is_jump = true;
            }
             else if(belongs(STRING5, char_categories)){
                buffer += char_code;
                state = 3;
                there_is_jump = true;
            }

            break;
        case 4:
            if(belongs(STRING1, char_categories)){
                state = 16;
                there_is_jump = true;
            }
             else if(belongs(STRING2, char_categories)){
                buffer += char_code;
                state = 15;
                there_is_jump = true;
            }
             else if(belongs(STRING5, char_categories)){
                buffer += char_code;
                state = 3;
                there_is_jump = true;
            }
             else if(belongs(STRING6, char_categories)){
                buffer += char_code;
                state = 11;
                there_is_jump = true;
            }
             else if(belongs(STRING7, char_categories)){
                buffer += char_code;
                state = 19;
                there_is_jump = true;
            }
             else if(belongs(STRING8, char_categories)){
                buffer += char_code;
                state = 13;
                there_is_jump = true;
            }

            break;
        case 5:
            if(belongs(STRING1, char_categories)){
                state = 16;
                there_is_jump = true;
            }
             else if(belongs(STRING2, char_categories)){
                buffer += char_code;
                state = 19;
                there_is_jump = true;
            }
             else if(belongs(STRING9, char_categories)){
                buffer += char_code;
                state = 5;
                there_is_jump = true;
            }

            break;
        case 6:
            if(belongs(STRING0, char_categories)){
                state = 21;
                there_is_jump = true;
            }
             else if(belongs(STRING1, char_categories)){
                state = 18;
                there_is_jump = true;
            }
             else if(belongs(STRING2, char_categories)){
                buffer += char_code;
                state = 12;
                there_is_jump = true;
            }
             else if(belongs(STRING3, char_categories)){
                buffer += char_code;
                state = 6;
                there_is_jump = true;
            }

            break;
        case 7:
            if(belongs(STRING0, char_categories)){
                state = 21;
                there_is_jump = true;
            }
             else if(belongs(STRING1, char_categories)){
                state = 18;
                there_is_jump = true;
            }
             else if(belongs(STRING2, char_categories)){
                buffer += char_code;
                state = 14;
                there_is_jump = true;
            }
             else if(belongs(STRING4, char_categories)){
                buffer += char_code;
                state = 7;
                there_is_jump = true;
            }

            break;
        case 8:
            if(belongs(STRING0, char_categories)){
                state = 21;
                there_is_jump = true;
            }
             else if(belongs(STRING1, char_categories)){
                state = 18;
                there_is_jump = true;
            }
             else if(belongs(STRING2, char_categories)){
                buffer += char_code;
                state = 17;
                there_is_jump = true;
            }
             else if(belongs(STRING5, char_categories)){
                buffer += char_code;
                state = 8;
                there_is_jump = true;
            }

            break;
        case 9:
            if(belongs(STRING0, char_categories)){
                state = 21;
                there_is_jump = true;
            }
             else if(belongs(STRING1, char_categories)){
                state = 18;
                there_is_jump = true;
            }
             else if(belongs(STRING2, char_categories)){
                buffer += char_code;
                state = 17;
                there_is_jump = true;
            }
             else if(belongs(STRING5, char_categories)){
                buffer += char_code;
                state = 8;
                there_is_jump = true;
            }
             else if(belongs(STRING6, char_categories)){
                buffer += char_code;
                state = 12;
                there_is_jump = true;
            }
             else if(belongs(STRING7, char_categories)){
                buffer += char_code;
                state = 20;
                there_is_jump = true;
            }
             else if(belongs(STRING8, char_categories)){
                buffer += char_code;
                state = 14;
                there_is_jump = true;
            }

            break;
        case 10:
            if(belongs(STRING0, char_categories)){
                state = 21;
                there_is_jump = true;
            }
             else if(belongs(STRING1, char_categories)){
                state = 18;
                there_is_jump = true;
            }
             else if(belongs(STRING2, char_categories)){
                buffer += char_code;
                state = 20;
                there_is_jump = true;
            }
             else if(belongs(STRING9, char_categories)){
                buffer += char_code;
                state = 10;
                there_is_jump = true;
            }

            break;
        case 11:
            if(belongs(STRING3, char_categories)){
                buffer += char_code;
                state = 1;
                there_is_jump = true;
            }

            break;
        case 12:
            if(belongs(STRING3, char_categories)){
                buffer += char_code;
                state = 6;
                there_is_jump = true;
            }

            break;
        case 13:
            if(belongs(STRING4, char_categories)){
                buffer += char_code;
                state = 2;
                there_is_jump = true;
            }

            break;
        case 14:
            if(belongs(STRING4, char_categories)){
                buffer += char_code;
                state = 7;
                there_is_jump = true;
            }

            break;
        case 15:
            if(belongs(STRING5, char_categories)){
                buffer += char_code;
                state = 3;
                there_is_jump = true;
            }

            break;
        case 16:
            if(belongs(STRING10, char_categories)){
                buffer += char_code;
                state = 4;
                there_is_jump = true;
            }
             else if(belongs(STRING11, char_categories)){
                buffer += char_code;
                state = 3;
                there_is_jump = true;
            }

            break;
        case 17:
            if(belongs(STRING5, char_categories)){
                buffer += char_code;
                state = 8;
                there_is_jump = true;
            }

            break;
        case 18:
            if(belongs(STRING10, char_categories)){
                buffer += char_code;
                state = 9;
                there_is_jump = true;
            }
             else if(belongs(STRING11, char_categories)){
                buffer += char_code;
                state = 8;
                there_is_jump = true;
            }

            break;
        case 19:
            if(belongs(STRING9, char_categories)){
                buffer += char_code;
                state = 5;
                there_is_jump = true;
            }

            break;
        case 20:
            if(belongs(STRING9, char_categories)){
                buffer += char_code;
                state = 10;
                there_is_jump = true;
            }

            break;
        case 21:
            if(ch != U'\"'){
                buffer += ch;
                state = 21;
                there_is_jump = true;
            }
             else if(belongs(STRING0, char_categories)){
                state = 22;
                there_is_jump = true;
            }

            break;
        case 22:
            if(belongs(STRING0, char_categories)){
                buffer += ch;
                state = 21;
                there_is_jump = true;
            }
             else if(belongs(STRING1, char_categories)){
                state = 18;
                there_is_jump = true;
            }

            break;
        default:
            ;
    }

    if(!there_is_jump){
        t = false;
        if(!is_elem(state, final_states_for_strings)){
            printf("At line %zu unexpectedly ended a string literal.", loc->current_line);
            en->increment_number_of_errors();
        }
        token.code=(buffer.length()==1)?Char:String;
            token.string_index = strs -> insert(buffer);
    }

    return t;
}

void Scaner::none_proc(){
    /* This subroutine will be called if, after reading the input text, it turned
     * out to be in the A_start automaton. Then you do not need to do anything. */
}

void Scaner::unknown_final_proc(){
    /* This subroutine will be called if, after reading the input text, it turned
     * out to be in the A_unknown automaton. Then you do not need to do anything. */
}

void Scaner::idkeyword_final_proc(){
    if(!is_elem(state, final_states_for_idkeywords)){
        printf("At line %zu unexpectedly ended identifier or keyword.", loc->current_line);
        en->increment_number_of_errors();
    }

    int search_result = search_keyword(buffer);
    if(search_result != THERE_IS_NO_KEYWORD) {
        token.code = kwlist[search_result].kw_code;
    }

}

void Scaner::delimiter_final_proc(){
        
    token.code = delim_jump_table[state].code;
    
}

void Scaner::number_final_proc(){
    if(!is_elem(state, final_states_for_numbers)){
        printf("At line %zu unexpectedly ended the number.", loc->current_line);
        en->increment_number_of_errors();
    }
    
          if(is_float){
            token.float_val=build_float();
            token.code = precision2code(precision);
          } else {
            token.int_val=integer_part;
            token.code = Integer;
          }
}

void Scaner::string_final_proc(){
    if(!is_elem(state, final_states_for_strings)){
        printf("At line %zu unexpectedly ended a string literal.", loc->current_line);
        en->increment_number_of_errors();
    }
    token.code=(buffer.length()==1)?Char:String;
    token.string_index = strs -> insert(buffer);
}

Lexem_info Scaner::current_lexem(){
    automaton = A_start; token.code = None;
    lexem_begin = loc->pcurrent_char;
    bool t = true;
    while((ch = *(loc->pcurrent_char)++)){
        char_categories = get_categories_set(ch); //categories_table[ch];
        t = (this->*procs[automaton])();
        if(!t){
            /* We get here only if the lexeme has already been read. At the same time,
             * the current automaton reads the character immediately after the end of
             * the token read, based on this symbol, it is decided that the token has
             * been read and the transition to the next character has been made.
             * Therefore, in order to not miss the first character of the next lexeme,
             * we need to decrease the pcurrent_char pointer by one. */
            (loc->pcurrent_char)--;
            return token;
        }
    }
    /* Here we can be, only if we have already read all the processed text. In this
     * case, the pointer to the current symbol indicates a byte, which is immediately
     * after the zero character, which is a sign of the end of the text. To avoid
     * entering subsequent calls outside the text, we need to go back to the null
     * character. */
    (loc->pcurrent_char)--;
    /* Further, since we are here, the end of the current token (perhaps unexpected)
     * has not yet been processed. It is necessary to perform this processing, and,
     * probably, to display any diagnostics. */
    (this->*finals[automaton])();
    return token;
}


