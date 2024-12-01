
import { ReadConfigRequest, ReadConfigResponse } from "../ReadConfig";
import { config } from "./init";
export function mock(req: ReadConfigRequest): ReadConfigResponse {
  return config;
}
