import { CreateNullFolderRequest, CreateNullFolderResponse } from "../CreateNullFolder";
import { cachedFileRoot } from "./init";
import { createNullFolder } from "./utils";

export function mock(req: CreateNullFolderRequest): CreateNullFolderResponse {
    createNullFolder(cachedFileRoot, req.path, req.newFolderName);
    return {};
}
