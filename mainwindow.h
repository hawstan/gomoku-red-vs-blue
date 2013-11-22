#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QPixmap>
#include <QPainter>

namespace Ui {
class MainWindow;
}

//TODO dynamic board size
#define FIELDS_WIDTH 15
#define FIELDS_HEIGHT 15
#define FIELD_XY 25

#define IMAGE_ICON "images/gomoku.png"
#define IMAGE_FIELD_CIRCLE "images/circle.png"
#define IMAGE_FIELD_CROSS "images/cross.png"

#define ABOUT_TEXT "This is an implementation of Gomoku board game using Qt toolkit.\n"\
		"Board size is 15x15. To win, the player must places exactly 5 pieces in a row.\n\n"\
		"Version 0.1\n"\
		"This game is provided under MIT License.\n"\
		"Created 2013-11-21 by Stanley Hawkeye."

enum Player : uchar
{
	NO_PLAYER = 0,
	CIRCLE = 1,
	CROSS = 2,
};


class MainWindow : public QMainWindow
{
	Q_OBJECT
private:
	Ui::MainWindow *ui;

	uchar fields[FIELDS_WIDTH][FIELDS_HEIGHT];
	int remainingFields;

	QPixmap canvas;

	Player turn;
	uchar winner;

	QImage field_cross;
	QImage field_circle;

	void initialize(); // used to reset the game
	void renderFields();
	void fieldClicked(int x, int y);
	int checkRow(int x, int y, int step_x, int step_y, int* marker_x, int* marker_y);
	void showWinner();

public:
	explicit MainWindow(QWidget* parent = 0);
	~MainWindow();

	bool eventFilter(QObject* object, QEvent* event);

public slots:
	void newGame();
	void saveScreenshot();
	void showAboutDialog();
	//TODO add save game
	//TODO add undo last move
};

#endif // MAINWINDOW_H
