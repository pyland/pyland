#include "cacheable_resource.hpp"
#include "resource_cache.hpp"



template<typename Res>
std::map<GraphicsContext*, std::shared_ptr<ResourceCache<Res>>> CacheableResource<Res>::resource_caches;



template<typename Res>
CacheableResource<Res>::CacheableResource():
    resource_cache(nullptr) {
}


template<typename Res>
CacheableResource<Res>::~CacheableResource() {
    if (resource_cache != nullptr) {
        resource_cache->remove_resource(resource_name);
    }
}


template<typename Res>
std::shared_ptr<Res> CacheableResource<Res>::get_shared(const std::string program_name) {
    GraphicsContext* context = GraphicsContext::get_current();

    if (resource_caches.count(context) == 0) {
        // Create a new resource cache as this is the first of its context.
        std::shared_ptr<ResourceCache<Res>> resource_cache = std::make_shared<ResourceCache<Res>>(context);
        resource_caches.insert(std::make_pair(context, resource_cache));
        context->register_resource_releaser(std::function<void()>([context] () {resource_caches.erase(context);}));
    }

    return resource_caches.find(context)->second->get_resource(program_name);
}
