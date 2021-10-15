#[macro_use]
mod cstr;
mod obj;

use crate::obj::Obj;

fn main() {
    let path = cstr!("head.obj");
    let obj = Obj::from_file(path);
}
