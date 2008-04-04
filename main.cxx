#include "imageAnalyzer.hxx"
#include <qapplication.h>
#include <stdlib.h>
#include <iostream>
#include <vigra/error.hxx>

int main(int argc, char **argv)
{
	try
	{
		QApplication a(argc, argv);
		if(argc < 2)
		{
			ImageAnalyzer *t = new ImageAnalyzer();
			t->show();
		}
		else
		{
			for(int i = 1; i < argc; ++i)
			{
				ImageAnalyzer *t = new ImageAnalyzer();
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
