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
    let packet = OutgoingPacket(vec![
        ServerSection::ServerSendBlockUpdate(core::CompressedBlockUpdate {
             chunk: IndexId { x: 1, y: 2, z: 3 },
             block: IndexId { x: 4, y: 5, z: 6 },
             new: Block::Grass,
            }),
    ]);
    //dbg!(&packet);
    //dbg!(packet.into_buffer());


    // Game State
    let mut game = Game::new();
    game.world.insert(
        IndexId{x:4,y:5,z:6}, ChunkData{
            blocks: [[[Block::Grass; CSIZE]; CSIZE]; CSIZE]
        }
    );

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
