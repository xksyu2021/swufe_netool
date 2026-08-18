pub mod core{
    use crate::wlan::share::frame::{Isp, AuthType, User};
    use windows::{
        Devices::WiFi::WiFiAdapter,
        core::Result
    };
    use windows::Devices::WiFi::{WiFiConnectionStatus, WiFiReconnectionKind};

    pub async fn connect(isp: &Isp, user: &User) -> Result<bool>{
        match isp.auth_type {
            AuthType::OpenAndWeb => conn_op_web(isp,user).await,
            AuthType::Edu => conn_edu(isp,user).await,
            _ => {
                eprintln!("*100 No connection type matched.");
                Ok(false)
            }
        }
    }

    async fn conn_op_web(isp: &Isp, user: &User) -> Result<bool>{
        let adapters = WiFiAdapter::FindAllAdaptersAsync()?.await?;
        if adapters.Size() == Ok(0) {
            eprintln!("*101 No WLAN adapter.");
            return Ok(false)
        }

        let adapter = adapters.GetAt(0)?;
        adapter.ScanAsync()?.await?;

        let networks = adapter.NetworkReport()?.AvailableNetworks()?;
        for network in networks{
            if network.Ssid()? == isp.ssid {
                let response =
                    adapter.ConnectAsync(&network,WiFiReconnectionKind::Automatic)?.await?;

                let res = response.ConnectionStatus()?;
                if res == WiFiConnectionStatus::Success {
                    return match super::super::auth::core::act(isp, user).await {
                        Ok(_) => Ok(true),
                        Err(e) => {
                            eprintln!("*100 {e}");
                            Ok(false)
                        }
                    }
                }
                eprintln!("*100 {:?}",res);
                return Ok(false);
            }
        }
        eprintln!("#202 No target SSID.");
        Ok(false)
    }

    async fn conn_edu(isp: &Isp, user: &User) -> Result<bool>{
        eprintln!("*100 Under developing.");
        Ok(false)
    }
}