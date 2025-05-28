//! Note that this package represents a server for a simple voxel game I am
//! working on it is not necessarily secure or good or fast. It is a learning
//! experience for me to get better at c++ and rust programming.

use client::GameClient;
use std::{char::MAX, ptr::read_unaligned, sync::{Arc, Mutex}}

mod listener;
mod client;
mod byteutil;
mod core;
mod clientpacket;
mod serverpacket;

const MAX_CLIENTS: usize = 3;
struct Game {
    clients: [Arc<Mutex<Option<GameClient>>>; MAX_CLIENTS],
}
impl Game {
    pub fn new() -> Self {
        for i in 0..MAX_CLIENTS {
            let client = Arc::new(Mutex::new(None));
        }
        return Self {
        }
    }
}


fn main() {
    // Game State
    // TODO: Increase performance of queue
    let game = Game {
        clients: std::sync::Arc::new(std::sync::Mutex::new(Vec::new()))
    };

    // Spin up the incoming thread
    let clientqueue = std::sync::Arc::clone(&game.clients);
    std::thread::spawn(move || {
        listener::connection_listener(clientqueue);
    });

    loop {
        let clients = game.clients.lock();
        clients.iter().for_each(|client| {
        });
    }
}
