#ifndef PARSER_H_
#define PARSER_H_

#include <string>

#define EOL '\n'

class Parser {
public:
	Parser();
	virtual ~Parser();
	void parseFile( const char* pFilename );
	void createOutline();
private:
	inline const char* proceedTo(const char* pStart, const char* pPattern) const;
	inline const char* proceedToLineEnd(const char* pStart) const;
	inline const char* isInclude(const char* pStart, char* includeFile) const;
	inline const char* createCstring(const char* p1, const char* p2, char* cs) const;
	inline bool startsWith(const char* in, const char* what) const;
private:
	char *_pCode = NULL;
	std::string _outline;
};

#endif /* PARSER_H_ */
