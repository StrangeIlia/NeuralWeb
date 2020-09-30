#ifndef MATRIXONROW_HPP
#define MATRIXONROW_HPP

#include <vector>
#include <stdexcept>

/// Qt не любит шаблоны, поэтому реализация прямо в файле с заголовками
template<class _BaseType, class _IndexType = int>
class MatrixOnRow
{
public:
    typedef     _BaseType       BaseType;
    typedef     _IndexType      IndexType;
private:
    int _rows, _columns;
    std::vector<BaseType> _matrix;
public:
    MatrixOnRow();
    MatrixOnRow(MatrixOnRow&& othen);
    MatrixOnRow(const MatrixOnRow& othen);
    /// Задает матрицу заданного размера, раполняет базовым значением типа
    MatrixOnRow(IndexType _rows, IndexType _columns);

    inline MatrixOnRow& operator = (MatrixOnRow&& othen);
    inline MatrixOnRow& operator = (const MatrixOnRow& othen);

    /// Вставляет в матрицу count строк, заполнениых нулями
    inline void insertRows(IndexType index, IndexType count);
    /// Вставляет в матрицу count столбцов, заполнениых нулями
    inline void insertColumns(IndexType index, IndexType count);
    /// Удаляет из матрицы строки с first по last включительно ([first; last])
    inline void removeRows(IndexType first, IndexType last);
    /// Удаляет из матрицы строки с first по last включительно ([first; last])
    inline void removeColumns(IndexType first, IndexType last);

    /// Изменяет число строк, сохранение данных не гарантировано
    inline void setRows(IndexType _rows);
    /// Изменяет число столбцов, сохранение данных не гарантировано
    inline void setColumns(IndexType _columns);
    /// Изменяет число строк и столбцов, сохранение данных не гарантировано
    inline void setSize(IndexType _rows, IndexType _columns);

    inline IndexType rows() const;
    inline IndexType columns() const;

    inline void setCell(IndexType i, IndexType j, const BaseType&);
    inline const BaseType& cell(IndexType i, IndexType j) const;

    inline BaseType& operator() (IndexType i, IndexType j);
    inline const BaseType& operator() (IndexType i, IndexType j) const;

    /// Методы для рефлексии
    inline std::vector<BaseType>& getBaseRow();
    inline const std::vector<BaseType>& getBaseRow() const;
};

template<class BaseType, class IndexType>
MatrixOnRow<BaseType, IndexType>::MatrixOnRow() : _rows(0), _columns(0) {

}

template<class BaseType, class IndexType>
MatrixOnRow<BaseType, IndexType>::MatrixOnRow(MatrixOnRow&& othen) :
    _rows(othen._rows), _columns(othen._columns), _matrix(std::move(othen._matrix))  {
    othen._rows = othen._columns = 0;
}

template<class BaseType, class IndexType>
MatrixOnRow<BaseType, IndexType>::MatrixOnRow(const MatrixOnRow& othen) :
    _rows(othen._rows), _columns(othen._columns), _matrix(othen._matrix) {
}

template<class BaseType, class IndexType>
MatrixOnRow<BaseType, IndexType>::MatrixOnRow(IndexType rows, IndexType columns) {
    if(_rows < 0) throw std::invalid_argument("MatrixOnRow::setSize: The number of rows cannot be negative");
    if(_columns < 0) throw std::invalid_argument("MatrixOnRow::setSize: The number of columns cannot be negative");
    _rows = rows;
    _columns = columns;
    _matrix.resize(_rows * _columns);
}

template<class BaseType, class IndexType>
MatrixOnRow<BaseType, IndexType>& MatrixOnRow<BaseType, IndexType>::operator = (MatrixOnRow&& othen) {
    _rows = othen._rows;
    _columns = othen._columns;
    _matrix = std::move(othen._matrix);
    othen._rows = othen._columns = 0;
}

template<class BaseType, class IndexType>
MatrixOnRow<BaseType, IndexType>& MatrixOnRow<BaseType, IndexType>::operator = (const MatrixOnRow& othen) {
    _rows = othen._rows;
    _columns = othen._columns;
    _matrix = othen._matrix;
}

template<class BaseType, class IndexType>
void MatrixOnRow<BaseType, IndexType>::insertRows(IndexType index, IndexType count) {
    if(index < 0) throw std::invalid_argument("MatrixOnRow::insertRows: index не может быть отрицательным числом");
    if(index > _rows) throw std::invalid_argument("MatrixOnRow::insertRows: index не может быть больше количества строк");
    if(count < 0) throw std::invalid_argument("MatrixOnRow::insertRows: count не может быть отрицательным числом");
    _matrix.insert(_matrix.begin() + index * _columns, count * _columns, 0);
    _rows += count;
}

