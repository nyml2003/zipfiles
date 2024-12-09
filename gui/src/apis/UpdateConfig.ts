type SoftwareConfiguration = {
  defaultBackupPath: string; // 默认备份路径
};
export interface UpdateConfigRequest extends Partial<SoftwareConfiguration> {}

export interface UpdateConfigResponse {}
