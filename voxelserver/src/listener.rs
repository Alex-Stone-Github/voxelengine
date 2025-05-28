use crate::{Game};
use crate::client::GameClient;

pub fn connection_listener(clients: std::sync::Arc<std::sync::Mutex<Vec<GameClient>>>) {
    let server = std::net::TcpListener::bind("127.0.0.1:8000")
            .expect("Port 8000 already in use");
    for potential_connection in server.incoming() {
        if let Ok(connection) = potential_connection {
            clients.lock().unwrap().push(GameClient::new(connection));
        }
    }
}