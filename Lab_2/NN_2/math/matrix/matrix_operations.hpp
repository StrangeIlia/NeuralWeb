#ifndef MATRIXOPERATIONS_H
#define MATRIXOPERATIONS_H

#include <limits>

#include "MatrixOnRow.hpp"

class MatrixOperations
{
public:
    MatrixOperations();

    template<class Base, class Index = int>
    static void swapRows(MatrixOnRow<Base, Index> &matrix, int firstRow, int secondRow) {
        auto &vector = matrix.getBaseRow();
        auto first = vector.begin() + firstRow * matrix.columns();
        auto second = vector.begin() + secondRow * matrix.columns();
        std::swap_ranges(first, first + matrix.columns(), second);
    }

    template<class Base, class Index = int>
    static bool slay(MatrixOnRow<Base, Index> &left, MatrixOnRow<Base, Index> &right) {
        if(left.rows() != right.rows()) return false;
        if(left.rows() > left.columns()) return false;

        double eps = std::numeric_limits<Base>::epsilon();

        for(Index i = 0; i != left.rows(); ++i) {
            Index bestIndex = i;
            for(Index ii = i; ii != left.rows(); ++ii) {
                if(std::abs(left(ii, i)) > eps) {
                    bestIndex = ii;
                    break;
                }
            }
            if(std::abs(left(bestIndex, i)) < eps) {
                return false;
            }
            if(bestIndex != i) {
                swapRows(left, i, bestIndex);
                swapRows(right, i, bestIndex);
            }

            auto selectedValue = left(i, i);
            for(int j = i; j != left.columns(); ++j)
                left(i, j) /= selectedValue;
            for(int j = 0; j != right.columns(); ++j)
                right(i, j) /= selectedValue;

            for(int ii = 0; ii != left.rows(); ++ii) {
                if(i == ii) continue;
                selectedValue = -left(ii, i);

                for(int j = i; j != left.columns(); ++j)
                    left(ii, j) += selectedValue * left(i, j);
                for(int j = 0; j != right.columns(); ++j)
                    right(ii, j) += selectedValue * right(i, j);
            }
        }

        return true;
    }

};

#endif // MATRIXOPERATIONS_H
