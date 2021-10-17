use libc::*;
use std::ffi::CStr;
use std::ptr::null_mut;
use std::mem::MaybeUninit;
use std::ops::Drop;

macro_rules! cstr {
    ($s:expr) => ({
        use ::std::ffi::CStr;
        let s = concat!($s, "\0");
        let b = s.as_bytes();
        unsafe {
            CStr::from_bytes_with_nul_unchecked(b)
        }
    })
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

struct MmapFile {
    data: &'static [u8],
}

impl Drop for MmapFile {
    fn drop(&mut self) {
        let addr = self.data.as_ptr() as *mut c_void;
        let size = self.data.len();
        unsafe {
            munmap(addr, size);
        }
    }
}

fn mmap_read_only(path: &CStr) -> MmapFile {
    use std::slice;
    use std::str;
    unsafe {
        let fd = open(path.as_ptr(), O_RDONLY | O_CLOEXEC);
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
        let data = slice::from_raw_parts(addr as *const u8, size);
        MmapFile { data }
    }
}

#[derive(Debug, Copy, Clone, PartialEq)]
enum Token {
    Fn,
    Let,
    Ret,
    Lparen,
    Rparen,
    Lbrace,
    Rbrace,
    Arrow,
    Ellipsis,
    Colon,
    Comma,
    Star,
    Eq,
    Ident,
    Int,
    String,
    Eof,
}

struct Parser<'a> {
    text: &'a [u8],
    start: usize,
    end: usize,
    token: Token,
}

impl<'a> Parser<'a> {
    fn parse_token_(&mut self, a: char, b: char, c: char) {
        let t = match a as char {
            '(' => Token::Lparen,
            ')' => Token::Rparen,
            '{' => Token::Lbrace,
            '}' => Token::Rbrace,
            '-' => {
            }
            'a'..='z' | 'A'..=b'Z' | '0'..='9' => Token::Ident,
            _ => panic!("unexpected character {:?}", b as char),
        };
    }

    fn parse_token(&mut self) {
        self.start = self.end;
        let b = self.text[self.end];

        match t {
            Token::Ident => self.parse_ident(),
            _ => unimplemented!(),
        }
        self.token = t;
    }

    fn parse_ident(&mut self) {
        let s = &self.text[self.end + 1..];
        let n = s.len();
        let mut i = 1;
        while i < n {
            match s[i] as char {
                'a'..=b'z' | 'A'..=b'Z' | '0'..='9' => (),
                _ => break,
            }
            i += 1;
        }
        self.end += i;
    }
}

fn parse(text: &[u8]) {
    let mut p = Parser {
        text: text,
        start: 0,
        end: 0,
        token: Token::Eof,
    };
    loop {
        p.parse_token();
        if p.token == Token::Eof {
            break;
        }
    }
}

fn main() {
    let path = cstr!("test.pd");
    let file = mmap_read_only(path);
    parse(file.data);
}
