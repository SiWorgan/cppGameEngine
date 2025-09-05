#ifndef POOL_H
#define POOL_H

class IPool {
    public:
        virtual ~IPool() {}
};

template <typename T>
class Pool: public IPool {
    private:
        std::vector<T> data;

    public:
        Pool(int size = 100){
            data.resize(size);
        }

        virtual ~Pool() = default;

        bool isEmpty() const { return data.empty(); }
        int GetSize() const { return data.size(); }
        void Resize(int n) { data.resize(n); }
        void Clear() { data.clear(); }
        void Add(T object) { data.push_back(object); }
        void Set(int index, T object) { data[index] = object; }
        T& Get(int index) { return static_cast<T&>(data[index]); }
        T& operator [](unsigned int index) { return data[index]; }
};

#endif