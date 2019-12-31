/* @Author: Jonathan Wang
 * @Date: 27th Nov., 2019
 * @Developing Environment: Manjaro 18.1.3 Juhraya
 * */
#include <stack>
#include <deque>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <string>
using namespace std;

/* 0    <constant>
 * 1    main
 * 2    int
 * 3    if
 * 4    else
 * 5    while
 * 6    do
 * 7    +
 * 8    -
 * 9    *
 * 10   /
 * 11   <
 * 12   >
 * 13   !=
 * 14   >=
 * 15   <=
 * 16   ==
 * 17   ,
 * 18   ;
 * 19   (
 * 20   )
 * 21   {
 * 22   }
 * 23   =
 * 24   <Identifiers>
 * */

#define ID_CONSTANT 0
#define ID_MAIN 1
#define ID_INT 2
#define ID_IF 3
#define ID_ELSE 4
#define ID_WHILE 5
#define ID_DO 6
#define ID_PLUS 7
#define ID_MINUS 8
#define ID_MUL 9
#define ID_DIV 10
#define ID_LT 11
#define ID_GT 12
#define ID_NE 13
#define ID_GE 14
#define ID_LE 15
#define ID_EQ 16
#define ID_COMMA 17
#define ID_SEMICOLON 18
#define ID_LPAREN 19
#define ID_RPAREN 20
#define ID_LBRACE 21
#define ID_RBRACE 22
#define ID_GIVE 23
#define ID_IDENTIFIER 24

string reserved_words[6] = {"main", "int", "if", "else", "while", "do"};
int reserved_nums[6] = {1, 2, 3, 4, 5, 6};
string symbols[17] = {"+", "-", "*", "/", "<", ">", "!=", ">=", "<=", "==", ",", ";", "(", ")", "{", "}", "="};
int symbol_nums[17] = {7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};
string error_check[25] = {"<Constant>", "main", "int", "if", "else", "while", "do", "+", "-", "*", "/", "<", ">", "!=", ">=", "<=", "==", ",", ";", "(", ")", "{", "}", "=", "<Identifier>"};

struct word {
    string value;
    int type;
    int line;
};
word next_word;
struct quaternary {
    string op;
    string arg1;
    string arg2;
    string res;
}qua[105];
int qua_num = 0;
string letters[5005];
int file_length;        // total file length
int cur;                // current pointer position
int cur_line = 1;       // current line
string cur_word;        // current word
int lookahead_type;     // type of current word
string var[200];
int var_num = 0;
int tmpvar_num = 0;
int follow_loop = 0;
bool have_loop = false;
stack<int> condition_stack;
stack<int> else_stack;
stack<int> loop_stack;
stack<int> loop_start_stack;
bool have_error = false;


/************************* Lexical Analysis *************************/
int is_reserved(string s) {
    int i;
    for (i = 0; i < 6; i++) {
        if (s == reserved_words[i])
            return reserved_nums[i];
    }
    return 0;
}

int is_symbol(string s) {
    int i;
    for (i = 0; i < 17; i++) {
        if (s == symbols[i])
            return symbol_nums[i];
    }
    return 0;
}

bool is_digit(string s) {
    if ("0" <= s && s <= "9")
        return true;
    return false;
}

bool is_letter(string s) {
    if ("a" <= s && s <= "z")
        return true;
    return false;
}

int char_type(string s) {
    if (is_letter(s))
        return 1;
    if (is_digit(s))
        return 2;
    if (s == ">" || s == "=" || s == "<" || s == "!" || 
        s == "," || s == ";" || s == "(" || s == ")" || 
        s == "{" || s == "}" || s == "+" || s == "-" || 
        s == "*" || s == "/")
        return 3;
    return 0;
}

string concat_reserved(string s, int n) {
    int i = n + 1;
    int flag = 1;

    while (flag) {
        if (is_digit(letters[i]) || is_letter(letters[i])) {
            s = (s + letters[i]).c_str();
            if (is_reserved(s)) {
                i++;
                cur = i;
                return s;
            }
            i++;
        }else{
            flag = 0;
        }
    }
    cur = i;
    return s;
}

