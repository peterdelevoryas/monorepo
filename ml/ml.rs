use std::fs::File;
use std::io::{BufReader, BufWriter, Read, Write};

fn read_big_endian_u32(f: &mut BufReader<File>) -> u32 {
    let mut buf = [0u8; 4];
    f.read_exact(&mut buf).unwrap();
    u32::from_be_bytes(buf)
}

fn tga_uncompressed_grayscale(image: &[u8], width: u16, height: u16, path: &str) {
    let width = width.to_le_bytes();
    let height = height.to_le_bytes();
    let header: [u8; 18] = [
        0,          // No ID included.
        0,          // No color map included.
        3,          // Uncompressed grayscale image.
        0,          // First color map index (none), lower 8 bits.
        0,          // First color map index (none), upper 8 bits.
        0,          // Color map length (zero), lower 8 bits.
        0,          // Color map length (zero), upper 8 bits.
        0,          // Color map entry size (zero).
        0,          // X origin, lower 8 bits.
        0,          // X origin, upper 8 bits.
        0,          // Y origin, lower 8 bits.
        0,          // Y origin, upper 8 bits.
        width[0],   // Width, lower 8 bits.
        width[1],   // Width, upper 8 bits.
        height[0],  // Height, lower 8 bits.
        height[1],  // Height, upper 8 bits.
        8,          // Pixel depth (bits per pixel).
        0x20,       // Alpha-channel depth and image orientation.
    ];

    let mut f = File::create(path).unwrap();
    f.write_all(&header).unwrap();
    f.write_all(image).unwrap();
}

fn mnist_handwritten_digits() {
    let f = File::open("train-labels-idx1-ubyte").unwrap();
    let mut f = BufReader::new(f);
    let magic = read_big_endian_u32(&mut f);
    let n_labels = read_big_endian_u32(&mut f);
    assert_eq!(magic, 0x0000_0801);
    assert_eq!(n_labels, 60_000);
    let mut labels = vec![0u8; 60_000];
    f.read_exact(&mut labels).unwrap();

    let f = File::open("train-images-idx3-ubyte").unwrap();
    let mut f = BufReader::new(f);
    let magic = read_big_endian_u32(&mut f);
    let n_images = read_big_endian_u32(&mut f);
    let height = read_big_endian_u32(&mut f);
    let width = read_big_endian_u32(&mut f);
    assert_eq!(magic, 0x0000_0803);
    assert_eq!(n_images, 60_000);
    assert_eq!(height, 28);
    assert_eq!(width, 28);
    let mut images = vec![0u8; 60_000 * 28 * 28];
    f.read_exact(&mut images).unwrap();

    let mut images2 = vec![0u8; 60_000 * 28 * 28];
    for i in 0..200 {
        for j in 0..28 {
            for k in 0..300 {
                for l in 0..28 {
                    images2[i * 28 * 300 * 28 + j * 300 * 28 + k * 28 + l] =
                        images[(i * 300 + k) * 28 * 28 + j * 28 + l];
                }
            }
        }
    }
    tga_uncompressed_grayscale(&images2, 300 * 28, 200 * 28, "collage.tga");
    //tga_uncompressed_grayscale(&images, 200 * 28, 300 * 28, "images.tga");
    //tga_uncompressed_grayscale(&images[..10 * 28 * 28], 28, 10 * 28, "images.tga");
}

fn generate_data(m: usize) -> Vec<f32> {
    let mut f = File::open("/dev/urandom").unwrap();
    let mut e = vec![0u8; m];
    f.read_exact(&mut e).unwrap();
    drop(f);

    let mut data = vec![0.0; m * 3];
    let (x, y) = data.split_at_mut(m * 2);
    for i in 0..m {
        let m = m as f32;
        let e = (e[i] as f32 - 128.0) / 32.0;
        let x0 = 1.0;
        let x1 = -m / 2.0 + i as f32;
        x[i * 2 + 0] = x0;
        x[i * 2 + 1] = x1;
        y[i] = x1 + e;
    }

    return data;
}

fn dot_product(a: &[f32], b: &[f32]) -> f32 {
    assert_eq!(a.len(), b.len());
    let n = a.len();
    let mut product = 0.0;
    for i in 0..n {
        product += a[i] * b[i];
    }
    return product;
}

fn batch_gradient_descent(x: &[f32], y: &[f32], w: &mut [f32]) {
    let a = 0.01;
    let m = x.len();
    let n = w.len();
    assert_eq!(x.len(), m * n);
    assert_eq!(x.len() / n, y.len());

    let mut w_ = [0.0; 16];
    let mut w_ = &mut w_[..n];

    for k in 0..10_000 {
        for i in 0..n {
            let mut g = 0.0;
            for j in 0..m {
                g += dot_product(w, &x[j * n..j * n + n]);
            }
            g /= m;
            w_[i] = w[i] - a * g;
        }

    }
}

fn linear_regression() {
    let m = 30;
    let mut data = generate_data(m);
    let (x, y) = data.split_at_mut(2 * m);
    println!("{:?} {:?}", x, y);

    let f = File::create("in.csv").unwrap();
    let mut f = BufWriter::new(f);
    writeln!(f, "x,y").unwrap();
    for i in 0..m {
        writeln!(f, "{},{}", x[i * 2 + 1], y[i]).unwrap();
    }

    let mut w = [0.0; 2];
    batch_gradient_descent(x, y, &mut w);

    println!("w {:?}", w);

    let f = File::create("out.csv").unwrap();
    let mut f = BufWriter::new(f);
    writeln!(f, "x,y^").unwrap();
}

fn main() {
    linear_regression();
    mnist_handwritten_digits();
}
