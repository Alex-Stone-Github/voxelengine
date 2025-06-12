use std::io::Read;
use std::io::Write;

use crate::byteutil;
use crate::clientpacket;
use crate::clientpacket::*;
use crate::gametypes::EntityInfo;
use crate::serverpacket::*;
use crate::core::*;
use crate::gametypes::Vector3;



pub struct GameClient {
    pub connection: std::net::TcpStream,
    pub incoming_bytes: Vec<u8>,
    pub outgoing: OutgoingPacket,
}
impl GameClient {
    pub fn new(stream: std::net::TcpStream) -> Self {
        return Self {
            connection: stream,
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
    pub fn step(&mut self, world: &mut std::collections::HashMap<IndexId,
        ChunkData>) {
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
            }
        }
        // Make updates to local information and send relevant
        for section in incoming_packet.0 {
            if let ClientSection::ClientGetChunkFull(chunkid) = section {
                // We are requesting the full chunk
                let exists = world.keys().any(|id| *id == chunkid);
                if !exists {continue;}
                let data = &world[&chunkid];
                let section = ServerSection::ServerSendFullChunk(chunkid, data.clone());
                self.outgoing.0.push(section);
            }
        }
        self.outgoing.0.push(
            ServerSection::ServerSendEntityInfo(EntityInfo{
                position: Vector3::new(1.0, 32.0, -3.0),
                yaw: 0.0,
            })
        ); // Add to the outgoing
        self.outgoing.0.push(
            ServerSection::ServerSendEntityInfo(EntityInfo{
                position: Vector3::new(69.0, 30.0, -69.0),
                yaw: std::f32::consts::FRAC_PI_2,
            })
        ); // Add to the outgoing

        // Send appropriate information out
        println!("Send Packet with {} sections and {} length", self.outgoing.0.len(), self.outgoing.calulate_net_size());
        let buffer = self.outgoing.into_buffer();
        self.outgoing.0.clear();
        self.connection.write_all(buffer.iter().as_slice())
            .expect("Failed to write to client");
    }
}