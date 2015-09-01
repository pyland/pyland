#ifndef CACHEABLE_RESOURCE_H
#define CACHEABLE_RESOURCE_H

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "callback.hpp"
#include "graphics_context.hpp"
#include "resource_cache.hpp"

template<typename Res>
class ResourceCache;

class GraphicsContext;

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

    void clear(){
	resource_cache.lock()->clear();
	resource_caches.clear();
    }
};

template<typename Res>
std::map<GraphicsContext*, std::shared_ptr<ResourceCache<Res>>> CacheableResource<Res>::resource_caches;

template<typename Res>
CacheableResource<Res>::CacheableResource():
    resource_cache() {
}

template<typename Res>
CacheableResource<Res>::~CacheableResource() {
    if (std::shared_ptr<ResourceCache<Res>> resource_cache_shared = resource_cache.lock()) {
        resource_cache_shared->remove_resource(resource_name);
    }
}

template<typename Res>
std::shared_ptr<Res> CacheableResource<Res>::get_shared(const std::string resource_name) {
    GraphicsContext* context = GraphicsContext::get_current();

    if (resource_caches.count(context) == 0) {
        // Create a new resource cache as this is the first of its context.
        std::shared_ptr<ResourceCache<Res>> resource_cache = std::make_shared<ResourceCache<Res>>(context);
        // Tell it who it is.
        resource_cache->weak_this = std::weak_ptr<ResourceCache<Res>>(resource_cache);
        resource_caches.insert(std::make_pair(context, resource_cache));
        context->register_resource_releaser(std::function<void()>([context] () {resource_caches.erase(context);}));
    }

    return resource_caches.find(context)->second->get_resource(resource_name);
}

template<typename Res>
std::shared_ptr<Res> CacheableResource<Res>::new_shared(const std::string resource_name) {
    std::shared_ptr<Res> resource = Res::new_resource(resource_name);
    resource->resource_name = resource_name;
    return resource;
}

#endif
