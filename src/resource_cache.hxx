// //////////////////////////////////////////////////////////////
// //////////////////////// CURRENT BUGS ////////////////////////
// //////////////////////////////////////////////////////////////
//  Caches are created twice and destroyed once due to copying.
//      When inserting into the map (pair?). This doesn't do any harm,
//      it's just weird.

#include <map>
#include <memory>
#include <utility>
#include <string>

#include "resource_cache.hpp"
#include "cacheable_resource.hpp"
#include "graphics_context.hpp"



template<typename Res>
ResourceCache<Res>::ResourceCache(GraphicsContext*):
    weak_this()
{
    LOG(INFO) << "Created resource cache " << this;
}


template<typename Res>
ResourceCache<Res>::~ResourceCache() {
    // We do not clean up the resources here. This will be done by
    // shared pointers after they are no longer needed.
    LOG(INFO) << "Destroyed resource cache " << this;
}


template<typename Res>
std::shared_ptr<Res> ResourceCache<Res>::get_resource(const std::string resource_name) {
    LOG(INFO) << "Getting resource \"" << resource_name << "\" from cache " << this;
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
    LOG(INFO) << "Removing resource \"" << resource_name << "\" from cache " << this;
    resources.erase(resource_name);
}
