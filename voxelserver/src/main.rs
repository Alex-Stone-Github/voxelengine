//! Note that this package represents a server for a simple voxel game I am
//! working on it is not necessarily secure or good or fast. It is a learning
//! experience for me to get better at c++ and rust programming.

use client::GameClient;
use core::{ChunkData, IndexId};
use std::sync::{Arc, Mutex};

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
    // Game State
    // TODO: Increase performance of queue
    let game = Game::new();

    // Spin up the incoming thread
    let clients = Arc::clone(&game.clients);
    std::thread::spawn(move || {
        listener::connection_listener(clients);
    });

    loop {
        game.clients.lock().unwrap().iter().for_each(|client| {
            client.lock().unwrap().step();
        });
        std::thread::sleep(std::time::Duration::from_millis(500));
    }
}
