//! Note that this package represents a server for a simple voxel game I am
//! working on it is not necessarily secure or good or fast. It is a learning
//! experience for me to get better at c++ and rust programming.

use client::GameClient;
use serverpacket::{OutgoingPacket, ServerSection};
use core::{ChunkData, IndexId, CSIZE};
use std::{any::Any, env::Args, sync::{Arc, Mutex}};
use crate::{core::Block, gametypes::{EntityInfo, Vector3}};

mod listener;
mod client;
mod byteutil;
mod core;
mod clientpacket;
mod serverpacket;
mod generate;
mod gametypes;

pub type SharedResource<T> = Arc<Mutex<T>>;

struct World {
    chunks: std::collections::HashMap<IndexId, ChunkData>,
    entities: std::collections::HashMap<usize, EntityInfo>,
}
struct Game {
    clients: SharedResource<Vec<SharedResource<GameClient>>>,
    world: SharedResource<World>,
}
impl Game {
    fn new() -> Self {
        Self {
            clients: Arc::new(Mutex::new(vec![])),
            world: Arc::new(Mutex::new(World{
                chunks: std::collections::HashMap::new(),
                entities: std::collections::HashMap::new(),
            })),
        }
    }
}

fn main() {
    // Get port
    let mut port_no = 8000;
    let args: Vec<String> = std::env::args().collect();
    if args.len() == 2 {
        port_no = args[1].parse().expect("Invalid port argument");
    }

    // Game State
    let game = Arc::new(Mutex::new(Game::new()));
    const DST: i32 = 6;
    for x in -DST..DST {
        for z in -DST..DST {
            game.lock().unwrap().world.lock().unwrap().chunks.insert(
                IndexId{x,y:0,z}, generate::generate(IndexId{x,y:0,z})
            );
        }
    }

    // Spin up the incoming thread
    let tmp_game = Arc::clone(&game);
    std::thread::spawn(move || {
        listener::connection_listener(tmp_game, port_no);
    });

    // Logic Processing Thread
    let mut i = 0.0;
    loop {
        i += 0.001;
        // Deltatime calculations (TODO)
        let _ = game.lock().unwrap().world.lock().unwrap().entities
            .insert(1000, EntityInfo { position: 
                Vector3 { x: 0.0, y: 33.0, z: 0.0 }, yaw: i });
        std::thread::sleep(std::time::Duration::from_millis(10));
    }
}
