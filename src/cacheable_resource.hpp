#ifndef CACHEABLE_RESOURCE_H
#define CACHEABLE_RESOURCE_H

#include <map>
#include <memory>
#include <string>

#include "graphics_context.hpp"



template<typename Res>
class ResourceCache;



///
/// Template superclass for resources which can be added to a cache.
///
/// To make a resource, Res, cacheable:
///     make Res inherit from CacheableResource<Res>,
///     make Res friend CacheableResource<Res>,
///     implement void Res::new_resource(const std::string resource_name)
///
template<typename Res>
class CacheableResource {
private:
    friend class ResourceCache<Res>;
    ///
    /// Pointer to the resource cache (if any) which contains this.
    ///
    /// Resources don't require caching, so this is optional.
    ///
    std::weak_ptr<ResourceCache<Res>> resource_cache;
    
    ///
    /// Map of graphics contexts to shader caches.
    ///
    static std::map<GraphicsContext*, std::shared_ptr<ResourceCache<Res>>> resource_caches;

    ///
    /// Creates a new resource using the give resource name.
    ///
    static std::shared_ptr<Res> new_shared(const std::string resource_name);
protected:
    ///
    /// The name which was used to load the resource.
    ///
    std::string resource_name;
    
    CacheableResource();
    ~CacheableResource();
    
public:
    ///
    /// Get a commonly used resource configuration.
    ///
    /// This function is used to share resources between separate
    /// graphical components. On first run with given parameters it will
    /// load the resource, on other calls it will retrieve it from a
    /// cache. There is a separate cache for each GL context.
    ///
    /// @param resource_name A string which represents a resource.
    /// @return A shared pointer to the relevant resource.
    ///
    static std::shared_ptr<Res> get_shared(const std::string resource_name);
};



#include "cacheable_resource.hxx"



#endif
