
struct GameClient<'a> {
    incoming: std::io::BufReader<&'a std::net::TcpStream>,
    outgoing: std::net::TcpStream
}

fn main() {
    let server = std::net::TcpListener::bind("127.0.0.1:8000")
        .expect("Port 8000 already in use");

    use std::io::{Write, Read};
    for potential_connection in server.incoming() {
        if let Ok(mut connection) = potential_connection {
            // Read
            let mut reader = std::io::BufReader::new(&connection);
            let mut buffer: [u8; 512] = [0; 512];
            reader.read(&mut buffer).unwrap();

            // Write
            let message = "hi there";
            connection.write(message.as_bytes()).unwrap();
        }
    }
}
