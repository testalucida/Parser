#ifndef OUTLINETREE_H_
#define OUTLINETREE_H_

#include <FL/Fl_Tree.H>

class OutlineTree : public Fl_Tree {
public:
	OutlineTree( int x, int y, int w, int h, const char* pLbl=0);
	virtual ~OutlineTree();
};

#endif /* OUTLINETREE_H_ */
