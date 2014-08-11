#ifndef RESOURCE_CACHE_H
#define RESOURCE_CACHE_H

#include <map>
#include <memory>
#include <string>

#include "graphics_context.hpp"



template<typename Res>
class CacheableResource;



///
/// Contains resources asscociated with a common GL context.
///
/// Res must be a sub-class of CacheableResource.
///
template<typename Res>
class ResourceCache {
private:
    friend class CacheableResource<Res>;
    ///
    /// A weak pointer to itself, which can be given to resources.
    ///
    std::weak_ptr<ResourceCache<Res>> weak_this;
    ///
    /// Map from names to resource pointers.
    ///
    std::map<std::string, std::weak_ptr<Res>> resources;
public:
    ///
    /// Creates a cache and registers it with the context for clean-up.
    ///
    ResourceCache(GraphicsContext* context);
    ~ResourceCache();
    
    ///
    /// Get a resource from a resource name.
    ///
    /// If already loaded, retrieves a resource from the cache.
    /// Otherwise, the resource is loaded.
    ///
    /// @param resource_name Resource name given to resource
    ///        constructor.
    /// @return A resource pointer to the relevant resource.
    ///
    std::shared_ptr<Res> get_resource(const std::string resource_name);

    ///
    /// Removes a resource from the cache. Does not destroy it.
    ///
    void remove_resource(const std::string resource_name);
};



#include "resource_cache.hxx"



#endif
