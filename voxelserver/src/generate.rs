use crate::core::{Block, ChunkData, IndexId, CSIZE};


pub fn generate(id: IndexId) -> ChunkData {
    let mut blocks: [[[Block; CSIZE]; CSIZE]; CSIZE] = [[[Block::Air; CSIZE]; CSIZE]; CSIZE];
    for xi in 0..CSIZE {
    for zi in 0..CSIZE {
    for yi in 0..CSIZE {
    let mut blk = Block::Air;
    let x = (id.x * CSIZE as i32) + xi as i32;
    let y = (id.y * CSIZE as i32) + yi as i32;
    let z = (id.z * CSIZE as i32) - zi as i32;
    // Block Creation
    let h = (((x as f32 * 0.3).sin() * (z as f32 * 0.2).sin())*10.0+15.0) as i32;
    if y < h {blk = Block::Stone;}
    if y == h {blk = Block::Grass;}
    blocks[xi as usize][yi as usize][zi as usize] = blk;
    }}}
    ChunkData{blocks}
}