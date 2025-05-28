use crate::core::*;

pub enum ServerSection {
    ServerSendFullChunk(IndexId, ChunkData),
    ServerSendBlockUpdate(CompressedBlockUpdate),
}
pub struct OutgoingPacket(pub Vec<ServerSection>);