string concat_symbol(string s, int n) {
    int i = n + 1;
    string ss = letters[i];
    if (ss == ">" || ss == "=" || ss == "<" || ss == "!") {
        s = (s + letters[i]).c_str();
        i++;
    }
    cur = i;
    return s;
}

string concat_digit(string s, int n) {
    int i = n + 1;
    int flag = 1;

    while (flag) {
        if (is_digit(letters[i])) {
            s = (s + letters[i]).c_str();
            i++;
        }else{
            flag = 0;
        }
    }
    cur = i;
    return s;
}


void take_word() {
    int ty;
    if (cur < file_length) {
        string s1, s2;
        s1 = letters[cur];
        while (s1 == " " || s1 == "\n" || s1 == "\t") {
            if (s1 == "\n") {
                cur_line++;
            }
            if (cur + 1 >= file_length)
                return;
            s1 = letters[++cur];
        }
        ty = char_type(s1);
        switch (ty) {
            case 1:
                s2 = concat_reserved(s1, cur);
                next_word.value = s2;
                next_word.line = cur_line;
                if (is_reserved(s2)) {
                    next_word.type = is_reserved(s2);
                }else{
                    if (next_word.type == ID_CONSTANT) {
                        cout << "******************** Error *******************" << endl;
                        cout << "Line " << cur_line << ": " << endl;
                        cout << "Identifier can not start with a digit!" << endl;
                        have_error = true;
                        cout << "******************** Error *******************" << endl;
                    }
                    next_word.type = 24;
                }
                break;
            case 2:
                s2 = concat_digit(s1, cur);
                next_word.value = s2;
                next_word.line = cur_line;
                next_word.type = 0;
                break;
            case 3:
                s2 = concat_symbol(s1, cur);
                next_word.value = s2;
                next_word.line = cur_line;
                next_word.type = is_symbol(s2);
                break;
            default:
                // cout << "@line: " << cur_line << "type error" << endl;
                // exit(0);
                have_error = true;
                break;
        }
    }
}


/************************* Syntax Analysis *************************/
void match_word(int expected_type) {
    if (lookahead_type != expected_type) {
        cout << "******************** Error *******************" << endl;
        cout << "Line " << cur_line << ": " << endl;
        cout << "Syntax error! Expect ";
        if (expected_type == 0 || expected_type == 24) {
            cout << error_check[expected_type] << " \"" << next_word.value << "\"";
        }else{
            cout << "\"" << error_check[expected_type] << "\"";
        }
        cout << ", but get \"" << next_word.value << "\"." << endl;
        // exit(0);
        have_error = true;
        cout << "******************** Error *******************" << endl;
        take_word();
        lookahead_type = next_word.type;
    }else {
        take_word();
        lookahead_type = next_word.type;
    }
}

bool is_exist_var(string query) {
    int i;
    for (i = 0; i < var_num; i++) {
        if (var[i] == query)
            return true;
    }
    return false;
}

void parse_IDENTIFIER();
void parse_RESERVED();
void parse_LOOP_SEN();
void parse_CONDITION_SEN();
void parse_SEN_ONE();
void parse_MULTISEN();
string parse_FACTOR();
string parse_ITEM1(string arg1);
string parse_ITEM();
string parse_EXPRESSION1(string arg1);
string parse_EXPRESSION();
int parse_RELATION_OPT();
pair<int, pair<string, string>> parse_CONDITION();
void parse_ASSIGN_SEN();
void parse_SEN();
void parse_SEN_PART1();
void parse_SEN_PART();
void parse_IDENTIFIER_TAB1();
void parse_IDENTIFIER_TAB();
void parse_VAR_DECLARATION();
void parse_VAR_DECLARATION_PART();
void parse_MAIN_FUNC();
void parse_PROGRAM();

