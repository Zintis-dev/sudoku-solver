#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QLineEdit>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_sizeComboBox_currentIndexChanged(int index);
    void on_solveButton_clicked();
    void on_clearButton_clicked();

private:
    Ui::MainWindow *ui;
    QGridLayout *gridLayout;
    std::vector<std::vector<QLineEdit*>> grid;
    int gridSize;

    void createGrid(int size);
    bool solveSudoku(std::vector<std::vector<int>>& grid, int size);
    bool findUnassignedCell(const std::vector<std::vector<int>>& grid, int size, int& row, int& col);
    bool isValidPuzzle(const std::vector<std::vector<int>> &puzzle);
    bool isValid(const std::vector<std::vector<int>>& grid, int row, int col, int num, int size);
    bool isValidRowOrColumn(const std::vector<std::vector<int>>& puzzle, int index);
    bool isValidSubGrid(const std::vector<std::vector<int>>& puzzle, int startRow, int startCol, int subGridSize);
};

#endif // MAINWINDOW_H
