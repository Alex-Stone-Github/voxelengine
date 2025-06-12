
#[derive(PartialEq, Debug, Clone)]
pub struct Vector3 {
    pub x: f32,
    pub y: f32,
    pub z: f32,
}
impl Vector3 {
    pub fn new(x: f32, y: f32, z: f32) -> Self {
        Self {x, y, z}
    }
}

#[derive(PartialEq, Debug, Clone)]
pub struct EntityInfo {
    pub position: Vector3,
    pub yaw: f32,
}