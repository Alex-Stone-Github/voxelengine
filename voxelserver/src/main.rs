//! Note that this package represents a server for a simple voxel game I am
//! working on it is not necessarily secure or good or fast. It is a learning
//! experience for me to get better at c++ and rust programming.

use client::GameClient;
use serverpacket::{OutgoingPacket, ServerSection};
use core::{ChunkData, IndexId, CSIZE};
use std::sync::{Arc, Mutex};
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
    const DST: i32 = 6;
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
            // Testing Code
            client.lock().unwrap().outgoing.0.push(ServerSection::ServerSendEntityInfo(
                EntityInfo {
                    position: Vector3::new(9.0, 32.0, -9.0),
                    yaw: std::f32::consts::FRAC_PI_4,
                }
            ));
            client.lock().unwrap().outgoing.0.push(ServerSection::ServerSendEntityInfo(
                EntityInfo {
                    position: Vector3::new(0.0, 32.0, -9.0),
                    yaw: -std::f32::consts::FRAC_PI_4,
                }
            ));
            client.lock().unwrap().outgoing.0.push(ServerSection::ServerSendEntityInfo(
                EntityInfo {
                    position: Vector3::new(1.0, 32.0, -3.0),
                    yaw: 0.0,
                }
            ));
        });
        // Continue?
    }
}
