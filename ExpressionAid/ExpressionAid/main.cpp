#include "ExpressionAid.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ExpressionAid w;
	w.setMinimumSize(800, 600);
	w.setWindowTitle(QApplication::translate("Expression Aid", "Expression Aid"));
	w.setWindowIcon(QIcon("Resources/eye.png"));		// set icon
	w.show();
	return a.exec();
}
