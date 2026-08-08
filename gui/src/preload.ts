import { contextBridge, ipcRenderer } from 'electron'

type Info = {
    isp: string,
    account: string,
    password: string
}

contextBridge.exposeInMainWorld('conn', {
    submit: (info:Info) => ipcRenderer.invoke('submit',info)
})