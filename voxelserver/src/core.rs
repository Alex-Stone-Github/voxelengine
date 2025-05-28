

#[derive(Debug)]
pub struct PacketDecodeError;
#[derive(PartialEq, Debug)]
pub struct IndexId {
    pub x: i32,
    pub y: i32,
    pub z: i32,
}

#[derive(PartialEq, Debug)]
pub struct CompressedBlockUpdate {
    pub chunk: IndexId,
    pub block: IndexId,
    pub new: Block,
}
const CSIZE: usize = 32;
#[derive(PartialEq, Debug)]
pub enum Block {
    Air,
    Stone,
    Grass
}
#[derive(Debug)]
pub struct InvalidBlock;
impl Block {
    pub fn from_id(id: u32) -> Result<Self, InvalidBlock> {
        match id {
            0 => Ok(Self::Air),
            1 => Ok(Self::Stone),
            2 => Ok(Self::Grass),
            _ => Err(InvalidBlock),
        }
    }
    pub fn to_id(&self) -> u32 {
        match self {
            Self::Air => 0,
            Self::Stone => 1,
            Self::Grass => 2,
        }
    }
}
pub struct ChunkData {
    blocks: [[[Block; CSIZE]; CSIZE]; CSIZE],
}