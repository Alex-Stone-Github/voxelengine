use crate::core::*;
use crate::byteutil;
use crate::gametypes::*;

#[derive(PartialEq, Debug)]
pub enum ClientSection {
    ClientGetChunkUpdate(IndexId),
    ClientGetChunkFull(IndexId),
    ClientSendChunkUpdate(CompressedBlockUpdate),
    ClientSendPlayerPosUpdate(EntityInfo),
}
impl ClientSection {
    fn get_id(&self) -> u32 {
        match self {
            ClientSection::ClientGetChunkUpdate(..) => 0,
            ClientSection::ClientGetChunkFull(..) => 1,
            ClientSection::ClientSendChunkUpdate(..) => 2,
            ClientSection::ClientSendPlayerPosUpdate(..) => 3,
        }
    }
}
#[derive(PartialEq, Debug)]
pub struct IncomingPacket(pub Vec<ClientSection>);
impl IncomingPacket {
    pub fn from_buffer(buffer: &[u8]) -> Result<Self, PacketDecodeError> {
        let mut bytes = buffer.iter();
        let _len_bytes = byteutil::take_bytes(&mut bytes, 4);
        let mut sections = Vec::<ClientSection>::new();
        while bytes.len() > 0 {
            let kind_bytes = byteutil::take_bytes(&mut bytes, 4);
            let kind = byteutil::as_u32(&kind_bytes);
            match kind {
                0 => { // Get Chunk Update
                    let idx_x_bytes = byteutil::take_bytes(&mut bytes, 4);
                    let idx_x = byteutil::as_i32(&idx_x_bytes);
                    let idx_y_bytes = byteutil::take_bytes(&mut bytes, 4);
                    let idx_y = byteutil::as_i32(&idx_y_bytes);
                    let idx_z_bytes = byteutil::take_bytes(&mut bytes, 4);
                    let idx_z = byteutil::as_i32(&idx_z_bytes);
                    let chunk_id = IndexId {x: idx_x, y: idx_y, z: idx_z};
                    sections.push(ClientSection::ClientGetChunkUpdate(chunk_id));
                },
                1 => { // Get Chunk Full
                    let idx_x_bytes = byteutil::take_bytes(&mut bytes, 4);
                    let idx_x = byteutil::as_i32(&idx_x_bytes);
                    let idx_y_bytes = byteutil::take_bytes(&mut bytes, 4);
                    let idx_y = byteutil::as_i32(&idx_y_bytes);
                    let idx_z_bytes = byteutil::take_bytes(&mut bytes, 4);
                    let idx_z = byteutil::as_i32(&idx_z_bytes);
                    let chunk_id = IndexId {x: idx_x, y: idx_y, z: idx_z};
                    sections.push(ClientSection::ClientGetChunkFull(chunk_id));
                },
                2 => { // Send Chunk Update
                    let idx_x_bytes = byteutil::take_bytes(&mut bytes, 4);
                    let idx_x = byteutil::as_i32(&idx_x_bytes);
                    let idx_y_bytes = byteutil::take_bytes(&mut bytes, 4);
                    let idx_y = byteutil::as_i32(&idx_y_bytes);
                    let idx_z_bytes = byteutil::take_bytes(&mut bytes, 4);
                    let idx_z = byteutil::as_i32(&idx_z_bytes);
                    let chunk_id = IndexId {x: idx_x, y: idx_y, z: idx_z};

                    let blk_x_bytes = byteutil::take_bytes(&mut bytes, 4);
                    let blk_x = byteutil::as_i32(&blk_x_bytes);
                    let blk_y_bytes = byteutil::take_bytes(&mut bytes, 4);
                    let blk_y = byteutil::as_i32(&blk_y_bytes);
                    let blk_z_bytes = byteutil::take_bytes(&mut bytes, 4);
                    let blk_z = byteutil::as_i32(&blk_z_bytes);
                    let blk_id = IndexId {x: blk_x, y: blk_y, z: blk_z};

                    let blk_bytes = byteutil::take_bytes(&mut bytes, 4);
                    let blk = byteutil::as_u32(&blk_bytes);
                    let attempt_blk = Block::from_id(blk);
                    if let Ok(valid_blk) = attempt_blk {
                        sections.push(ClientSection::ClientSendChunkUpdate(
                            CompressedBlockUpdate {
                                chunk: chunk_id,
                                block: blk_id,
                                new: valid_blk
                            }));
                    } else {return  Err(PacketDecodeError);}
                },
                3 => { // Send Position Update
                    let x_bytes = byteutil::take_bytes(&mut bytes, 4);
                    let x = byteutil::as_f32(&x_bytes);
                    let y_bytes = byteutil::take_bytes(&mut bytes, 4);
                    let y = byteutil::as_f32(&y_bytes);
                    let z_bytes = byteutil::take_bytes(&mut bytes, 4);
                    let z = byteutil::as_f32(&z_bytes);
                    let yaw_bytes = byteutil::take_bytes(&mut bytes, 4);
                    let yaw = byteutil::as_f32(&yaw_bytes);
                    let position = Vector3{x, y, z};
                    let ent_info = EntityInfo{position, yaw};
                    sections.push(ClientSection::ClientSendPlayerPosUpdate(ent_info));
                }
                _ => return Err(PacketDecodeError)
            };
        }
        Ok(IncomingPacket(sections))
    }
}

