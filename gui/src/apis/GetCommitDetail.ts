import { FileDetail } from './GetAllFileDetails';

export interface GetCommitDetailRequest {
    uuid: string;
}
    
export interface GetCommitDetailResponse {
    files: FileDetail[];
}
