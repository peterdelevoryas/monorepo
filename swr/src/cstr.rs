
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


