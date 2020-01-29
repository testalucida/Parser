#include "Parser.h"

#include <cstdio>  /* printf */
#include <cstdlib> /* fopen, fseek, ... */
#include <iostream>
#include <cstring>

using namespace std;

/*comment*/ void testfunc() {

}

Parser::Parser() {
	// TODO Auto-generated constructor stub

}

Parser::~Parser() {
	if (_pCode)
		free(_pCode);
}

void Parser::parseFile(const char *pFilename) {
	_outline.clear();

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

	createOutline();
	cerr << "***********************" << endl << _outline << endl;

}

void Parser::createOutline() {
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
					_outline += buffer;
					_outline += EOL;
				}
			}
			break;
		default: /*for test purposes */
			cerr << *p;
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

inline const char* Parser::createCstring(const char* p1, const char* p2, char* cs) const {
	for(char* pC = cs; p1 <= p2; p1++, pC++) {
		*pC = *p1;
	}
	return p1;
}

/**
 * Checks if searchlen chars in 'in' for 'what'.
 * If 'what' is found in 'in', returns a pointer to the begin of the found string.
 * Else return NULL;
 */
bool Parser::startsWith(const char* in, const char* what) const {
	const char* p = in;
	const char* p2 = what;
	int len = strlen(what);
	for( int i = 0; i < len; i++, p++, p2++) {
		if(*p != *p2) return false;
	}
	return true;
}
















