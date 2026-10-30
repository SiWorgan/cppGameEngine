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
        int size;

        // Track of entity id per index
        std::unordered_map<int, int> entityIdToIndex;
        std::unordered_map<int, int> indexToEntityId;

    public:
        Pool(int capacity = 100){
            size = 0;
            data.resize(capacity);
        }

        virtual ~Pool() = default;

        bool IsEmpty() const { return size == 0; }
        int GetSize() const { return size; }
        void Resize(int n) { data.resize(n); }

        void Clear() { 
            data.clear(); 
            size = 0;
        }

        void Add(T object) { data.push_back(object); }

        void Set(int entityId, T object) {
            if (entityIdToIndex.find(entity) != entityIdToIndex.end()) {
                int index = entityIdToIndex[entityId];
                data[index] = object;
            } else {
                int index = size;
                entityIdToIndex.emplace(entityId, index);
                indexToEntityId.emplace(index, entityId);
                if (index >= data.capacity()) {
                    data.resize(size * 2);
                }
                data[index] = object;
                size++;
            }
        }

        void Remove(int entityId) {
            // Copy the last element to the deleted position
            int indexOfRemoved = entityIdToIndex[entityId];
            int indexOfLast = size - 1;
            data[indexOfRemoved] = data[indexOfLast];

            // Update the index-entity maps
            int entityIdOfLastElement = indexToEntityId[indexOfLast];
            entityIdToIndex[entityIdOfLastElement] = indexOfRemoved;
            indexToEntityId[indexOfRemoved] = entityIdOfLastElement;

            entityIdToIndex.erase(entityId);
            indexToEntityId.erase(indexOfLast);

            size--;
        }

        T& Get(int entityId) { 
            int index = entityIdToIndex[entityId];
            return static_cast<T&>(data[index]); 
        }
        T& operator [](unsigned int index) { return data[index]; }
};

#endif