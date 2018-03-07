#include "../include/myfirstscaner.h"
#include "../include/get_init_state.h"
#include "../include/search_char.h"
#include "../include/belongs.h"
#include <set>
#include <string>
#include <vector>
#include "../include/operations_with_sets.h"

#include <quadmath> 
size_t digit2int(char32_t ch)
{
size_t v =  ch - U'0';
return (v<=9)? v:(v&0b1101'1111) - 7;
};
__float128 myfirstscaner::build_float()
{
__float128 num;                                                                               
num = int_value + fract_part*powq(10, num_of_digits);
num = num*powq(10, sign_of_degree*exponent);
return num;}
myfirstscaner::Automaton_proc myfirstscaner::procs[] = {
    &myfirstscaner::start_proc(),     &myfirstscaner::unknown_proc(),   
    &myfirstscaner::idkeyword_proc(), &myfirstscaner::delimiter_proc(), 
    &myfirstscaner::number_proc(),    &myfirstscaner::string_proc()
};

myfirstscaner::Final_proc myfirstscaner::finals[] = {
    &myfirstscaner::none_proc(),            &myfirstscaner::unknown_final_proc(),   
    &myfirstscaner::idkeyword_final_proc(), &myfirstscaner::delimiter_final_proc(), 
    &myfirstscaner::number_final_proc(),    &myfirstscaner::string_final_proc()
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
    IDKEYWORD3,      IDKEYWORD4,      
    IDKEYWORD5,      IDKEYWORD6,      
    IDKEYWORD7,      IDKEYWORD8,      
    IDKEYWORD9,      IDKEYWORD10,     
    IDKEYWORD11,     IDKEYWORD12,     
    IDKEYWORD13,     IDKEYWORD14,     
    IDKEYWORD15,     Other
};

static const std::map<char32_t, uint64_t> categories_table = {
    {'\0', 1},          {'\X01', 1},        {'\X02', 1},        {'\X03', 1},        
    {'\X04', 1},        {'\X05', 1},        {'\X06', 1},        {'\a', 1},          
    {'\b', 1},          {'\t', 1},          {'\n', 1},          {'\v', 1},          
    {'\f', 1},          {'\r', 1},          {'\X0e', 1},        {'\X0f', 1},        
    {'\X10', 1},        {'\X11', 1},        {'\X12', 1},        {'\X13', 1},        
    {'\X14', 1},        {'\X15', 1},        {'\X16', 1},        {'\X17', 1},        
    {'\X18', 1},        {'\X19', 1},        {'\X1a', 1},        {'\X1b', 1},        
    {'\X1c', 1},        {'\X1d', 1},        {'\X1e', 1},        {'\X1f', 1},        
    {' ', 536870913},   {'!', 2},           {", 12},            {'#', 2},           
    {'$', 24},          {'%', 2},           {'&', 2},           {', 32},            
    {'*', 2},           {'+', 2},           {',', 2},           {'-', 65538},       
    {'.', 131072},      {'/', 2},           {'0', 1262494656},  {'1', 1266705280},  
    {'2', 1266705152},  {'3', 1266705152},  {'4', 1266705152},  {'5', 1262510848},  
    {'6', 1266705152},  {'7', 1262510848},  {'8', 1266704896},  {'9', 1262510592},  
    {':', 2},           {';', 2},           {'<', 2},           {'=', 2},           
    {'>', 2},           {'?', 2},           {'A', 1263280128},  {'B', 1263281152},  
    {'C', 1263280128},  {'D', 1263312896},  {'E', 1263312896},  {'F', 1263280128},  
    {'G', 1263271936},  {'H', 1263271936},  {'I', 1263271936},  {'J', 1263271936},  
    {'K', 1263271936},  {'L', 1263271936},  {'M', 1263271936},  {'N', 1263271936},  
    {'O', 1263271936},  {'P', 1263271936},  {'Q', 1263304704},  {'R', 1263271936},  
    {'S', 1263304704},  {'T', 1263271936},  {'U', 1263271936},  {'V', 1263271936},  
    {'W', 1263271936},  {'X', 1263273984},  {'Y', 1263271936},  {'Z', 1263271936},  
    {'[', 2},           {'^', 2},           {'_', 1267466240},  {'a', 1267998720},  
    {'b', 1263805440},  {'c', 1267998720},  {'d', 1267507200},  {'e', 1268031488},  
    {'f', 1267998720},  {'g', 1401683968},  {'h', 18447335424}, {'i', 1267990528},  
    {'j', 1263271936},  {'k', 1263271936},  {'l', 1269039104},  {'m', 1267990528},  
    {'n', 1301020672},  {'o', 1250693120},  {'p', 1267990528},  {'q', 1263304704},  
    {'r', 9857925120},  {'s', 5562990592},  {'t', 3415474176},  {'u', 1267990528},  
    {'v', 1263796224},  {'w', 1263796224},  {'x', 1267468288},  {'y', 1267466240},  
    {'z', 1263271936},  {'{', 2},           {'|', 2},           {'}', 2},           
    {'~', 2},           {'Ё', 1263271936}, {'А', 1263271936}, {'Б', 1263271936}, 
    {'В', 1263271936}, {'Г', 1263271936}, {'Д', 1263271936}, {'Е', 1263271936}, 
    {'Ж', 1263271936}, {'З', 1263271936}, {'И', 1263271936}, {'Й', 1263271936}, 
    {'К', 1263271936}, {'Л', 1263271936}, {'М', 1263271936}, {'Н', 1263271936}, 
    {'О', 1263271936}, {'П', 1263271936}, {'Р', 1263271936}, {'С', 1263271936}, 
    {'Т', 1263271936}, {'У', 1263271936}, {'Ф', 1263271936}, {'Х', 1263271936}, 
    {'Ц', 1263271936}, {'Ч', 1263271936}, {'Ш', 1263271936}, {'Щ', 1263271936}, 
    {'Ъ', 1263271936}, {'Ы', 1263271936}, {'Ь', 1263271936}, {'Э', 1263271936}, 
    {'Ю', 1263271936}, {'Я', 1263271936}, {'а', 1263271936}, {'б', 1263271936}, 
    {'в', 1263271936}, {'г', 1263271936}, {'д', 1263271936}, {'е', 1263271936}, 
    {'ж', 1263271936}, {'з', 1263271936}, {'и', 1263271936}, {'й', 1263271936}, 
    {'к', 1263271936}, {'л', 1263271936}, {'м', 1263271936}, {'н', 1263271936}, 
    {'о', 1263271936}, {'п', 1263271936}, {'р', 1263271936}, {'с', 1263271936}, 
    {'т', 1263271936}, {'у', 1263271936}, {'ф', 1263271936}, {'х', 1263271936}, 
    {'ц', 1263271936}, {'ч', 1263271936}, {'ш', 1263271936}, {'щ', 1263271936}, 
    {'ъ', 1263271936}, {'ы', 1263271936}, {'ь', 1263271936}, {'э', 1263271936}, 
    {'ю', 1263271936}, {'я', 1263271936}, {'ё', 1263271936}
};


uint64_t get_categories_set(char32_t c){
    auto it = categories_table.find(c);
    if(it != categories_table.end()){
        return it->second;
    }else{
        return 1ULL << Other;
    }
}
bool myfirstscaner::start_proc(){
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

bool myfirstscaner::unknown_proc(){
    return belongs(Other, char_categories);
}

struct Keyword_list_elem{
    std::u32string keyword;
    codes kw_code;
};

static const Keyword_list_elem kwlist[] = {
    {U"array", k6},       {U"bool", k11},       
    {U"bool16_t", k30},   {U"bool32_t", k31},   
    {U"bool64_t", k32},   {U"bool8_t", k29},    
    {U"byte", k15},       {U"char", k12},       
    {U"char16_t", k34},   {U"char32_t", k35},   
    {U"chat8_t", k33},    {U"const", k41},      
    {U"elif", k49},       {U"else", k50},       
    {U"endif", k51},      {U"enum", k42},       
    {U"export", k3},      {U"false", k46},      
    {U"float", k9},       {U"float128_t", k28}, 
    {U"float32_t", k26},  {U"float64_t", k27},  
    {U"fn", k44},         {U"for", k52},        
    {U"if", k47},         {U"import", k2},      
    {U"int", k8},         {U"int128_t", k20},   
    {U"int16_t", k17},    {U"int32_t", k18},    
    {U"int64_t", k19},    {U"int8_t", k16},     
    {U"long short", k7},  {U"match", k58},      
    {U"module", k1},      {U"print", k57},      
    {U"read", k56},       {U"ref", k40},        
    {U"repeat", k54},     {U"string", k13},     
    {U"string16_t", k37}, {U"string32_t", k38}, 
    {U"string8_t", k36},  {U"struct", k43},     
    {U"then", k48},       {U"true", k45},       
    {U"type", k5},        {U"uint", k10},       
    {U"uint128_t", k25},  {U"uint16_t", k22},   
    {U"uint32_t", k23},   {U"uint64_t", k24},   
    {U"uint8_t", k21},    {U"until", k55},      
    {U"val", k39},        {U"var", k4},         
    {U"void", k14},       {U"while", k53}
};

#define NUM_OF_KEYWORDS 58

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
    1, 2, 3, 4, 5, 6
};

bool myfirstscaner::idkeyword_proc(){
    bool t             = true;
    bool there_is_jump = false;
    switch(state){
        case 0:
            if(belongs(IDKEYWORD0, char_categories)){
                state = 2;
                there_is_jump = true;
            }
             else if(belongs(IDKEYWORD1, char_categories)){
                buffer += ch;
                state = 2;
                there_is_jump = true;
            }
             else if(belongs(IDKEYWORD2, char_categories)){
                buffer += ch;
                state = 3;
                there_is_jump = true;
            }

            break;
        case 2:
            if(belongs(IDKEYWORD3, char_categories)){
                state = 2;
                there_is_jump = true;
            }
             else if(belongs(IDKEYWORD4, char_categories)){
                buffer += ch;
                state = 2;
                there_is_jump = true;
            }

            break;
        case 3:
            if(belongs(IDKEYWORD5, char_categories)){
                state = 2;
                there_is_jump = true;
            }
             else if(belongs(STRING8, char_categories)){
                buffer += ch;
                state = 4;
                there_is_jump = true;
            }

            break;
        case 4:
            if(belongs(IDKEYWORD6, char_categories)){
                state = 2;
                there_is_jump = true;
            }
             else if(belongs(IDKEYWORD7, char_categories)){
                buffer += ch;
                state = 5;
                there_is_jump = true;
            }

            break;
        case 5:
            if(belongs(IDKEYWORD8, char_categories)){
                state = 2;
                there_is_jump = true;
            }
             else if(belongs(IDKEYWORD9, char_categories)){
                buffer += ch;
                state = 6;
                there_is_jump = true;
            }

            break;
        case 6:
            if(belongs(IDKEYWORD10, char_categories)){
                buffer += ch;
                state = 8;
                there_is_jump = true;
            }
             else if(belongs(IDKEYWORD11, char_categories)){
                state = 2;
                there_is_jump = true;
            }

            break;
        case 7:
            if(belongs(IDKEYWORD12, char_categories)){
                buffer += ch;
                state = 1;
                there_is_jump = true;
            }

            break;
        case 8:
            if(belongs(IDKEYWORD13, char_categories)){
                buffer += ch;
                state = 11;
                there_is_jump = true;
            }

            break;
        case 9:
            if(belongs(IDKEYWORD14, char_categories)){
                buffer += ch;
                state = 7;
                there_is_jump = true;
            }

            break;
        case 10:
            if(belongs(STRING8, char_categories)){
                buffer += ch;
                state = 9;
                there_is_jump = true;
            }

            break;
        case 11:
            if(belongs(IDKEYWORD15, char_categories)){
                buffer += ch;
                state = 10;
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
    {4, U'!'},  {38, U'#'}, {37, U'%'}, {14, U'&'}, {32, U'*'}, 
    {30, U'+'}, {49, U','}, {31, U'-'}, {35, U'/'}, {40, U':'}, 
    {50, U';'}, {16, U'<'}, {25, U'='}, {21, U'>'}, {0, U'?'},  
    {47, U'['}, {12, U'^'}, {45, U'{'}, {1, U'|'},  {46, U'}'}, 
    {26, U'~'}
};

struct Elem {
    /** A pointer to a string of characters that can be crossed. */
    char32_t*       symbols;
    /** A lexeme code. */
    codes code;
    /** If the current character matches symbols[0], then the transition to the state
     *  first_state;
     *  if the current character matches symbols[1], then the transition to the state
     *  first_state + 1;
     *  if the current character matches symbols[2], then the transition to the state
     *  first_state + 2, and so on. */
    uint16_t        first_state;
};

static const Elem delim_jump_table[] = {
    {const_cast<char32_t*>(U""), d1, 0},        
    {const_cast<char32_t*>(U"|:"), d15, 2},     
    {const_cast<char32_t*>(U""), d2, 0},        
    {const_cast<char32_t*>(U""), d37, 0},       
    {const_cast<char32_t*>(U"|^&="), d8, 5},    
    {const_cast<char32_t*>(U"|"), Unknown, 6},  
    {const_cast<char32_t*>(U"^"), Unknown, 7},  
    {const_cast<char32_t*>(U"&"), Unknown, 8},  
    {const_cast<char32_t*>(U""), d14, 0},       
    {const_cast<char32_t*>(U""), d3, 0},        
    {const_cast<char32_t*>(U""), d5, 0},        
    {const_cast<char32_t*>(U""), d7, 0},        
    {const_cast<char32_t*>(U"^"), d17, 13},     
    {const_cast<char32_t*>(U""), d4, 0},        
    {const_cast<char32_t*>(U"&"), d19, 15},     
    {const_cast<char32_t*>(U""), d6, 0},        
    {const_cast<char32_t*>(U"=<-:"), d9, 17},   
    {const_cast<char32_t*>(U""), d11, 0},       
    {const_cast<char32_t*>(U""), d21, 0},       
    {const_cast<char32_t*>(U""), d41, 0},       
    {const_cast<char32_t*>(U""), d46, 0},       
    {const_cast<char32_t*>(U"=>"), d10, 22},    
    {const_cast<char32_t*>(U""), d12, 0},       
    {const_cast<char32_t*>(U">"), d22, 24},     
    {const_cast<char32_t*>(U""), d23, 0},       
    {const_cast<char32_t*>(U""), d13, 0},       
    {const_cast<char32_t*>(U"|^&"), d24, 27},   
    {const_cast<char32_t*>(U""), d16, 0},       
    {const_cast<char32_t*>(U""), d18, 0},       
    {const_cast<char32_t*>(U""), d20, 0},       
    {const_cast<char32_t*>(U""), d25, 0},       
    {const_cast<char32_t*>(U""), d26, 0},       
    {const_cast<char32_t*>(U"*"), d27, 33},     
    {const_cast<char32_t*>(U"."), d31, 34},     
    {const_cast<char32_t*>(U""), d32, 0},       
    {const_cast<char32_t*>(U"."), d28, 36},     
    {const_cast<char32_t*>(U""), d30, 0},       
    {const_cast<char32_t*>(U""), d29, 0},       
    {const_cast<char32_t*>(U"#"), d35, 39},     
    {const_cast<char32_t*>(U""), d36, 0},       
    {const_cast<char32_t*>(U":]>="), d48, 41},  
    {const_cast<char32_t*>(U""), d38, 0},       
    {const_cast<char32_t*>(U""), d43, 0},       
    {const_cast<char32_t*>(U""), d47, 0},       
    {const_cast<char32_t*>(U""), d49, 0},       
    {const_cast<char32_t*>(U""), d39, 0},       
    {const_cast<char32_t*>(U""), d40, 0},       
    {const_cast<char32_t*>(U":"), Unknown, 48}, 
    {const_cast<char32_t*>(U""), d42, 0},       
    {const_cast<char32_t*>(U""), d44, 0},       
    {const_cast<char32_t*>(U""), d45, 0}
};

bool myfirstscaner::delimiter_proc(){
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
    1, 2,  3, 4, 5, 6, 7, 8, 
    9, 10
};

bool myfirstscaner::number_proc(){
    bool t             = true;
    bool there_is_jump = false;
    switch(state){
        case 0:
            if(belongs(STRING10, char_categories)){
                integer_part = integer_part * 10 + digit2int(ch);
                state = 10;
                there_is_jump = true;
            }
             else if(belongs(STRING11, char_categories)){
                integer_part = integer_part * 10 + digit2int(ch);
                state = 9;
                there_is_jump = true;
            }

            break;
        case 1:
            if(belongs(NUMBER0, char_categories)){
                precision = ch; is_float = true;
                state = 1;
                there_is_jump = true;
            }

            break;
        case 2:
            if(belongs(NUMBER1, char_categories)){
                exp_sign = setexp(ch);
                state = 14;
                there_is_jump = true;
            }
             else if(belongs(STRING5, char_categories)){
                fractional_part = fractional_part / 10 + digit2int(ch); frac_part_num_digits += 1;
                state = 8;
                there_is_jump = true;
            }
             else if(belongs(NUMBER0, char_categories)){
                precision = ch; is_float = true;
                state = 1;
                there_is_jump = true;
            }

            break;
        case 3:
            if(belongs(STRING2, char_categories)){
                state = 2;
                there_is_jump = true;
            }
             else if(belongs(NUMBER1, char_categories)){
                exp_sign = setexp(ch);
                state = 14;
                there_is_jump = true;
            }
             else if(belongs(STRING5, char_categories)){
                fractional_part = fractional_part / 10 + digit2int(ch); frac_part_num_digits += 1;
                state = 8;
                there_is_jump = true;
            }
             else if(belongs(NUMBER0, char_categories)){
                precision = ch; is_float = true;
                state = 1;
                there_is_jump = true;
            }

            break;
        case 4:
            if(belongs(STRING2, char_categories)){
                state = 11;
                there_is_jump = true;
            }
             else if(belongs(STRING3, char_categories)){
                integer_part = (integer_part << 1) + digit2int(ch);
                state = 4;
                there_is_jump = true;
            }

            break;
        case 5:
            if(belongs(STRING2, char_categories)){
                state = 12;
                there_is_jump = true;
            }
             else if(belongs(STRING4, char_categories)){
                integer_part = (integer_part << 3) + digit2int(ch);
                state = 5;
                there_is_jump = true;
            }

            break;
        case 6:
            if(belongs(STRING2, char_categories)){
                state = 14;
                there_is_jump = true;
            }
             else if(belongs(STRING5, char_categories)){
                exponent = exponent * 10 + digit2int(ch);
                state = 6;
                there_is_jump = true;
            }

            break;
        case 7:
            if(belongs(STRING2, char_categories)){
                state = 16;
                there_is_jump = true;
            }
             else if(belongs(STRING9, char_categories)){
                integer_part = (integer_part << 4) + digit2int(ch);
                state = 7;
                there_is_jump = true;
            }

            break;
        case 8:
            if(belongs(STRING2, char_categories)){
                state = 14;
                there_is_jump = true;
            }
             else if(belongs(NUMBER1, char_categories)){
                exp_sign = setexp(ch);
                state = 14;
                there_is_jump = true;
            }
             else if(belongs(STRING5, char_categories)){
                fractional_part = fractional_part / 10 + digit2int(ch); frac_part_num_digits += 1;
                state = 8;
                there_is_jump = true;
            }
             else if(belongs(NUMBER0, char_categories)){
                precision = ch; is_float = true;
                state = 1;
                there_is_jump = true;
            }

            break;
        case 9:
            if(belongs(STRING2, char_categories)){
                state = 15;
                there_is_jump = true;
            }
             else if(belongs(NUMBER1, char_categories)){
                exp_sign = setexp(ch);
                state = 14;
                there_is_jump = true;
            }
             else if(belongs(NUMBER2, char_categories)){
                is_float = true;
                state = 13;
                there_is_jump = true;
            }
             else if(belongs(STRING5, char_categories)){
                integer_part = integer_part * 10 + digit2int(ch);
                state = 9;
                there_is_jump = true;
            }
             else if(belongs(NUMBER0, char_categories)){
                precision = ch; is_float = true;
                state = 1;
                there_is_jump = true;
            }

            break;
        case 10:
            if(belongs(STRING2, char_categories)){
                state = 15;
                there_is_jump = true;
            }
             else if(belongs(NUMBER1, char_categories)){
                exp_sign = setexp(ch);
                state = 14;
                there_is_jump = true;
            }
             else if(belongs(NUMBER2, char_categories)){
                is_float = true;
                state = 13;
                there_is_jump = true;
            }
             else if(belongs(STRING5, char_categories)){
                integer_part = integer_part * 10 + digit2int(ch);
                state = 9;
                there_is_jump = true;
            }
             else if(belongs(STRING6, char_categories)){
                state = 11;
                there_is_jump = true;
            }
             else if(belongs(NUMBER0, char_categories)){
                precision = ch; is_float = true;
                state = 1;
                there_is_jump = true;
            }
             else if(belongs(STRING7, char_categories)){
                state = 16;
                there_is_jump = true;
            }
             else if(belongs(STRING8, char_categories)){
                state = 12;
                there_is_jump = true;
            }

            break;
        case 11:
            if(belongs(STRING3, char_categories)){
                integer_part = (integer_part << 1) + digit2int(ch);
                state = 4;
                there_is_jump = true;
            }

            break;
        case 12:
            if(belongs(STRING4, char_categories)){
                integer_part = (integer_part << 3) + digit2int(ch);
                state = 5;
                there_is_jump = true;
            }

            break;
        case 13:
            if(belongs(STRING5, char_categories)){
                fractional_part = fractional_part / 10 + digit2int(ch); frac_part_num_digits += 1;
                state = 3;
                there_is_jump = true;
            }

            break;
        case 14:
            if(belongs(STRING5, char_categories)){
                exponent = exponent * 10 + digit2int(ch);
                state = 6;
                there_is_jump = true;
            }

            break;
        case 15:
            if(belongs(STRING5, char_categories)){
                integer_part = integer_part * 10 + digit2int(ch);
                state = 9;
                there_is_jump = true;
            }

            break;
        case 16:
            if(belongs(STRING9, char_categories)){
                integer_part = (integer_part << 4) + digit2int(ch);
                state = 7;
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

bool myfirstscaner::string_proc(){
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

void myfirstscaner::none_proc(){
    /* This subroutine will be called if, after reading the input text, it turned
     * out to be in the A_start automaton. Then you do not need to do anything. */
}

void myfirstscaner::unknown_final_proc(){
    /* This subroutine will be called if, after reading the input text, it turned
     * out to be in the A_unknown automaton. Then you do not need to do anything. */
}

void myfirstscaner::idkeyword_final_proc(){
    if(!is_elem(state, final_states_for_idkeywords)){
        printf("At line %zu unexpectedly ended identifier or keyword.", loc->current_line);
        en->increment_number_of_errors();
    }

    int search_result = search_keyword(buffer);
    if(search_result != THERE_IS_NO_KEYWORD) {
        token.code = kwlist[search_result].kw_code;
    }

}

void myfirstscaner::delimiter_final_proc(){
        
    token.code = delim_jump_table[state].code;
    
}

void myfirstscaner::number_final_proc(){
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

void myfirstscaner::string_final_proc(){
    if(!is_elem(state, final_states_for_strings)){
        printf("At line %zu unexpectedly ended a string literal.", loc->current_line);
        en->increment_number_of_errors();
    }
    token.code=(buffer.length()==1)?Char:String;
    token.string_index = strs -> insert(buffer);
}

Lexem_info myfirstscaner::current_lexem(){
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


