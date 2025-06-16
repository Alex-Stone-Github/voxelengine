use std::io::Read;
use std::io::Write;

use crate::byteutil;
use crate::clientpacket;
use crate::clientpacket::*;
use crate::gametypes::EntityInfo;
use crate::serverpacket::*;
use crate::core::*;
use crate::gametypes::Vector3;

static mut i: usize = 0;

pub struct GameClient {
    pub connection: std::net::TcpStream,
    pub incoming_bytes: Vec<u8>,
    pub outgoing: OutgoingPacket,
    pub unique: usize,
}
impl GameClient {
    pub fn new(stream: std::net::TcpStream) -> Self {
        // SAFETY: Safe because we only ever have one thread create clients at a time(listener)
        unsafe{i += 1;}
        Self {
            connection: stream,
            incoming_bytes: Vec::new(),
            outgoing: OutgoingPacket(Vec::new()),
            // Safe because we only ever have one thread create clients at a time(listener)
            unique: unsafe{i}
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
    pub fn step(&mut self, world: crate::SharedResource<crate::World>) {
        // Read More bytes
        let mut buffer: [u8; 1024*8] = [0; 1024*8];
        // TODO / BUG: This call blocks and will wait until we have more bytes / Crazy
        if let Ok(len) = self.connection.read(buffer.as_mut_slice()) {
            let new_bytes = &buffer[0..len];
            self.incoming_bytes.extend_from_slice(
                new_bytes
            );
        }
        // Attempt to construct an incoming packet
        let mut incoming_packet = IncomingPacket(vec![]);
        if let Some(packet_len) = self.packet_len_hint() {
            if packet_len <= self.incoming_bytes.len() {
                let bytes = &self.incoming_bytes[0..packet_len];
                incoming_packet = clientpacket::IncomingPacket::from_buffer(bytes)
                    .expect("Invalid Packet");
                self.incoming_bytes.drain(0..packet_len);
                println!("Received Packet with {} sections", incoming_packet.0.len());
            }
        }
        // Make updates to local information and send relevant
        for section in incoming_packet.0 {
            match section  {
                ClientSection::ClientGetChunkFull(chunkid) => {
                    // Return the full chunk data if it exists
                    let chunks = &world.lock().unwrap().chunks;
                    let exists = chunks.keys().any(|id| *id == chunkid);
                    if !exists {continue;}
                    let data = &chunks[&chunkid];
                    let section = ServerSection::ServerSendFullChunk(chunkid, data.clone());
                    self.outgoing.0.push(section);
                }
                ClientSection::ClientSendPlayerPosUpdate(transform) => {
                    let _ = world.lock().unwrap().entities.insert(self.unique, transform);
                }
                _ => unimplemented!()
            };
        }

        // Send appropriate information out
        println!("Sent Packet with {} sections and {} length", self.outgoing.0.len(), self.outgoing.calulate_net_size());
        let buffer = self.outgoing.into_buffer();
        self.outgoing.0.clear();
        self.connection.write_all(buffer.iter().as_slice())
            .expect("Failed to write to client");
    }
}