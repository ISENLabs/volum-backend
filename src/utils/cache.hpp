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

template <typename T> 
struct Cache_Object{
    std::shared_ptr<T> object; // As it might be destroyed in its base context
    size_t max_life;
};

template <typename T, typename K>
class CacheHandler{
private:
    int _ttl;
    std::map<K, Cache_Object<T>> _elements; // K: key, T: type of the stored object

public:
    CacheHandler(uint ttl);
    std::optional<T> get_element(K key); // Returns the value if not expired. If expired, returns null + deletes the element.
    void store_element(K key, T object);
    void ping(K key); // Update the TTL for the element
    void remove(K key);// Remove the element
};
};