#[test]
fn test_packet_decode_basic() {
    let mut buffer = Vec::<u8>::new();
    buffer.extend(byteutil::u32_to_bytes(4+4+4*3)); // Packet Length
    buffer.extend(byteutil::u32_to_bytes(0)); // Section id(GetChunkUpdate)

    buffer.extend(byteutil::i32_to_bytes(1)); // Section id(GetChunkUpdate)
    buffer.extend(byteutil::i32_to_bytes(2)); // Section id(GetChunkUpdate)
    buffer.extend(byteutil::i32_to_bytes(3)); // Section id(GetChunkUpdate)

    let ideal = IncomingPacket(vec![
        ClientSection::ClientGetChunkUpdate(IndexId { x: 1, y: 2, z: 3 })
        ]);
    let decoding = IncomingPacket::from_buffer(buffer.as_slice());
    let success = decoding.expect("Failed decoding");
    assert_eq!(ideal, success);
}
#[test]
fn test_packet_decode_large() {
    let mut buffer = Vec::<u8>::new();
    buffer.extend(byteutil::u32_to_bytes(4+4+4*3+4*3+4)); // Packet Length
    buffer.extend(byteutil::u32_to_bytes(2)); // Section id(GetChunkUpdate)

    buffer.extend(byteutil::i32_to_bytes(1)); // Section id(GetChunkUpdate)
    buffer.extend(byteutil::i32_to_bytes(2)); // Section id(GetChunkUpdate)
    buffer.extend(byteutil::i32_to_bytes(3)); // Section id(GetChunkUpdate)

    buffer.extend(byteutil::i32_to_bytes(4)); // Block id(GetChunkUpdate)
    buffer.extend(byteutil::i32_to_bytes(5)); // Block id(GetChunkUpdate)
    buffer.extend(byteutil::i32_to_bytes(6)); // Block id(GetChunkUpdate)

    buffer.extend(byteutil::u32_to_bytes(1)); // Stone

    let ideal = IncomingPacket(vec![
        ClientSection::ClientSendChunkUpdate(
            CompressedBlockUpdate {
                chunk: IndexId { x: 1, y: 2, z: 3 },
                block: IndexId { x: 4, y: 5, z: 6 },
                new: Block::from_id(1).unwrap(),
            },
        )
    ]);
    let decoding = IncomingPacket::from_buffer(buffer.as_slice());
    let success = decoding.expect("Failed decoding");
    assert_eq!(ideal, success);
}