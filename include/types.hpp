template <typename T>
class FlatArray {
    public:
        FlatArray() : _rows(0), _cols(0) {} // Default constructor

        FlatArray(int rows, int cols) : _rows(rows), _cols(cols), _data(rows * cols, 0) {}

        T& operator()(int i, int j) {
            return _data[i * _cols + j];
        }

        const T& operator()(int i, int j) const {
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

        T& operator[](size_t idx) {
            return _data[idx];
        }

        const T& operator[](size_t idx) const {
            return _data[idx];
        }

        // Iteration support
        auto begin() { return _data.begin(); }
        auto end() { return _data.end(); }
        auto begin() const { return _data.begin(); }
        auto end() const { return _data.end(); }

    private:
        int _rows, _cols;
        std::vector<T> _data;
};
