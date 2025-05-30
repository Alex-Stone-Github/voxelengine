//! Note that this package represents a server for a simple voxel game I am
//! working on it is not necessarily secure or good or fast. It is a learning
//! experience for me to get better at c++ and rust programming.

use client::GameClient;
use serverpacket::{OutgoingPacket, ServerSection};
use core::{ChunkData, IndexId, CSIZE};
use std::sync::{Arc, Mutex};
use crate::core::Block;

mod listener;
mod client;
mod byteutil;
mod core;
mod clientpacket;
mod serverpacket;
mod generate;

pub type SharedResource<T> = Arc<Mutex<T>>;

struct Game {
    clients: SharedResource<Vec<SharedResource<GameClient>>>,
    world: std::collections::HashMap<IndexId, ChunkData>,
}
impl Game {
    fn new() -> Self {
        Self {
            clients: Arc::new(Mutex::new(vec![])),
            world: std::collections::HashMap::new(),
        }
    }
}


fn main() {
    // Game State
    let mut game = Game::new();
    /* 
    game.world.insert(
        IndexId{x:1,y:0,z:0}, ChunkData{
            blocks: [[[Block::Grass; CSIZE]; CSIZE]; CSIZE]
        }
    );
    game.world.insert(
        IndexId{x:0,y:0,z:1}, ChunkData{
            blocks: [[[Block::Stone; CSIZE]; CSIZE]; CSIZE]
        }
    );
    game.world.insert(
        IndexId{x:-1,y:0,z:-1}, ChunkData{
            blocks: [[[Block::Stone; CSIZE]; CSIZE]; CSIZE]
        }
    );
    game.world.insert(
        IndexId{x:-1,y:0,z:0}, generate::generate(IndexId{x:-1,y:0,z:0})
    );
    game.world.insert(
        IndexId{x:-2,y:0,z:0}, generate::generate(IndexId{x:-2,y:0,z:0})
    );
    */
    const DST: i32 = 3;
    for x in -DST..DST {
        for z in -DST..DST {
            game.world.insert(
                IndexId{x,y:0,z}, generate::generate(IndexId{x,y:0,z})
            );
        }
    }

    // Spin up the incoming thread
    let clients = Arc::clone(&game.clients);
    std::thread::spawn(move || {
        listener::connection_listener(clients);
    });

    loop {
        game.clients.lock().unwrap().iter().for_each(|client| {
            client.lock().unwrap().step(&mut game.world);
        });
    }
}
