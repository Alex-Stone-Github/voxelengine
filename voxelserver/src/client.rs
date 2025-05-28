use std::io::{BufRead, Read};
use std::process::exit;

use crate::byteutil;
use crate::clientpacket;
use crate::clientpacket::*;
use crate::serverpacket::*;
use crate::core::*;



pub struct GameClient {
    pub connection: std::net::TcpStream,
    pub pending_updates: std::sync::Arc<std::sync::Mutex<Vec<ServerSection>>>,
    pub incoming_bytes: Vec<u8>,
    pub outgoing: OutgoingPacket,
}
impl GameClient {
    pub fn new(stream: std::net::TcpStream) -> Self {
        return Self {
            connection: stream,
            pending_updates: std::sync::Arc::new(std::sync::Mutex::new(Vec::new())),
            incoming_bytes: Vec::new(),
            outgoing: OutgoingPacket(Vec::new()),
        }
    }
    pub fn packet_len_hint(&self) -> Option<usize> {
        if self.incoming_bytes.len() < 4 {return None};
        let len_bytes: [u8; 4] = [
            self.incoming_bytes[0], self.incoming_bytes[1],
            self.incoming_bytes[2], self.incoming_bytes[3]
        ];
        Some(byteutil::as_u32(len_bytes.as_slice()) as usize)
    }
    pub fn step(&mut self) {
        // Read More bytes
        let mut buffer: [u8; 1024*8] = [0; 1024*8];
        if let Ok(len) = self.connection.read(buffer.as_mut_slice()) {
            let new_bytes = &buffer[0..len];
            self.incoming_bytes.extend_from_slice(
                new_bytes
            );
        }
        // Attempt to construct an incoming packet
        let mut packet = IncomingPacket(vec![]);
        if let Some(packet_len) = self.packet_len_hint() {
            if packet_len <= self.incoming_bytes.len() {
                let bytes = &self.incoming_bytes[0..packet_len];
                packet = clientpacket::IncomingPacket::from_buffer(bytes)
                    .expect("Invalid Packet");
                dbg!(packet);
                self.incoming_bytes.drain(0..packet_len);
            }
        }
        // Make updates to local information
        // Send appropriate information out
    }
}