pub struct Image {
    pub pixels: Vec<u8>,
    pub width: u32,
    pub height: u32,
}

impl Image {
    pub fn new(width: u32, height: u32) -> Self {
        Image {
            pixels: vec![0; (width * height) as usize],
            width: width,
            height: height,
        }
    }

    pub fn write_file(&self, path: &str) {
        let mut header = [0u8; 18];
        header[2] = 2;
        header[12] = (self.width & 0xFF) as u8;
    }
}
