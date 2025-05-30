#include "world.hpp"


ChunkData generate(IndexId id) {
    ChunkData data;
    data.id = id;
    for (size_t ix = 0; ix < sizex; ix ++) {
        for (size_t iy = 0; iy < sizey; iy ++) {
            for (size_t iz = 0; iz < sizez; iz ++) {
                Block b = Air;
                float h = sin(ix*.2) * sin(iz*.3);
                if (iy < h*5+20) b = Stone;
                if (ix == 0 && iz == 0) b = Grass;
                data.blocks[ix][iy][iz] = b;
            }
        }
    }
    return data;
}

void World::reload_check(IndexId pos) {
    constexpr int render_dist = 2;
    for (int cx = -render_dist; cx < render_dist; cx ++) {
        for (int cz = -render_dist; cz < render_dist; cz ++) {
            IndexId nid(pos.x+cx, pos.y+0, pos.z+cz);
            auto already_exists = std::ranges::any_of(live_chunks, [nid](LiveChunk const& chunk) {
                return nid == chunk.block_data.id;
            });
            if (already_exists) continue;

            // Generate a new chunk from local data
            std::lock_guard<std::mutex> lock(lcguard);
            auto lc = lcchunk.find(nid);
            if (lc == lcchunk.end()) continue; // TODO: Do not generate the chunk
            lock.~lock_guard();


            auto& cd = lc->second;
            LiveChunk chunky(cd);
            chunky.position.x = static_cast<float>(cd.id.x) * static_cast<float>(sizex) * blocksize;
            chunky.position.y = static_cast<float>(cd.id.y) * static_cast<float>(sizey) * blocksize;
            chunky.position.z = static_cast<float>(cd.id.z) * static_cast<float>(sizez) * blocksize;
            live_chunks.emplace_back(std::move(chunky));

            // Mark Neighbrs to recompute their own meshes
            auto neighbors = find_neighbors(nid);
            std::ranges::for_each(neighbors, [](std::optional<LiveChunk*> neighbor) {
                if (neighbor.has_value()) neighbor.value()->dirty = true;
            });
        }
    }
}
void World::dirty_check() {
    auto location = std::ranges::find_if(live_chunks, [](LiveChunk const& c){
        return c.dirty;
    });
    if (location == live_chunks.end()) return;
    auto& c = *location;

    auto [north, west, south, east, up, down] = find_neighbors(c.block_data.id);
    recompute_mesh(c, north, west, south, east, up, down);
}
std::optional<LiveChunk*> World::find_by_id(IndexId id) {
    auto location = std::ranges::find_if(this->live_chunks, 
        [id](LiveChunk const& chunk) {
            return chunk.block_data.id == id;
        });
    if (location == live_chunks.end()) return std::nullopt;
    return &(*location);
}
std::array<std::optional<LiveChunk*>, 6> World::find_neighbors(IndexId id) {
    auto posid = id;
    auto north = find_by_id(IndexId(posid.x, posid.y, posid.z-1));
    auto west =  find_by_id(IndexId(posid.x-1, posid.y, posid.z));
    auto south = find_by_id(IndexId(posid.x, posid.y, posid.z+1));
    auto east =  find_by_id(IndexId(posid.x+1, posid.y, posid.z));
    auto up =    find_by_id(IndexId(posid.x, posid.y+1, posid.z));
    auto down =  find_by_id(IndexId(posid.x, posid.y-1, posid.z));
    return {north, west, south, east, up, down};
}