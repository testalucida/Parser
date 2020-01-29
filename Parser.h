#ifndef PARSER_H_
#define PARSER_H_

#include <vector>
#include <string>

#define EOL '\n'
#define SPACE ' '

enum Primitives {
	BOOL,
	CHAR,
	DOUBLE,
	FLOAT,
	INT,
	LONG,
	NOT_SET,
	VOID
};

const static char* primitives[] = {
	"bool", "char", "double", "float",
	"int", "long", "void"
};

//be aware of "unsigned"

const static char* keywords[] = {
	"class", "const", "enum", "inline", "namespace",
	"static", "struct", "using", "typedef", "virtual"
};

struct Tag {
	int line = 0;
	int col = 0;
	std::string type;
	bool isDefine = false;
	bool isInclude = false;
	bool isDeclaration = false;
	bool isNamespace = false;
	bool isClass = false;
	bool isStruct = false;
	bool isMethod = false;
	bool isFunction = false;
	bool isMemberVar = false;
	bool isStatic = false;
	bool isConst = false;
	bool isVirtual = false;
	std::string text;
};

typedef std::vector<Tag*> Tags;

class Parser {
public:
	Parser();
	virtual ~Parser();
	Tags& parseFile( const char* pFilename );
private:
	void createTags();

	/**
	 * proceeds from pStart to the last char of pPattern.
	 * returns a pointer to this last char.
	 */
	inline const char* proceedTo(const char* pStart, const char* pPattern) const;

	/**
	 * proceeds to the end of the line in which pStart is located.
	 * returns a pointer to the last char before EOL.
	 */
	inline const char* proceedToLineEnd(const char* pStart) const;

	/**
	 * Checks if the given string is an include stmt.
	 * If so, the included file will be provided in 'includeFile'.
	 * Returns a pointer to the last char before EOL.
	 * An include stmt can only be recognized if pStart starts with #
	 */
	inline const char* isInclude(const char* pStart, char* includeFile) const;

	/**
	 * Creates a string  in the given char buffer cs consisting of the chars
	 * between p1 and p2. *p1 and *p2 are included.
	 * cs will be terminated by 0x00.
	 * The start pointer p1 is returned.
	 */
	inline const char* createCstring(const char* p1, const char* p2, char* cs) const;

	/**
	 * Checks if 's' starts with 'what'.
	 * If so, returns true, else false.
	 */
	inline bool startsWith(const char* in, const char* what) const;

	/**
	 * Examines tmp for content relevant for an outline. This might be
	 * a class declaration, method signature etc.
	 * if and switch statements, for- and while- loops are ignored.
	 * tmp contains code which was collected from the end of the last processed
	 * code fragment (e.g include stmt or comment) until an opening curly bracket.
	 * tmp must not end with '{' (but 1 char earlier).
	 * tmp must not contain any comments or literals.
	 */
	inline Tag* checkForOutlineRelevance(std::string& tmp) const;
private:
	char* _pCode = NULL;
	Tags _tags;
	bool _inNamespace = false;
	bool _inClassOrStruct = false;
};

#endif /* PARSER_H_ */
