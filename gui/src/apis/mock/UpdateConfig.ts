import { UpdateConfigRequest, UpdateConfigResponse } from "../UpdateConfig";
import { config } from "./init";
export function mock(req: UpdateConfigRequest): UpdateConfigResponse {
  if (req.ip) config.ip = req.ip;
  if (req.defaultBackupPath) config.defaultBackupPath = req.defaultBackupPath;
  return {};
}