void parse_RESERVED() {
    cout << "parse_RESERVED" << endl;
    if (lookahead_type == ID_MAIN || 
        lookahead_type == ID_IF || 
        lookahead_type == ID_ELSE || 
        lookahead_type == ID_WHILE || 
        lookahead_type == ID_DO || 
        lookahead_type == ID_INT) {
        take_word();
        lookahead_type = next_word.type;
    }
}

void parse_IDENTIFIER() {
    cout << "parse_IDENTIFIER" << endl;
    if (lookahead_type == ID_IDENTIFIER) {
        var[var_num++] = next_word.value;
        take_word();
        lookahead_type = next_word.type;
    }else{
        cout << "******************** Error *******************" << endl;
        cout << "Line " << next_word.line << ": " << endl;
        cout << "Wrong identifier " << next_word.value << endl;
        // exit(0);
        have_error = true;
        cout << "******************** Error *******************" << endl;
    }
}

void parse_LOOP_SEN() {
    cout << "parse_LOOP_SEN" << endl;
    match_word(ID_WHILE);
    match_word(ID_LPAREN);
    loop_start_stack.push(qua_num);
    pair<int, pair<string, string>> args = parse_CONDITION();
    switch (args.first) {
        case ID_EQ:
            qua[qua_num].op = "!=";
            break;
        case ID_NE:
            qua[qua_num].op = "==";
            break;
        case ID_GT:
            qua[qua_num].op = "<=";
            break;
        case ID_GE:
            qua[qua_num].op = "<";
            break;
        case ID_LT:
            qua[qua_num].op = ">=";
            break;
        case ID_LE:
            qua[qua_num].op = ">";
            break;
        default:
            cout << "******************** Error *******************" << endl;
            cout << "Not a relation operater!" << endl;
            // exit(0);
            have_error = true;
            cout << "******************** Error *******************" << endl;
            break;
    }
    qua[qua_num].arg1 = args.second.first;
    qua[qua_num].arg2 = args.second.second;
    loop_stack.push(qua_num);
    qua_num++;
    match_word(ID_RPAREN);
    match_word(ID_DO);
    parse_SEN_ONE();
    int loop_pos = loop_stack.top();
    loop_stack.pop();
    qua[loop_pos].res = "(" + to_string(qua_num+2) + ")";
    qua[qua_num].op = "GOTO";
    qua[qua_num].arg1 = "";
    qua[qua_num].arg2 = "";
    int loop_start_pos = loop_start_stack.top();
    loop_start_stack.pop();
    qua[qua_num].res = "(" + to_string(loop_start_pos+1) + ")";
    qua_num++;
    qua[qua_num].op = "";
    qua[qua_num].arg1 = "";
    qua[qua_num].arg2 = "";
    qua[qua_num].res = "";
    // qua_num++;
    follow_loop = qua_num;
    have_loop = true;
}

void parse_CONDITION_SEN() {
    cout << "parse_CONDITION_SEN" << endl;
    match_word(ID_IF);
    match_word(ID_LPAREN);
    pair<int, pair<string, string>> args = parse_CONDITION();
    switch (args.first) {
        case ID_EQ:
            qua[qua_num].op = "!=";
            break;
        case ID_NE:
            qua[qua_num].op = "==";
            break;
        case ID_GT:
            qua[qua_num].op = "<=";
            break;
        case ID_GE:
            qua[qua_num].op = "<";
            break;
        case ID_LT:
            qua[qua_num].op = ">=";
            break;
        case ID_LE:
            qua[qua_num].op = ">";
            break;
        default:
            cout << "******************** Error *******************" << endl;
            cout << "Unknown Error!" << endl;
            // exit(0);
            have_error = true;
            cout << "******************** Error *******************" << endl;
            break;
    }
    qua[qua_num].arg1 = args.second.first;
    qua[qua_num].arg2 = args.second.second;
    condition_stack.push(qua_num);
    qua_num++;
    match_word(ID_RPAREN);
    parse_SEN_ONE();
    qua[qua_num].op = "GOTO";
    qua[qua_num].arg1 = "";
    qua[qua_num].arg2 = "";
    else_stack.push(qua_num);
    qua_num++;
    match_word(ID_ELSE);
    int condition_pos = condition_stack.top();
    condition_stack.pop();
    qua[condition_pos].res = "(" + to_string(qua_num+1) + ")";
    parse_SEN_ONE();
    int else_num = else_stack.top();
    else_stack.pop();
    qua[else_num].res = "(" + to_string(qua_num+1) + ")";
    have_loop = true;
    follow_loop = qua_num;
}

