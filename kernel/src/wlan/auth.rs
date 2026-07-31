mod info{
    use serde::{Deserialize, Serialize};

    pub static CL_ADDRESS: &str = "cgi-bin/get_challenge";
    pub static AUTH_ADDRESS: &str = "cgi-bin/srun_portal";
    pub static IP_TEST: &str = "10.9.0.4";

    #[derive(Serialize)]
    pub struct ClParam{
        pub username: String,
        pub ip: String,
        pub double_stack: String,
        pub timestamp: String
    }

    #[derive(Deserialize)]
    pub struct ClResponse{
        pub challenge: Option<String>
    }

    #[derive(Serialize)]
    pub struct InfoParam {
        pub username: String,
        pub password: String,
        pub ip: String,
        pub ac_id: String,
        pub enc_ver: String,
    }

    #[derive(Serialize)]
    pub struct PostParam {
        pub action: String,
        pub username: String,
        pub password: String,
        pub ac_id: String,
        pub ip: String,
        pub double_stack: String,
        pub n: String,
        #[serde(rename = "type")]
        pub r#type: String,
        pub os: String,
        pub name: String,
        pub info: String,
        pub chksum: String,
    }
}

pub mod core{
    use super::info;
    use super::sec;
    use crate::wlan::share::frame::{Isp, User};
    use std::net::UdpSocket;
    use std::time::{SystemTime, SystemTimeError, UNIX_EPOCH};
    use reqwest;
    use secrecy::ExposeSecret;

    fn timestamp() -> Result<u128,SystemTimeError> {
        Ok(SystemTime::now()
            .duration_since(UNIX_EPOCH)?
            .as_millis())
    }

    fn get_ip() -> Option<String> {
        let socket = UdpSocket::bind("0.0.0.0:0").ok()?;
        socket.connect(format!("http://{}:80", info::IP_TEST)).ok()?;
        socket.local_addr().ok().map(|addr| addr.ip().to_string())
    }

    pub async fn act(isp: &Isp, user: &User) -> Result<bool, reqwest::Error> {
        let client = reqwest::Client::new();

        let full_acc = format!("{}@{}",user.account,isp.domain);

        let ip = if let Some(ip) = get_ip() {
            ip
        } else {
            eprintln!("#201");
            eprintln!("FAILED(Usr): Cannot get ipv4!");
            return Ok(false)
        };

        let time = if let Ok(time) = timestamp() {
            time
        } else {
            eprintln!("#200");
            eprintln!("FAILED(Sys): Wrong system time!");
            return Ok(false)
        };

        let cl_param = super::info::ClParam{
            username: full_acc.clone(),
            ip: ip.to_string(),
            double_stack: "0".to_string(),
            timestamp: time.to_string()
        };
        let cl_url = format!("http://{}:{}/{}", isp.auth_add, isp.auth_port, info::CL_ADDRESS);
        let cl_response = client.get(&cl_url).query(&cl_param).send().await?;

        if cl_response.status().is_success() {
            let rp_map: info::ClResponse = cl_response.json().await?;
            let challenge = if let Some(cl) = rp_map.challenge {
                cl
            } else {
                eprintln!("#203");
                eprintln!("FAILED(Sys): Connot get challenge code!");
                return Ok(false)
            };

            let info_param = info::InfoParam{
                username: full_acc.clone(),
                password: user.password.expose_secret().parse().unwrap(),
                ip: ip.clone(),
                ac_id: "1".to_string(),
                enc_ver: "srun_bx1".to_string(),
            };

            let password = sec::pw_md5(user.password.expose_secret(), &*challenge);
            let info = sec::info_srbx(&serde_json::to_string(&info_param).unwrap(), &challenge);
            let chksum = sec::chksum_sh1(&challenge, &full_acc, &password, "1", &ip, "200", "1", &info);

            let post_param = info::PostParam {
                action: "login".to_string(),
                username: full_acc.clone(),
                password,
                ac_id: "1".to_string(),
                ip,
                double_stack: "0".to_string(),
                n: "200".to_string(),
                r#type: "1".to_string(),
                os: "Windows 11".to_string(),
                name: "Windows".to_string(),
                info,
                chksum
            };

            let post_url = format!("http://{}:{}/{}", isp.auth_add, isp.auth_port, info::AUTH_ADDRESS);
            let post_response = client.post(&post_url).form(&post_param).send().await?;

            if post_response.status().is_success() {
                println!("#901");
                println!("OK: Auth succeed.");
                return Ok(true)
            }

            eprintln!("#204");
            eprintln!("FAILED(Usr|Self): Auth not pass!");
            return Ok(false)
        }

        eprintln!("#202");
        eprintln!("FAILED(Usr|Self): Cannot connect challenge server!");
        Ok(false)
    }
}

