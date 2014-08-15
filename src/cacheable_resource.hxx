#include <map>
#include <memory>
#include <utility>
#include <string>



#include "cacheable_resource.hpp"
#include "resource_cache.hpp"
#include "graphics_context.hpp"



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