void parse_MULTISEN(){
    cout << "parse_MULTISEN" << endl;
    match_word(ID_LBRACE);
    parse_SEN_PART();
    match_word(ID_RBRACE);
}

void parse_SEN_ONE() {
    cout << "parse_SEN_ONE" << endl;
    if (lookahead_type == ID_LBRACE) {
        parse_MULTISEN();
    }else if(lookahead_type == ID_IDENTIFIER || 
             lookahead_type == ID_IF || 
             lookahead_type == ID_WHILE) {
        parse_SEN();
    }else{
        cout << "******************** Error *******************" << endl;
        cout << "Line " << cur_line << ": " << endl;
        cout << "Not a sentence!" << endl;
        // exit(0);
        have_error = true;
        cout << "******************** Error *******************" << endl;
    }
}

string parse_FACTOR() {
    cout << "parse_FACTOR" << endl;
    string ret;
    if (lookahead_type == ID_IDENTIFIER) {
        if (!is_exist_var(next_word.value)) {
            cout << "******************** Error *******************" << endl;
            cout << "Line " << next_word.line << ": " << endl;
            cout << "Undeclared identifier " << next_word.value << endl;
            // exit(0);
            have_error = true;
            cout << "******************** Error *******************" << endl;
        }
        ret = next_word.value;
        take_word();
        lookahead_type = next_word.type;
    }else if (lookahead_type == ID_CONSTANT) {
        ret = next_word.value;
        take_word();
        lookahead_type = next_word.type;
    }else if(lookahead_type == ID_LPAREN) {
        match_word(ID_LPAREN);
        ret = parse_EXPRESSION();
        match_word(ID_RPAREN);
    }else{
        cout << "******************** Error *******************" << endl;
        cout << "Line " << cur_line << ": " << endl;
        cout << "Not a factor! Must be an identifier, a constant or an expression!" << endl;
        // exit(0);
        have_error = true;
        cout << "******************** Error *******************" << endl;
    }
    return ret;
}

string parse_ITEM1(string arg1) {
    cout << "parse_ITEM1" << endl;
    string ret = arg1;
    string to_pass;
    if (lookahead_type == ID_MUL) {
        take_word();
        lookahead_type = next_word.type;
        string arg2 = parse_FACTOR();
        qua[qua_num].op = "*";
        qua[qua_num].arg1 = arg1;
        qua[qua_num].arg2 = arg2;
        string var_name = "tmp" + to_string(tmpvar_num);
        tmpvar_num++;
        qua[qua_num].res = var_name;
        ret = var_name;
        to_pass = ret;
        qua_num++;
        ret = parse_ITEM1(to_pass);
    }else if(lookahead_type == ID_DIV){
        take_word();
        lookahead_type = next_word.type;
        string arg2 = parse_FACTOR();
        qua[qua_num].op = "/";
        qua[qua_num].arg1 = arg1;
        qua[qua_num].arg2 = arg2;
        string var_name = "tmp" + to_string(tmpvar_num);
        tmpvar_num++;
        qua[qua_num].res = var_name;
        ret = var_name;
        to_pass = ret;
        qua_num++;
        ret = parse_ITEM1(to_pass);
    }else{
        // nothing to do
    }
    return ret;
}
string parse_ITEM() {
    cout << "parse_ITEM" << endl;
    string arg1 = parse_FACTOR();
    string ret = parse_ITEM1(arg1);
    return ret;
}

