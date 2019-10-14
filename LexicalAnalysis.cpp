/* @Author: Jonathan Wang
 * @Date: 14th Oct., 2019
 * @Developing Environment: Manjaro 18.1.0 Juhraya
 * */

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

/* A Simple Lexical Analysis Program */

/* 1	main
 * 2	int
 * 3	if
 * 4	else
 * 5	while
 * 6	do
 * 7	+
 * 8	-
 * 9	*
 * 10	/
 * 11	<
 * 12	>
 * 13	!=
 * 14	>=
 * 15	<=
 * 16	==
 * 17	,
 * 18	;
 * 19	(
 * 20	)
 * 21	{
 * 22	}
 * 23	<Identifiers>
 * */

string reserved_words[6] = {"main", "int", "if", "else", "while", "do"};
int reserved_nums[6] = {1, 2, 3, 4, 5, 6};
// string add_opts[2] = {"+", "-"};
// string mul_opts[2] = {"*", "/"};
// string rel_opts[6] = {"<", ">", "!=", ">=", "<=", "=="};
// string isolates[6] = (",", ";", "(", ")", "{", "}");
string symbols[16] = {"+", "-", "*", "/", "<", ">", "!=", ">=", "<=", "==", ",", ";", "(", ")", "{", "}"};
int symbol_nums[16] = {7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22};

string letters[5005];
string words[5005];
string vis[5005];
int file_length, num, vis_num;

int isReserved(string s) {
	int i;
	for (i = 0; i < 6; i++) {
		if (s == reserved_words[i])
			return reserved_nums[i];
	}
	return 0;
}

int isSymbol(string s) {
	int i;
	for (i = 0; i < 16; i++) {
		if (s == symbols[i])
			return symbol_nums[i];
	}
	return 0;
}

bool isDigit(string s) {
	if ("0" <= s && s <= "9")
		return true;
	return false;
}

bool isLetter(string s) {
	if ("a" <= s && s <= "z")
		return true;
	return false;
}

int charType(string s) {
	if (isLetter(s))
		return 1;
	if (isDigit(s))
		return 2;
	if (s == ">" || s == "=" || s == "<" || s == "!" || 
		s == "," || s == ";" || s == "(" || s == ")" || 
		s == "{" || s == "}" || s == "+" || s == "-" || 
		s == "*" || s == "/")
		return 3;
	return 0;
}

string concatReserved(string s, int n) {
	int i = n + 1;
	int flag = 1;

	while (flag) {
		if (isDigit(letters[i]) || isLetter(letters[i])) {
			s = (s + letters[i]).c_str();
			if (isReserved(s)) {
				i++;
				num = i;
				return s;
			}
			i++;
		}else{
			flag = 0;
		}	
	}
	num = i;
	return s;
}

string concatSymbol(string s, int n) {
	int i = n + 1;
	string ss = letters[i];
	if (ss == ">" || ss == "=" || ss == "<" || ss == "!") {
		s = (s + letters[i]).c_str();
		i++;
	}
	num = i;
	return s;
}

string concatDigit(string s, int n) {
	int i = n + 1;
	int flag = 1;

	while (flag) {
		if (isDigit(letters[i])) {
			s = (s + letters[i]).c_str();
			i++;
		}else{
			flag = 0;
		}
	}
	num = i;
	return s;
}

void printItem(string s, int ty) {
	int i;
	for (i = 0; i < vis_num; i++) {
		if (vis[i] == s)
			return;
	}
	cout << s << "\t" << ty << endl;
	vis[vis_num] = s;
	vis_num++;
}

void takeWord() {
	int ty;
	num = 0;
	while (num < file_length) {
		string s1, s2;
		s1 = letters[num];
		ty = charType(s1);
		switch (ty) {
			case 1:
				s2 = concatReserved(s1, num);
				if (isReserved(s2)) {
					printItem(s2, isReserved(s2));
				}else{
					printItem(s2, 0);
				}
				break;
			case 2:
				s2 = concatDigit(s1, num);
				printItem(s2, 23);
				break;
			case 3:
				s2 = concatSymbol(s1, num);
				printItem(s2, isSymbol(s2));
				break;
		}
	}
}

int main() {
	char ch;
	vis_num = 0;
	freopen("in.c", "r", stdin);
	freopen("out.txt", "w", stdout);

	file_length = 0;
	while (cin >> ch) {
		if (ch != ' ')
			letters[file_length++] = ch;
	}
	takeWord();

	return 0;
}
