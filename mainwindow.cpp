#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <cmath>
#include <QMessageBox>
#include <QIntValidator>
#include <chrono>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    gridLayout = ui->gridLayout;
    createGrid(4);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createGrid(int size)
{
    gridSize = size;
    grid.clear();

    QLayoutItem *child;
    while ((child = gridLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    // Creates grid
    for (int i = 0; i < size; i++) {
        std::vector<QLineEdit*> row;
        for (int j = 0; j < size; j++) {
            QLineEdit *cell = new QLineEdit(this);
            cell->setMaxLength(1);
            cell->setValidator(new QIntValidator(1, size, this));
            cell->setAlignment(Qt::AlignCenter);
            cell->setPlaceholderText("");
            cell->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

            // Connect editingFinished signal to ensure validation on user input
            connect(cell, &QLineEdit::editingFinished, [cell, size]() {
                QString text = cell->text();
                if (!text.isEmpty()) {
                    int value = text.toInt();
                    if (value < 1 || value > size) {
                        cell->clear();
                        QMessageBox::warning(cell->parentWidget(), "Invalid Input",
                                             QString("Please enter a value between 1 and %1.").arg(size));
                    }
                }
            });

            gridLayout->addWidget(cell, i, j);
            row.push_back(cell);
        }
        grid.push_back(row);
    }
}

void MainWindow::on_sizeComboBox_currentIndexChanged(int index)
{
    if (index == 0)
        createGrid(4);
    else
        createGrid(9);
}

void MainWindow::on_solveButton_clicked()
{
    std::vector<std::vector<int>> puzzle(gridSize, std::vector<int>(gridSize, 0));

    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            QString text = grid[i][j]->text();
            if (!text.isEmpty()) {
                int value = text.toInt();
                // Checks if user input is not larger than max value
                if (value < 1 || value > gridSize) {
                    QMessageBox::warning(this, "Invalid Input",
                                         QString("All values must be between 1 and %1. Please correct your input.").arg(gridSize));
                    return;
                }
                puzzle[i][j] = value;
            }
        }
    }

    if (!isValidPuzzle(puzzle)) {
        QMessageBox::warning(this, "Invalid Puzzle", "The puzzle contains duplicate numbers in rows, columns, or sub grids.");
        return;
    }

    // Starts chrono timer
    auto start = std::chrono::high_resolution_clock::now();

    bool solved = solveSudoku(puzzle, gridSize);

    // Ends Chrono timer
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    qDebug() << "Solved in " << duration.count() << " microseconds = " << duration.count() / 1000.0 << " milliseconds";

    if (solved) {
        // Displays solved puzzle to the user in GUI
        for (int i = 0; i < gridSize; i++) {
            for (int j = 0; j < gridSize; j++) {
                grid[i][j]->setText(QString::number(puzzle[i][j]));
            }
        }
    } else {
        QMessageBox::warning(this, "Unsolvable Puzzle", "The puzzle cannot be solved. Please check your input.");
    }
}

// Checks if user entered values corresponds with sudoku's rules
bool MainWindow::isValidPuzzle(const std::vector<std::vector<int>>& puzzle)
{
    for (int i = 0; i < gridSize; i++) {
        if (!isValidRowOrColumn(puzzle, i)) {
            return false;
        }
    }

    int subGridSize = std::sqrt(gridSize);
    for (int row = 0; row < gridSize; row += subGridSize) {
        for (int col = 0; col < gridSize; col += subGridSize) {
            if (!isValidSubGrid(puzzle, row, col, subGridSize)) {
                return false;
            }
        }
    }
    return true;
}

bool MainWindow::isValidRowOrColumn(const std::vector<std::vector<int>>& puzzle, int index)
{
    std::vector<bool> rowCheck(gridSize + 1, false);
    std::vector<bool> colCheck(gridSize + 1, false);

    for (int i = 0; i < gridSize; i++) {
        int rowValue = puzzle[index][i];
        if (rowValue != 0) {
            if (rowCheck[rowValue]) {
                qDebug() << "Duplicate found in row: " << rowValue << " at position (" << index << ", " << i << ")";
                return false;
            }
            rowCheck[rowValue] = true;
        }

        int colValue = puzzle[i][index];
        if (colValue != 0) {
            if (colCheck[colValue]) {
                qDebug() << "Duplicate found in column: " << colValue << " at position (" << i << ", " << index << ")";
                return false;
            }
            colCheck[colValue] = true;
        }
    }
    return true;
}

bool MainWindow::isValidSubGrid(const std::vector<std::vector<int>>& puzzle, int startRow, int startCol, int subGridSize)
{
    std::vector<bool> check(gridSize + 1, false);
    for (int i = 0; i < subGridSize; i++) {
        for (int j = 0; j < subGridSize; j++) {
            int value = puzzle[startRow + i][startCol + j];
            if (value != 0) {
                if (check[value]) {
                    qDebug() << "Duplicate found in sub grid: " << value << " at position (" << startRow + i << ", " << startCol + j << ")";
                    return false;
                }
                check[value] = true;
            }
        }
    }
    return true;
}

// Clears the grid after user clicks on "Clear" button
void MainWindow::on_clearButton_clicked()
{
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            grid[i][j]->clear();
        }
    }
}

bool MainWindow::solveSudoku(std::vector<std::vector<int>>& grid, int size)
{
    int row, col;
    if (!findUnassignedCell(grid, size, row, col)) {
        return true;
    }

    for (int num = 1; num <= size; num++) {
        if (isValid(grid, row, col, num, size)) {
            grid[row][col] = num;

            if (solveSudoku(grid, size)) {
                return true;
            }
            // Backtracks if unable to solve
            grid[row][col] = 0;
        }
    }
    return false;
}

// Finds empty cell
bool MainWindow::findUnassignedCell(const std::vector<std::vector<int>>& grid, int size, int& row, int& col)
{
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (grid[i][j] == 0) {
                row = i;
                col = j;
                return true;
            }
        }
    }
    return false;
}

// Checks if num is valid for row, column or sub grid
bool MainWindow::isValid(const std::vector<std::vector<int>>& grid, int row, int col, int num, int size)
{
    int subgridSize = std::sqrt(size);

    for (int i = 0; i < size; i++) {
        if (grid[row][i] == num || grid[i][col] == num)
            return false;
    }

    int startRow = row / subgridSize * subgridSize;
    int startCol = col / subgridSize * subgridSize;
    for (int i = 0; i < subgridSize; i++) {
        for (int j = 0; j < subgridSize; j++) {
            if (grid[startRow + i][startCol + j] == num)
                return false;
        }
    }
    return true;
}
