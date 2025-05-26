#include "world.hpp"

ChunkData generate(ChunkId id) {
    ChunkData data;
    data.id = id;
    for (size_t ix = 0; ix < sizex; ix ++) {
        for (size_t iy = 0; iy < sizey; iy ++) {
            for (size_t iz = 0; iz < sizez; iz ++) {
                Block b = Air;
                float h = sin(ix*.2) * sin(iz*.3);
                if (iy < h*5+20) b = Stone;
                if (ix == 0 && iz == 0) b = Air;
                data.blocks[ix][iy][iz] = b;
            }
        }
    }
    return data;
}

void World::reload_check(ChunkId pos) {
    constexpr int render_dist = 2;
    for (int cx = -render_dist; cx < render_dist; cx ++) {
        for (int cz = -render_dist; cz < render_dist; cz ++) {
            ChunkId nid(pos.x+cx, pos.y+0, pos.z+cz);
            auto already_exists = std::ranges::any_of(live_chunks, [nid](LiveChunk const& chunk) {
                return nid == chunk.block_data.id;
            });
            if (already_exists) continue;

            // Generate the chunk
            auto b = generate(nid);
            LiveChunk chunky(b);
            chunky.position.x = static_cast<float>(pos.x+cx) * static_cast<float>(sizex) * blocksize;
            chunky.position.z = static_cast<float>(pos.z+cz) * static_cast<float>(sizez) * blocksize;
            live_chunks.emplace_back(std::move(chunky));
        }
    }
}
void World::dirty_check() {
    auto location = std::ranges::find_if(live_chunks, [](LiveChunk const& c){
        return c.dirty;
    });
    if (location == live_chunks.end()) return;
    LiveChunk& c = *location;
    recompute_mesh(c, &c, &c, &c, &c, &c, &c);
}