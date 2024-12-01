type SoftwareConfiguration = {
  ip: string; // 服务器ip
  defaultBackupPath: string; // 默认备份路径
};
export interface ReadConfigRequest { }

export interface ReadConfigResponse extends SoftwareConfiguration { }

