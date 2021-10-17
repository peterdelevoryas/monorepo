use libc::*;
use std::ffi::CStr;
use std::ptr::null_mut;
use std::mem::MaybeUninit;
use std::ops::Drop;

#[derive(Debug, Default)]
pub struct Obj {
    vertices: Vec<[f32; 3]>,
    triangles: Vec<[u16; 3]>,
}

struct MmapFile {
    addr: *mut c_void,
    size: size_t,
}

impl Drop for MmapFile {
    fn drop(&mut self) {
        unsafe {
            munmap(self.addr, self.size);
        }
    }
}

struct Vector<T> {
    data: *mut T,
    len: u32,
    cap: u32,
}

impl<T> Vector<T> {
    const fn new() -> Self {
        Vector {
            data: null_mut(),
            len: 0,
            cap: 0,
        }
    }

    fn append(&mut self, x: T) {
        use std::mem::size_of;
        unsafe {
            if self.len >= self.cap {
                self.cap = match self.cap {
                    0 => 8,
                    n => n * 3 / 2,
                };
                let new_size = self.cap as usize * size_of::<T>();
                let old_ptr = self.data as *mut c_void;
                let new_ptr = realloc(old_ptr, new_size);
                self.data = new_ptr as *mut T;
            }
            self.data.offset(self.len as isize).write(x);
        }
    }
}

macro_rules! libc_panic {
    ($fmt:expr, $($e:expr),*) => ({
        use ::std::ffi::CStr;
        use ::libc::*;
        let err = *__error();
        let err = strerror(err);
        let err = CStr::from_ptr(err);
        let err = err.to_string_lossy();
        panic!(concat!($fmt, ": {}"), $($e),*, err);
    })
}

fn mmap_read_only(path: &str) -> MmapFile {
    debug_assert_eq!(path.chars().last().unwrap(), '\0');
    unsafe {
        let path_ = path.as_ptr() as *const _;
        let fd = open(path_, O_RDONLY | O_CLOEXEC);
        if fd == -1 {
            libc_panic!("unable to open {:?}", path);
        }
        let mut status = MaybeUninit::uninit();
        if fstat(fd, status.as_mut_ptr()) != 0 {
            libc_panic!("unable to fstat {:?}", path);
        }
        let status = status.assume_init();
        let size = status.st_size as size_t;
        let addr = mmap(null_mut(), size, PROT_READ, MAP_SHARED, fd, 0);
        if addr == MAP_FAILED {
            libc_panic!("unable to mmap {:?}, size {:?}", path, size);
        }
        close(fd);
        MmapFile { addr, size }
    }
}

impl Obj {
    pub fn from_file(path: &str) -> Obj {
        let file = mmap_read_only(path);
        unimplemented!()
    }
}
