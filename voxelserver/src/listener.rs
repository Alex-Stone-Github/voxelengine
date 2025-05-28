use crate::{Game, SharedResource};
use crate::client::GameClient;
use std::sync::{Arc, Mutex};


pub fn connection_listener(clients: SharedResource<Vec<SharedResource<GameClient>>>) {
    let server = std::net::TcpListener::bind("127.0.0.1:8000")
            .expect("Port 8000 already in use");
    for potential_connection in server.incoming() {
        if let Ok(connection) = potential_connection {
            let mut client_queue = clients.lock().unwrap();
            client_queue.push(Arc::new(Mutex::new(
                GameClient::new(connection)
            )));
        }
    }
}