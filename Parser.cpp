#include "Parser.h"

#include <cstdio>  /* printf */
#include <cstdlib> /* fopen, fseek, ... */
#include <iostream>
#include <cstring>
#include <algorithm>

using namespace std;

/*comment*/ void testfunc() {

}

Parser::Parser() {
	// TODO Auto-generated constructor stub

}

Parser::~Parser() {
	if (_pCode)
		free(_pCode);

	for( Tag* tag : _tags ) {
		delete tag;
	}
}

Tags& Parser::parseFile(const char *pFilename) {
	_tags.clear();

	size_t size;

	/* Open file in read-only mode */
	FILE *fp = fopen(pFilename, "r");

	/* Get the buffer size */
	fseek(fp, 0, SEEK_END); /* Go to end of file */
	size = ftell(fp); /* How many bytes did we pass ? */

	/* Set position of stream to the beginning */
	rewind(fp);

	/* Allocate the buffer*/
	_pCode = (char*) malloc((size + 1) * sizeof(*_pCode));

	/* Read the file into the buffer */
	fread(_pCode, size, 1, fp); /* Read 1 chunk of size bytes from fp into buffer */

	/* NULL-terminate the buffer */
	_pCode[size] = '\0';

	createTags();
	return _tags;
}

void Parser::createTags() {
	//TODO: do we have to deal with line breaks?
	string tempbuffer;
	for (const char *p = _pCode; *p; p++) {
		switch (*p) {
		case '/':
			//could be beginning of a comment
			p++;
			if (*p == '/') {
				//we have a line comment
				p = proceedToLineEnd(p); //p points to one char before EOL
				if (!p) {
					return;
				}
			} else if (*p == '*') {
				//we have a block comment
				p = proceedTo(p, "*/");
				if (!p) {
					throw runtime_error("unterminated block comment detected");
				}
			} else {
				--p; //no comment; go back to last char, could be EOL
			}
			break;
		case '#':
			{
				char buffer[256] = {'\0'};
				p = isInclude(p, buffer);
				if(*buffer) {
					Tag* pTag = new Tag;
					pTag->isInclude = true;
					pTag->text = buffer;
					_tags.push_back(pTag);
				}
			}
			break;
		case '\'':
			p = proceedTo(p, "'"); //omit constant characters
			break;
		case '"':
			p = proceedTo(p, "\""); //omit literals
			break;
		case '{':
			{
				// check tempbuffer if '{' is preceded by a class declaration
				// or function/method
				// signature
				Tag* pTag = checkForOutlineRelevance(tempbuffer);
				if (pTag) {
					_tags.push_back(pTag);
					_inNamespace = pTag->isNamespace;
					_inClassOrStruct = (pTag->isClass || pTag->isStruct);
				}

				//if it's a method/function definiton or
				//if/switch/else/for/while statement proceed to closing '}'
				//TODO
			}
			break;
		default:
			//read *p into tempbuffer. When reaching next opening curly bracket
			//we'll check if tempbuffer's content is relevant for outline.
			tempbuffer += *p;
			break;
		}
	}
}

/**
 * proceeds from pStart to the last char of pPattern.
 * returns a pointer to this last char.
 */
const char* Parser::proceedTo(const char *pStart, const char *pPattern) const {
	const char *p = strstr(pStart, pPattern);
	if (p) {
		p += (strlen(pPattern) - 1); //point to last char of pPattern
	}
	return p;
}

/**
 * proceeds to the end of the line in which pStart is located.
 * returns a pointer to the last char before EOL.
 */
const char* Parser::proceedToLineEnd(const char *pStart) const {
	const char *p = pStart;
	for (; *p && *p != EOL; p++)
		;
	return (*p == EOL) ? --p : NULL;
}

/**
 * Checks if the given string is an include stmt.
 * If so, the included file will be provided in 'includeFile'.
 * Returns a pointer to the last char before EOL.
 * An include stmt can only be recognized if pStart starts with #
 */
const char* Parser::isInclude(const char* pStart, char* includeFile) const {
	const char* p = pStart;
	if(startsWith(p, "#include")) {
		p += strlen("#include");
		for(; *p && !(*p == '"' or *p == '<'); p++ ); //search opening tag
		const char* cs = (*p == '"') ? "\"" : ">"; //cs: closing tag
		const char* pEnd = proceedTo(p, cs);
		p = createCstring(p, pEnd, includeFile);
	}
	return proceedToLineEnd(p);
}

/**
 * Creates a string  in the given char buffer cs consisting of the chars
 * between p1 and p2. *p1 and *p2 are included.
 * cs will be terminated by 0x00.
 * The start pointer p1 is returned.
 */
const char*
Parser::createCstring(const char* p1, const char* p2, char* cs) const {
	char* pC = cs;
	for(; p1 <= p2; p1++, pC++) {
		*pC = *p1;
	}
	*pC = 0x00;
	return p1;
}

/**
 * Checks if 's' starts with 'what'.
 * If so, returns true, else false.
 */
bool Parser::startsWith(const char* s, const char* what) const {
	const char* p = s;
	const char* p2 = what;
	int len = strlen(what);
	for( int i = 0; i < len; i++, p++, p2++) {
		if(*p != *p2) return false;
	}
	return true;
}

/**
 * Examines tmp for content relevant for an outline. This might be
 * a class declaration, method signature etc.
 * if and switch statements, for- and while- loops are ignored.
 * tmp contains code which was collected from the end of the last processed
 * code fragment (e.g include stmt or comment) until an opening curly bracket.
 * tmp must not end with '{' (but 1 char before).
 * tmp must not contain any comments or literals.
 */
Tag* Parser::checkForOutlineRelevance(std::string& tmp) const {
	//parsing from right to left.
	const char* pStart = tmp.c_str();
	const char* p = pStart + tmp.length() - 1;
	//omit EOL and spaces:
 	while (p >= pStart && (*p == EOL || *p == SPACE)) --p;
	//if we find ';' or '{' we're dealing with a code block - ignore.
	if (*p == ';' || *p == '{') return NULL;
	//if we find ':' it's a case: stmt - ignore
	if (*p == ':') return NULL;
	//if we find a closing (right) parenthesis, go to the left one:
	if (*p == ')') {
		while (p >= pStart && *p != '(') --p;
		if (*p != '(') return NULL; //code not consistent
	}
	//omit EOL and spaces:
	while (p >= pStart && (*p == EOL || *p == SPACE)) --p;
	//Now we have a remaining string from pStart till the last char
	//left of the opening parenthesis.
	//That could be a method or function definition...
	//   const void MyClass::myMethod
	//   static int myFunction
	//or a class declaration...
	//   class MyClass : public MyBaseClass
	//or a struct definition...
	//   struct MyStruct
	//   or simply struct
	//or a enum definition...
	//   enum MyValues
	//or a global array initialization...
	//   const static char* myCStringArray[] =
	return NULL;
}