string parse_EXPRESSION1(string arg1) {
    cout << "parse_EXPRESSION1" << endl;
    string ret = arg1;
    string to_pass;
    if (lookahead_type == ID_PLUS) {
        take_word();
        lookahead_type = next_word.type;
        string arg2 = parse_ITEM();
        qua[qua_num].op = "+";
        qua[qua_num].arg1 = arg1;
        qua[qua_num].arg2 = arg2;
        string var_name = "tmp" + to_string(tmpvar_num);
        tmpvar_num++;
        qua[qua_num].res = var_name;
        ret = var_name;
        to_pass = ret;
        qua_num++;
        ret = parse_EXPRESSION1(to_pass);
    }else if(lookahead_type == ID_MINUS){
        take_word();
        lookahead_type = next_word.type;
        string arg2 = parse_ITEM();
        qua[qua_num].op = "-";
        qua[qua_num].arg1 = arg1;
        qua[qua_num].arg2 = arg2;
        string var_name = "tmp" + to_string(tmpvar_num);
        tmpvar_num++;
        qua[qua_num].res = var_name;
        ret = var_name;
        to_pass = ret;
        qua_num++;
        ret = parse_EXPRESSION1(to_pass);
    }else{
        // nothing to do
    }
    return ret;
}
string parse_EXPRESSION() {
    cout << "parse_EXPRESSION" << endl;
    string arg1 = parse_ITEM();
    string ret = parse_EXPRESSION1(arg1);
    return ret;
}

int parse_RELATION_OPT() {
    cout << "parse_RELATION_OPT" << endl;
    int ropt_type;
    if (lookahead_type != ID_EQ && lookahead_type != ID_NE 
    &&  lookahead_type != ID_GT && lookahead_type != ID_LT
    &&  lookahead_type != ID_GE && lookahead_type != ID_LE) {
        cout << "******************** Error *******************" << endl;
        cout << "Line " << next_word.line << ": " << endl;
        cout << "Not a relation operator!" << endl;
        // exit(0);
        have_error = true;
        cout << "******************** Error *******************" << endl;
    }else{
        ropt_type = lookahead_type;
        take_word();
        lookahead_type = next_word.type;
    }
    return ropt_type;
}

pair<int, pair<string,string>> parse_CONDITION() {
    cout << "parse_CONDITION" << endl;
    string arg1 = parse_EXPRESSION();
    int ropt_type = parse_RELATION_OPT();
    string arg2 = parse_EXPRESSION();
    pair<int, pair<string, string>> args;
    args.first = ropt_type;
    args.second.first = arg1;
    args.second.second = arg2;
    return args;
}

void parse_ASSIGN_SEN() {
    cout << "parse_ASSIGN_SEN" << endl;
    if (!is_exist_var(next_word.value)) {
        cout << "******************** Error *******************" << endl;
        cout << "Line " << next_word.line << ": " << endl;
        cout << "Undeclared identifier " << next_word.value << endl;
        // exit(0);
        have_error = true;
        cout << "******************** Error *******************" << endl;
    }
    string temp_identifier = next_word.value;
    parse_IDENTIFIER();
    match_word(ID_GIVE);
    string to_give = parse_EXPRESSION();
    qua[qua_num].op = "=";
    qua[qua_num].arg1 = to_give;
    qua[qua_num].arg2 = "/";
    qua[qua_num].res = temp_identifier;
    qua_num++;
}

void parse_SEN() {
    cout << "parse_SEN" << endl;
    if (lookahead_type == ID_IF) {
        parse_CONDITION_SEN();
    }else if(lookahead_type == ID_WHILE) {
        parse_LOOP_SEN();
    }else if(lookahead_type == ID_IDENTIFIER) {
        parse_ASSIGN_SEN();
    }else{
        cout << "******************** Error *******************" << endl;
        cout << "Line " << next_word.line << ": " << endl;
        cout << "Not a sentence!" << endl;
        // exit(0);
        have_error = true;
        cout << "******************** Error *******************" << endl;
    }
}