mod sec{
    use hmac::{Hmac, KeyInit, Mac};
    use md5::Md5;
    use sha1::{Digest, Sha1};

    pub fn pw_md5(password: &str, token: &str) -> String {
        let mut mac = Hmac::<Md5>::new_from_slice(token.as_bytes())
            .expect("MD5 accepts any key length");
        mac.update(password.as_bytes());
        mac.finalize()
            .into_bytes()
            .iter()
            .map(|b| format!("{:02x}", b))
            .collect()
    }

    pub fn info_srbx(info_json: &str, token: &str) -> String {
        let pack_u32 = |a: &str, append_len: bool| -> Vec<u32> {
            let bytes = a.as_bytes();
            let mut v: Vec<u32> = bytes
                .chunks(4)
                .map(|chunk| {
                    (0..chunk.len()).fold(0u32, |val, i| val | ((chunk[i] as u32) << (i * 8)))
                })
                .collect();
            if append_len {
                v.push(bytes.len() as u32);
            }
            v
        };

        if info_json.is_empty() {
            return String::new();
        }

        let mut v = pack_u32(info_json, true);
        let mut k = pack_u32(token, false);
        if k.len() < 4 {
            k.resize(4, 0);
        }

        let n = v.len() - 1;
        let mut z = v[n];
        let mut y;
        const DELTA: u32 = 0x9E3779B9;
        let q = 6 + 52 / (n + 1);
        let mut d: u32 = 0;

        for _ in 0..q {
            d = d.wrapping_add(DELTA);
            let e = ((d >> 2) & 3) as usize;

            for p in 0..n {
                y = v[p + 1];
                let mut m = (z >> 5) ^ (y << 2);
                m = m.wrapping_add((y >> 3) ^ (z << 4) ^ (d ^ y));
                m = m.wrapping_add(k[(p & 3) ^ e] ^ z);
                v[p] = v[p].wrapping_add(m);
                z = v[p];
            }

            y = v[0];
            let p = n;
            let mut m = (z >> 5) ^ (y << 2);
            m = m.wrapping_add((y >> 3) ^ (z << 4) ^ (d ^ y));
            m = m.wrapping_add(k[(p & 3) ^ e] ^ z);
            v[n] = v[n].wrapping_add(m);
            z = v[n];
        }

        let mut cipher_bytes = Vec::with_capacity(v.len() * 4);
        for &val in &v {
            cipher_bytes.extend_from_slice(&[
                (val & 0xff) as u8,
                ((val >> 8) & 0xff) as u8,
                ((val >> 16) & 0xff) as u8,
                ((val >> 24) & 0xff) as u8,
            ]);
        }

        const ALPHA: &[u8; 64] =
            b"LVoJPiCN2R8G90yg+hmFHuacZ1OWMnrsSTXkYpUq/3dlbfKwv6xztjI7DeBE45QA";

        let mut b64 = String::with_capacity((cipher_bytes.len() + 2) / 3 * 4);
        for chunk in cipher_bytes.chunks(3) {
            let b0 = chunk[0] as u32;
            let b1 = chunk.get(1).copied().unwrap_or(0) as u32;
            let b2 = chunk.get(2).copied().unwrap_or(0) as u32;
            let triple = (b0 << 16) | (b1 << 8) | b2;
            b64.push(ALPHA[((triple >> 18) & 0x3f) as usize] as char);
            b64.push(ALPHA[((triple >> 12) & 0x3f) as usize] as char);
            b64.push(if chunk.len() > 1 {
                ALPHA[((triple >> 6) & 0x3f) as usize] as char
            } else {
                '='
            });
            b64.push(if chunk.len() > 2 {
                ALPHA[(triple & 0x3f) as usize] as char
            } else {
                '='
            });
        }

        format!("{{SRBX1}}{b64}")
    }

    pub fn chksum_sh1(
        token: &str,
        username: &str,
        hmd5: &str,
        ac_id: &str,
        ip: &str,
        n: &str,
        type_: &str,
        info: &str,
    ) -> String {
        let input = [
            token, username, token, hmd5, token, ac_id, token, ip, token, n, token, type_, token, info,
        ]
            .concat();

        let mut hasher = Sha1::new();
        hasher.update(input.as_bytes());
        hasher.finalize().iter().map(|b| format!("{:02x}", b)).collect()
    }
}