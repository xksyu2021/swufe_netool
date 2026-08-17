use tokio::io;
use tokio::net::windows::named_pipe::ServerOptions;
use std::process::Stdio;
use tokio::process::Command;

#[tokio::main]
async fn main(){
    // create child process
    let mut gui_child = match Command::new("gui.exe").spawn() {
        Ok(child) => child,
        Err(_) => {
            eprintln!("#102");
            eprintln!("FAILED: Cannot start GUI!");
            return;
        }
    };
    let mut kernel_child = match Command::new("kernel.exe")
        .stdin(Stdio::piped())
        .stdout(Stdio::piped())
        .stderr(Stdio::inherit())
        .spawn()
    {
        Ok(child) => child,
        Err(_) => {
            eprintln!("#103");
            eprintln!("FAILED: Cannot start kernel!");
            return;
        }
    };

    // create IPC bridge server
    let server = match ServerOptions::new().create(r"\\.\pipe\swufe_netool") {
        Ok(server) => {
            // wait until gui connect this.
            match &server.connect().await {
                Ok(_) => server,
                Err(_) => {
                    eprintln!("#101");
                    eprintln!("FAILED: Cannot connect cilent!");
                    let _ = kernel_child.kill().await;
                    let _ = gui_child.kill().await;
                    return;
                }
            }
        }
        Err(_) => {
            eprintln!("#100");
            eprintln!("FAILED: Cannot create pipe!");
            let _ = kernel_child.kill().await;
            let _ = gui_child.kill().await;
            return;
        }
    };

    // get kernel stdin and stdout
    let kernel_stdin = match kernel_child.stdin.take() {
        Some(stdin) => stdin,
        None => {
            eprintln!("#104");
            eprintln!("FAILED: Cannot obtain console stdin!");
            let _ = kernel_child.kill().await;
            let _ = gui_child.kill().await;
            return;
        }
    };
    let kernel_stdout = match kernel_child.stdout.take() {
        Some(stdout) => stdout,
        None => {
            eprintln!("#105");
            eprintln!("FAILED: Cannot obtain console stdout!");
            let _ = kernel_child.kill().await;
            let _ = gui_child.kill().await;
            return;
        }
    };

    // build the bridge between gui and pipe and kernel
    let (pipe_reader, pipe_writer) = io::split(server);
    let gui_to_console = tokio::spawn(async move {
        let mut pipe_reader = pipe_reader;
        let mut child_stdin = kernel_stdin;
        io::copy(&mut pipe_reader, &mut child_stdin).await
    });
    let console_to_gui = tokio::spawn(async move {
        let mut child_stdout = kernel_stdout;
        let mut pipe_writer = pipe_writer;
        io::copy(&mut child_stdout, &mut pipe_writer).await
    });

    // actions after disconnection
    tokio::select! {
        result = gui_to_console => {
            match result {
                Ok(Ok(_)) => (),
                _ => {
                    eprintln!("#106");
                    eprintln!("FAILED: IPC IO ERROR in gui->console!");
                    let _ = kernel_child.kill().await;
                    let _ = gui_child.kill().await;
                    return;
                }
            }
        }
        result = console_to_gui => {
            match result {
                Ok(Ok(_)) => (),
                _ => {
                    eprintln!("#107");
                    eprintln!("FAILED: IPC IO ERROR in console->gui!");
                    let _ = kernel_child.kill().await;
                    let _ = gui_child.kill().await;
                    return;
                }
            }
        }
    }
}