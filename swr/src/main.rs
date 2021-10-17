#![allow(dead_code)]

mod obj;

use obj::Obj;

fn main() {
    let obj = Obj::from_file("head.obj\0");
}
