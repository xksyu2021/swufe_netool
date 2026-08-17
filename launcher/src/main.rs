use std::ffi::OsStr;
use std::iter::once;
use std::os::windows::ffi::OsStrExt;
use std::ptr::{null, null_mut};

use windows_sys::Win32::{
    Foundation, Security,
    System::{
        Pipes, Threading
    }
};

fn wide_null(value: &str) -> Vec<u16> {
    OsStr::new(value)
        .encode_wide()
        .chain(once(0))
        .collect()
}

fn main(){
    let pipe = unsafe{
        Pipes::CreateNamedPipeW(
            wide_null(r"\\.\pipe\swufe_netool").as_ptr(),
            Pipes::PIPE_ACCESS_DUPLEX,
            
        )
    };
}