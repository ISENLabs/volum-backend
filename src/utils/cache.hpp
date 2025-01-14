#include <memory>
#include <map>
#include <iostream>
#include <string>
#include <optional>

namespace Utils::Cache{

/**
 * /!\ Warning
 * This cache is really simple and will NOT handle auto-deletion of stored object
 * Meaning that if not deleted explicitely by calling remove(), or not accessed, the object will be stored indefinitely.
 * The current implementation is good for our little project, with ~100s of objects, but MUST NOT be used in production otherwise.
*/

// Get timestamp
size_t timestamp(){
    return static_cast<size_t>(std::time(nullptr));
}

template <typename T> 
struct Cache_Object{
    std::shared_ptr<T> object; // As it might be destroyed in its base context
    size_t max_life;
};

template <typename K, typename T>
class CacheHandler{
private:
    int _ttl;
    std::map<K, Cache_Object<T>> _elements; // K: key, T: type of the stored object

public:
    CacheHandler(uint ttl){
        _ttl = ttl;
    }

    // Returns the value if not expired. If expired, returns null + deletes the element.
    std::optional<T> get_element(K key){
        if(_elements.find(key) != _elements.end()){
            if(_elements[key].max_life >= timestamp()){
                _elements[key].max_life = timestamp()+_ttl;
                return std::optional<T>(*(_elements[key].object));
            }
            // too old ? 
            else{
                remove(key);
                return std::nullopt;
            }
        }
        else{
            return std::nullopt;
        }
    }
    
    // Update the TTL for the element
    void ping(K key){
        // Check if K exists
        if(_elements.find(key) != _elements.end()){
            _elements[key].max_life = timestamp() + _ttl;
        }
    }
    
    // Remove the element
    void remove(K key){
        _elements.erase(key);
    }
    
    void store_element(K key, T object){
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


};
};