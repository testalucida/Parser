#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>

#include "main_window.h"
#include "Parser.h"

int main() {
	Fl_Double_Window* pWin =  make_window(); //new Fl_Double_Window(100, 100, 500, 600, "Outline");
	g_pTree->root_label("Outline");
	Parser parser;
	parser.parseFile( "testheader.h" );
	pWin->show();
	return Fl::run();
}
