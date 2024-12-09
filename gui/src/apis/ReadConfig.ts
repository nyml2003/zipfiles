type SoftwareConfiguration = {
  defaultBackupPath: string; // 默认备份路径
};
export interface ReadConfigRequest { }

export interface ReadConfigResponse extends SoftwareConfiguration { }

