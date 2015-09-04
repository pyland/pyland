#ifndef RESOURCE_CACHE_H
#define RESOURCE_CACHE_H

#include <exception>
#include <glog/logging.h>
#include <map>
#include <memory>
#include <ostream>
#include <utility>
#include <string>

template<typename Res>
class CacheableResource;

class GraphicsContext;

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

    ///
    /// Removes all the resources from the cache. Does not destoy it.
    ///
    void clear();
};

// //////////////////////////////////////////////////////////////
// //////////////////////// CURRENT BUGS ////////////////////////
// //////////////////////////////////////////////////////////////
//  Caches are created twice and destroyed once due to copying.
//      When inserting into the map (pair?). This doesn't do any harm,
//      it's just weird.

template<typename Res>
ResourceCache<Res>::ResourceCache(GraphicsContext*):
    weak_this() {
    VLOG(1) << "Created resource cache " << this;
}

template<typename Res>
ResourceCache<Res>::~ResourceCache() {
    // We do not clean up the resources here. This will be done by
    // shared pointers after they are no longer needed.
    VLOG(1) << "Destroyed resource cache " << this;
}

template<typename Res>
std::shared_ptr<Res> ResourceCache<Res>::get_resource(const std::string resource_name) {
    VLOG(3) << "Getting resource \"" << resource_name << "\" from cache " << this;

    if (resources.count(resource_name) == 0) {
        // First-time load.
        try {
            std::shared_ptr<Res> resource = Res::new_shared(resource_name);
            resources.insert(std::make_pair(resource_name, std::weak_ptr<Res>(resource)));
            resource->resource_cache = weak_this;
            return resource;
        }
        catch (std::exception &e) {
            LOG(ERROR) << "Error creating shared resource \"" << resource_name << "\": " << e.what();
            throw e;
        }
    }
    else {
        // Get from cache.
        std::shared_ptr<Res> resource = resources.find(resource_name)->second.lock();
        // resource->resource_cache = weak_this;

        // Some say we should check the pointer, but we don't keep dead
        // weak pointers lying around for us to care about.
        return resource;
    }
}

template<typename Res>
void ResourceCache<Res>::remove_resource(const std::string resource_name) {
    VLOG(1) << "Removing resource \"" << resource_name << "\" from cache " << this;
    resources.erase(resource_name);
}

template<typename Res>
void ResourceCache<Res>::clear() {
    VLOG(1) << "Clearing all resources from cache";
    resources.clear();
}
#endif
