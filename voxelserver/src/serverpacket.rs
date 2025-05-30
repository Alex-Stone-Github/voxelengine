use std::fmt::{Display, Pointer};

use crate::{byteutil, core::*};

#[derive(Clone, Debug)]
pub enum ServerSection {
    ServerSendFullChunk(IndexId, ChunkData),
    ServerSendBlockUpdate(CompressedBlockUpdate),
}
impl ServerSection {
    pub fn calc_net_size(&self) -> usize {
        match self {
            ServerSection::ServerSendFullChunk(..) => {
                std::mem::size_of::<u32>() +
                std::mem::size_of::<IndexId>() +
                std::mem::size_of::<ChunkData>()
            }
            ServerSection::ServerSendBlockUpdate(..) => {
                std::mem::size_of::<u32>() +
                std::mem::size_of::<IndexId>()*2+4
            }
        }
    }
    pub fn to_id(&self) -> u32 {
        match self {
            ServerSection::ServerSendFullChunk(..) => 0,
            ServerSection::ServerSendBlockUpdate(..) => 1
        }
    }
    pub fn to_net_bytes(&self) -> Box<[u8]> {
        let mut bytes = Vec::<u8>::new();
        let id = self.to_id();
        let id_bytes = byteutil::u32_to_bytes(id);
        bytes.extend_from_slice(&id_bytes);
        match self {
            ServerSection::ServerSendFullChunk(id, data) => {
                bytes.extend_from_slice(&byteutil::i32_to_bytes(id.x));
                bytes.extend_from_slice(&byteutil::i32_to_bytes(id.y));
                bytes.extend_from_slice(&byteutil::i32_to_bytes(id.z));
                for i in 0..CSIZE {
                    for j in 0..CSIZE {
                        for k in 0..CSIZE {
                            let block = data.blocks[i][j][k].to_id() as u8;
                            bytes.push(block);
                        }
                    }
                }
            },
            ServerSection::ServerSendBlockUpdate(cbu) => {
                bytes.extend_from_slice(&byteutil::i32_to_bytes(cbu.chunk.x));
                bytes.extend_from_slice(&byteutil::i32_to_bytes(cbu.chunk.y));
                bytes.extend_from_slice(&byteutil::i32_to_bytes(cbu.chunk.z));

                bytes.extend_from_slice(&byteutil::i32_to_bytes(cbu.block.x));
                bytes.extend_from_slice(&byteutil::i32_to_bytes(cbu.block.y));
                bytes.extend_from_slice(&byteutil::i32_to_bytes(cbu.block.z));

                bytes.extend_from_slice(&byteutil::u32_to_bytes(cbu.new.to_id()));
            }
        }
        bytes.into_boxed_slice()
    }
}


#[derive(Clone, Debug)]
pub struct OutgoingPacket(pub Vec<ServerSection>);
impl OutgoingPacket {
    pub fn calulate_net_size(&self) -> u32{
        let size: usize = self.0.iter().map(|s| s.calc_net_size()).sum();
        size as u32 + 4 // add length of the size header
    }
    pub fn into_buffer(&self) -> Box<[u8]> {
        let mut bytes = Vec::<u8>::new();
        let msg_len = self.calulate_net_size();
        bytes.extend_from_slice(&byteutil::u32_to_bytes(msg_len));
        for section in self.0.iter() {
            let next = section.to_net_bytes();
            bytes.extend_from_slice(next.iter().as_slice());
        }
        bytes.into_boxed_slice()
    }
}