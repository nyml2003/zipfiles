import { UpdateConfigRequest, UpdateConfigResponse } from "../UpdateConfig";
import { config } from "./init";
export function mock(req: UpdateConfigRequest): UpdateConfigResponse {
  if (req.defaultBackupPath) config.defaultBackupPath = req.defaultBackupPath;
  return {};
}
