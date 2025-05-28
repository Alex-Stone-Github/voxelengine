pub fn as_i32(four_bytes: &[u8]) -> i32 {
    let little_endian = [four_bytes[3], four_bytes[2], four_bytes[1], four_bytes[0]];
    let thing = unsafe {std::mem::transmute::<[u8; 4], i32>(little_endian)};
    return thing;
}
pub fn as_u32(four_bytes: &[u8]) -> u32 {
    let little_endian = [four_bytes[3], four_bytes[2], four_bytes[1], four_bytes[0]];
    let thing = unsafe {std::mem::transmute::<[u8; 4], u32>(little_endian)};
    return thing;
}
pub fn as_f32(four_bytes: &[u8]) -> f32 {
    let little_endian = [four_bytes[3], four_bytes[2], four_bytes[1], four_bytes[0]];
    let thing = unsafe {std::mem::transmute::<[u8; 4], f32>(little_endian)};
    return thing;
}
pub fn i32_to_bytes(src: i32) -> [u8; 4] {
    let little_endian = unsafe{std::mem::transmute::<i32, [u8; 4]>(src)};
    [little_endian[3], little_endian[2], little_endian[1], little_endian[0]]
}
pub fn u32_to_bytes(src: u32) -> [u8; 4] {
    let little_endian = unsafe{std::mem::transmute::<u32, [u8; 4]>(src)};
    [little_endian[3], little_endian[2], little_endian[1], little_endian[0]]
}
pub fn f32_to_bytes(src: f32) -> [u8; 4] {
    let little_endian = unsafe{std::mem::transmute::<f32, [u8; 4]>(src)};
    [little_endian[3], little_endian[2], little_endian[1], little_endian[0]]
}

pub fn take_bytes<'a, Queue: Iterator<Item = &'a u8>>(queue: &mut Queue, count: usize) -> Box<[u8]> {
    let mut collection = Vec::with_capacity(count);
    for _ in 0..count {
        let byte = queue.next().expect("Requires count bytes");
        collection.push(byte.clone());
    }
    return collection.into_boxed_slice();
}