void parse_SEN_PART1() {
    cout << "parse_SEN_PART1" << endl;
    if (lookahead_type == ID_SEMICOLON) {
        match_word(ID_SEMICOLON);
        parse_SEN();
        parse_SEN_PART1();
    }else{
        // nothing to do
    }
}
void parse_SEN_PART() {
    cout << "parse_SEN_PART" << endl;
    parse_SEN();
    parse_SEN_PART1();
}

void parse_IDENTIFIER_TAB1() {
    cout << "parse_IDENTIFIER_TAB1" << endl;
    if (lookahead_type == ID_COMMA) {
        match_word(ID_COMMA);
        parse_IDENTIFIER();
        parse_IDENTIFIER_TAB1();
    }else{
        // nothing to do
    }
}
void parse_IDENTIFIER_TAB() {
    cout << "parse_IDENTIFIER_TAB" << endl;
    parse_IDENTIFIER();
    parse_IDENTIFIER_TAB1();
}

void parse_VAR_DECLARATION() {
    cout << "parse_VAR_DECLARATION" << endl;
    match_word(ID_INT);
}

void parse_VAR_DECLARATION_PART() {
    cout << "parse_VAR_DECLARATION_PART" << endl;
    parse_VAR_DECLARATION();
    parse_IDENTIFIER_TAB();
}

void parse_MAIN_FUNC() {
    cout << "parse_MAIN_FUNC" << endl;
    parse_VAR_DECLARATION_PART();
    match_word(ID_SEMICOLON);
    cout << "------------------------------------" << endl;
    parse_SEN_PART();
}

void parse_PROGRAM() {
   cout << endl << "Start Compiling..." << endl;
   match_word(ID_MAIN);
   match_word(ID_LPAREN);
   match_word(ID_RPAREN);
   match_word(ID_LBRACE);
   parse_MAIN_FUNC();
   match_word(ID_RBRACE);
   if (have_error) {
       cout << "==================== Have Error!!! ====================" << endl;
       exit(0);
   }
   cout << "==================== OK! ===================" << endl;
}


int main() {
    char ch;
    freopen("in.c", "r", stdin);

    file_length = 0;

    while (~scanf("%c", &ch)) {
        letters[file_length++] = ch;
    }
    cout << "File_length: " << file_length << endl;
    cur = 0;
    cur_line = 1;
    // while (cur != file_length) {
        // take_word();
        // cout << next_word.value << "\t" << next_word.type << "\t" << next_word.line << endl;
    // }

    take_word();
    lookahead_type = next_word.type;

    parse_PROGRAM();

    int k = 0;
    cout << endl;
    cout << "No.\tOP\tARG1\tARG2\tRES" << endl;
    for (k = 0; k < qua_num; k++) {
        cout << "[" << k+1 << "]\t" << qua[k].op << "\t" << qua[k].arg1 << "\t" << qua[k].arg2 << "\t" << qua[k].res << "\t" << endl;
    }
    if (have_loop && follow_loop == k) {
        cout << "[" << k+1 << "]\t" << "" << "\t" << "" << "\t" << "" << "\t" << "" << "\t" << endl;
    }
    // cout << follow_loop << endl << k << endl;

    ofstream out_file;
    out_file.open("out.txt");
    out_file << "No.\tOP\tARG1\tARG2\tRES" << endl;
    for (k = 0; k < qua_num; k++) {
        out_file << "[" << k+1 << "]\t" << qua[k].op << "\t" << qua[k].arg1 << "\t" << qua[k].arg2 << "\t" << qua[k].res << "\t" << endl;
    }
    if (have_loop && follow_loop == k) {
        out_file << "[" << k+1 << "]\t" << "" << "\t" << "" << "\t" << "" << "\t" << "" << "\t" << endl;
    }

    return 0;
}
