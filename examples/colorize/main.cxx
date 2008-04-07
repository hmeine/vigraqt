#include "colorize.hxx"
#include <QApplication>
#include <vigra/error.hxx>
#include <iostream>
#include <stdlib.h>

int main(int argc, char **argv)
{
	try
	{
		QApplication a(argc, argv);
		if(argc < 2)
		{
			Colorize *t = new Colorize();
			t->show();
		}
		else
		{
			for(int i = 1; i < argc; ++i)
			{
				Colorize *t = new Colorize();
				t->load(argv[i]);
				t->show();
			}
		}
		QObject::connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
		return a.exec();
	}
	catch (vigra::StdException & e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
}
