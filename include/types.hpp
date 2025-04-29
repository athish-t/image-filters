#pragma once

#include <opencv2/opencv.hpp>

using uchar = unsigned char;
using vec3 = std::array<uchar, 3>;

template <typename T>
class FlatArray {
    public:
        FlatArray() : _rows(0), _cols(0) {} // Default constructor

        FlatArray(int rows, int cols) : _rows(rows), _cols(cols), _data(rows * cols, 0) {}

        const std::vector<T>& data() const {
            return _data;
        }

        inline T& operator()(int i, int j) {
            return _data[i * _cols + j];
        }

        inline const T& operator()(int i, int j) const {
            return _data[i * _cols + j];
        }

        void resize(int rows, int cols) {
            _rows = rows;
            _cols = cols;
            _data.resize(rows * cols, 0);
        }

        size_t size() const {
            return _data.size();
        }

        inline T& operator[](size_t idx) {
            return _data[idx];
        }

        inline const T& operator[](size_t idx) const {
            return _data[idx];
        }

        // Iteration support
        auto begin() { return _data.begin(); }
        auto end() { return _data.end(); }
        auto begin() const { return _data.begin(); }
        auto end() const { return _data.end(); }

        int rows() const {
            return _rows;
        }

        int cols() const {
            return _cols;
        }

        bool empty() const {
            return _data.empty();
        }

    private:
        friend class FlatImageFactory;

        int _rows, _cols;
        std::vector<T> _data;
};

typedef FlatArray<uchar> FlatImage;

class FlatImageFactory {
public:
    static FlatImage from(const cv::Mat& mat) {
        if (mat.type() != CV_8UC1) {
            throw std::invalid_argument("Only single-channel 8-bit images are supported when converting from cv::Mat.");
        }
        FlatImage flatImage(mat.rows, mat.cols);
        std::copy(mat.data, mat.data + mat.total(), flatImage._data.begin());
        return flatImage;
    }

    template <typename T>
    static FlatImage from(const std::vector<std::vector<T>>& array2d) {
        if (array2d.empty() || array2d[0].empty()) {
            throw std::invalid_argument("Input 2D array cannot be empty.");
        }
        int rows = array2d.size();
        int cols = array2d[0].size();
        FlatImage flatImage(rows, cols);
        for (int i = 0; i < rows; ++i) {
            if (array2d[i].size() != cols) {
                throw std::invalid_argument("All rows in the 2D array must have the same number of columns.");
            }
            for (int j = 0; j < cols; ++j) {
                flatImage(i, j) = static_cast<uchar>(array2d[i][j]);
            }
        }
        return flatImage;
    }
};