template<class BaseType, class IndexType>
void MatrixOnRow<BaseType, IndexType>::insertColumns(IndexType index, IndexType count) {
    if(index < 0) throw std::invalid_argument("MatrixOnRow::insertColumns: index не может быть отрицательным числом");
    if(index > _columns) throw std::invalid_argument("MatrixOnRow::insertColumns: index не может быть больше количества строк");
    if(count < 0) throw std::invalid_argument("MatrixOnRow::insertColumns: count не может быть отрицательным числом");
    _matrix.reserve(_matrix.size() + _rows * count);
    _columns += count;
    for(int i = 0; i != _rows; ++i) {
        auto shift = _matrix.begin() + i * _columns;
        _matrix.insert(shift + index, count, 0);
    }
}

template<class BaseType, class IndexType>
void MatrixOnRow<BaseType, IndexType>::removeRows(IndexType first, IndexType last) {
    if(last < first)  throw std::invalid_argument("MatrixOnRow::removeRows: last не может быть меньше first");
    if(first < 0) throw std::invalid_argument("MatrixOnRow::removeRows: first не может быть отрицательным числом");
    if(first > _rows) throw std::invalid_argument("MatrixOnRow::removeRows: first не может быть больше количества строк");
    if(last < 0) throw std::invalid_argument("MatrixOnRow::removeRows: first не может быть отрицательным числом");
    if(last > _rows) throw std::invalid_argument("MatrixOnRow::removeRows: first не может быть больше количества строк");
    _matrix.erase(_matrix.begin() + first * _columns, _matrix.begin() + (last + 1) * _columns);
    _rows -= last - first + 1;
}

template<class BaseType, class IndexType>
void MatrixOnRow<BaseType, IndexType>::removeColumns(IndexType first, IndexType last) {
    if(last < first)  throw std::invalid_argument("MatrixOnRow::removeColumns: last не может быть меньше first");
    if(first < 0) throw std::invalid_argument("MatrixOnRow::removeColumns: first не может быть отрицательным числом");
    if(first > _columns) throw std::invalid_argument("MatrixOnRow::removeColumns: first не может быть больше количества столбцов");
    if(last < 0) throw std::invalid_argument("MatrixOnRow::removeColumns: first не может быть отрицательным числом");
    if(last > _columns) throw std::invalid_argument("MatrixOnRow::removeColumns: first не может быть больше количества столбцов");
    _columns -= first - last + 1;
    for(int i = 0; i != _rows; ++i) {
        auto shift = _matrix.begin() + i * _columns;
        _matrix.erase(shift + first, shift + last + 1);
    }
}

template<class BaseType, class IndexType>
void MatrixOnRow<BaseType, IndexType>::setRows(IndexType rows) {
    setSize(rows, _columns);
}

template<class BaseType, class IndexType>
void MatrixOnRow<BaseType, IndexType>::setColumns(IndexType columns) {
    setSize(_rows, columns);
}

template<class BaseType, class IndexType>
void MatrixOnRow<BaseType, IndexType>::setSize(IndexType rows, IndexType columns) {
    if(_rows == rows && _columns == columns) return;
    if(_rows < 0) throw std::invalid_argument("MatrixOnRow::setSize: The number of rows cannot be negative");
    if(_columns < 0) throw std::invalid_argument("MatrixOnRow::setSize: The number of columns cannot be negative");
    _rows = rows;
    _columns = columns;
    if(_rows != 0 && _columns != 0)
        _matrix.resize(rows * columns);
}

template<class BaseType, class IndexType>
IndexType MatrixOnRow<BaseType, IndexType>::rows() const {
    return _rows;
}

template<class BaseType, class IndexType>
IndexType MatrixOnRow<BaseType, IndexType>::columns() const {
    return _columns;
}

template<class BaseType, class IndexType>
void MatrixOnRow<BaseType, IndexType>::setCell(IndexType i, IndexType j, const BaseType& value) {
    _matrix[i * _columns + j] = value;
}

template<class BaseType, class IndexType>
const BaseType& MatrixOnRow<BaseType, IndexType>::cell(IndexType i, IndexType j) const {
    return _matrix[i * _columns + j];
}

template<class BaseType, class IndexType>
BaseType& MatrixOnRow<BaseType, IndexType>::operator() (IndexType i, IndexType j) {
    return _matrix[i * _columns + j];
}

template<class BaseType, class IndexType>
const BaseType& MatrixOnRow<BaseType, IndexType>::operator() (IndexType i, IndexType j) const {
    return _matrix[i * _columns + j];
}

template<class BaseType, class IndexType>
std::vector<BaseType>& MatrixOnRow<BaseType, IndexType>::getBaseRow() {
    return _matrix;
}

template<class BaseType, class IndexType>
const std::vector<BaseType>& MatrixOnRow<BaseType, IndexType>::getBaseRow() const {
    return _matrix;
}

#endif // MATRIXONROW_HPP
