#include "cache.hpp"
#include <time.h>

using Utils::Cache::CacheHandler;
using Utils::Cache::Cache_Object;

// Get timestamp
size_t timestamp(){
    return static_cast<size_t>(std::time(nullptr));
}

template<typename T, typename K>
CacheHandler<T, K>::CacheHandler(uint ttl){
    _ttl = ttl;
}

template<typename T, typename K>
void CacheHandler<T, K>::ping(K key){
    // Check if K exists
    if(_elements.find(key) != _elements.end()){
        _elements[key].max_life = timestamp() + _ttl;
    }
}

template<typename T, typename K>
std::optional<T> CacheHandler<T, K>::get_element(K key){
    if(_elements.find(key) != _elements.end()){
        if(_elements[key].max_life >= timestamp()){
            _elements[key].max_life = timestamp()+_ttl;
            return std::optional<T>(*(_elements[key].object));
        }
        // too old ? 
        else{
            remove(key);
            return std::optional<T>(nullptr);
        }
    }
    else{
        return std::optional<T>(nullptr);
    }
}


template<typename T, typename K>
void CacheHandler<T, K>::remove(K key){
    _elements.erase(key);
}

template<typename T, typename K>
void CacheHandler<T, K>::store_element(K key, T object){
    // if the element already exists, just update its object
    Cache_Object<T> cobject;
    cobject.object = std::make_shared<T>(object);
    cobject.max_life = timestamp() + _ttl;

    if(_elements.find(key) != _elements.end()){
        _elements[key] = cobject;
    }
    else
        _elements.insert({key, cobject});
}
