#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <QMessageBox>
#include <QFileDialog>

#define STATUS_TEXT_CIRCLE "Circle"
#define STATUS_TEXT_CROSS "Cross"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    canvas(FIELDS_WIDTH * FIELD_XY - 1, FIELDS_HEIGHT * FIELD_XY - 1),

	field_cross(IMAGE_FIELD_CROSS),
	field_circle(IMAGE_FIELD_CIRCLE)
{
	ui->setupUi(this);

	this->setWindowIcon(QIcon(IMAGE_ICON));

	ui->fieldLabel->installEventFilter(this);

	initialize();
}

void MainWindow::initialize()
{
	for(int x=0; x < FIELDS_WIDTH; x++)
		for(int y=0; y < FIELDS_HEIGHT; y++)
			this->fields[x][y] = NO_PLAYER;

	turn = CIRCLE;
	ui->statusLabel->setText(STATUS_TEXT_CIRCLE);
	remainingFields = FIELDS_WIDTH * FIELDS_HEIGHT;

	ui->fieldLabel->setFixedSize(FIELDS_WIDTH * FIELD_XY, FIELDS_HEIGHT * FIELD_XY);

	renderFields();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::newGame()
{
	initialize();
}

bool MainWindow::eventFilter(QObject* object, QEvent* e)
{
	if(object != ui->fieldLabel || e->type() != QEvent::MouseButtonPress)
		return false;

	const QMouseEvent* event = static_cast<const QMouseEvent*>(e);

	int x = event->x() / FIELD_XY;
	int y = event->y() / FIELD_XY;

	this->fieldClicked(x, y);

	return false;
}

void MainWindow::renderFields()
{
	QPainter painter(&canvas);
	painter.setBrush(Qt::NoBrush);
	painter.setPen(Qt::NoPen);

	// background
	painter.fillRect(0, 0, canvas.width(), canvas.height(), Qt::white);


	// fields
	painter.setPen(Qt::black);
	for(int x = 0; x < FIELDS_WIDTH; x++)
	{
		for(int y = 0; y < FIELDS_HEIGHT; y++)
		{
			painter.drawRect(x * FIELD_XY - 1, y * FIELD_XY - 1, FIELD_XY, FIELD_XY);

			if(fields[x][y] == CIRCLE)
			{
				painter.drawImage(QPoint(x * FIELD_XY, y * FIELD_XY), field_circle);
			}
			else if(fields[x][y] == CROSS)
			{
				painter.drawImage(QPoint(x * FIELD_XY, y * FIELD_XY), field_cross);
			}
			else
			{
				painter.fillRect(x * FIELD_XY, y * FIELD_XY, FIELD_XY - 1, FIELD_XY - 1, Qt::white);
			}
		}
	}

	ui->fieldLabel->setPixmap(canvas);
}

int MainWindow::checkRow(int x, int y, int step_x, int step_y, int* marker_x, int* marker_y)
{
	*marker_x = x;
	*marker_y = y;

	if(x-step_x < 0 || x+step_x > FIELDS_WIDTH)
		return 0;

	if(y-step_y < 0 || y+step_y > FIELDS_HEIGHT)
			return 0;

	if(fields[x][y] == fields[x + step_x][y + step_y])
	{
		return 1 + checkRow(x + step_x, y + step_y, step_x, step_y, marker_x, marker_y);
	}
	return 0;
}

void MainWindow::fieldClicked(int x, int y)
{
	if(fields[x][y] > NO_PLAYER) // cannot reclaim claimed field
		return;

	if(turn == NO_PLAYER)
		return;

	fields[x][y] = turn;

	renderFields();

	// check if the player has five in a row
	int x1;
	int y1;
	int x2;
	int y2;
	if(checkRow(x, y, -1, 0, &x1, &y1) + checkRow(x, y, 1, 0, &x2, &y2) == 4
		|| checkRow(x, y, 0, -1, &x1, &y1) + checkRow(x, y, 0, 1, &x2, &y2) == 4

		|| checkRow(x, y, -1, 1, &x1, &y1) + checkRow(x, y, 1, -1, &x2, &y2) == 4

		|| checkRow(x, y, -1, -1, &x1, &y1) + checkRow(x, y, 1, 1, &x2, &y2) == 4)
	{
		// connect the row with a line
		QPainter p(&canvas);
		p.setRenderHint(QPainter::Antialiasing, true);
		p.setPen(QPen(Qt::gray, 5));
		p.drawLine(FIELD_XY * x1 + FIELD_XY/2, FIELD_XY * y1 + FIELD_XY/2,
				FIELD_XY * x2 + FIELD_XY/2, FIELD_XY * y2 + FIELD_XY/2);
		//TODO do this with one line only
		p.drawLine(FIELD_XY * x1 + FIELD_XY/2, FIELD_XY * y1 + FIELD_XY/2,
						FIELD_XY * x + FIELD_XY/2, FIELD_XY * y + FIELD_XY/2);
		ui->fieldLabel->setPixmap(canvas);

		winner = turn;
		turn = NO_PLAYER;
		showWinner();
		return;
	}

	// if all the fields are full, it's a tie
	remainingFields--;
	if(remainingFields <= 0)
	{
		turn = NO_PLAYER;
		winner = 0;
		showWinner();
		return;
	}

	// swap turn
	if(turn == CIRCLE)
	{
		ui->statusLabel->setText(STATUS_TEXT_CROSS);
		turn = CROSS;
	}
	else
	{
		ui->statusLabel->setText(STATUS_TEXT_CIRCLE);
		turn = CIRCLE;
	}
}

void MainWindow::showWinner()
{
	QMessageBox msgbox;
	QString text;
	if(winner == 0)
		text = "It's a tie!";
	else if(winner == CIRCLE)
		text = "Circle wins!";
	else if(winner == CROSS)
		text = "Cross wins!";
	else
		text = "Unknown error. Could not determine winner";

	ui->statusLabel->setText(text);
	msgbox.setWindowTitle("Game Over");
	msgbox.setText(text);
	msgbox.exec();
}


void MainWindow::saveScreenshot()
{
	QFileDialog fd(this);
	fd.setNameFilter("PNG Image (*.png);;All Files (*)");
	fd.setFileMode(QFileDialog::AnyFile);
	fd.setConfirmOverwrite(true);
	if(!fd.exec())
		return;

	QMessageBox cmsgbox;
	if(canvas.save(fd.selectedFiles().first(), "png", 0))
		cmsgbox.setText("Screenshot saved!");
	else
		cmsgbox.setText("Failed to save screenshot!");
	cmsgbox.exec();
}

void MainWindow::showAboutDialog()
{
	QMessageBox msgbox;
	msgbox.setTextFormat(Qt::RichText);
	msgbox.setText("About Gomoku Red vs Blue");
	msgbox.setInformativeText(ABOUT_TEXT);
	msgbox.setMaximumSize(400, 600);
	msgbox.setStandardButtons(QMessageBox::Ok);
	msgbox.exec();
}
