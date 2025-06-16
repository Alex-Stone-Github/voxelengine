use crate::core::{ChunkData, IndexId};
use crate::serverpacket::ServerSection;
use crate::{SharedResource};
use crate::client::GameClient;
use crate::gametypes::{Vector3, EntityInfo};
use std::sync::{Arc, Mutex};


pub fn connection_listener(game: SharedResource<crate::Game>, port: u16) {
    let hostname = format!("127.0.0.1:{}", port);
    println!("Starting server on port {}", port);
    let server = std::net::TcpListener::bind(hostname.as_str())
            .expect("Port already in use");

    for potential_connection in server.incoming() {
        if let Ok(connection) = potential_connection {
            // Create a new client from a connection and add it to the queue
            let tmp_game = game.lock().unwrap();
            let mut client_queue = tmp_game.clients.lock().unwrap();
            let new_client = Arc::new(Mutex::new(GameClient::new(connection)));
            client_queue.push(Arc::clone(&new_client));
            // Give the client its own thread
            let tmp_client = Arc::clone(&new_client);
            let tmp_world = Arc::clone(&tmp_game.world);
            std::thread::spawn(move || {
                println!("Thread Started for new client({})!", tmp_client.lock().unwrap().unique);
                loop {
                    // Step
                    tmp_client.lock().unwrap().step(Arc::clone(&tmp_world));

                    // Add entities to the package
                    tmp_world.lock().unwrap().entities.values().for_each(|entity| {
                        tmp_client.lock().unwrap().outgoing.0.push(ServerSection::ServerSendEntityInfo(entity.clone()));
                    });
                }
            });
        }
    }
}