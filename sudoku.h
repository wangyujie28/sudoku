#ifndef SUDOKU_H
#define SUDOKU_H

#include <vector>

using sudoku_vec = std::vector<std::vector<int>>;

/*
In:  inputs 输入数独矩阵,使用-1代表需要计算的数据
Out: bool   输出数独结果 false代表无解  true代表有解
方法1：使用递归回溯的方法解决，只要找到一种可行解就返回
方法2：使用Dance Link舞蹈链来解决这个问题(原始舞蹈链，未优化)
方法3：使用Dance Link舞蹈链来解决这个问题(优化)
*/

bool FindSudokuAnswer(sudoku_vec& inputs);

bool FindSudokuAnswer2(sudoku_vec& inputs);

bool FindSudokuAnswer3(sudoku_vec& inputs);

#endif