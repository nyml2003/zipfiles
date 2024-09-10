export interface GetFileListRequest {
    path: string
 }

export interface GetFileListResponse {
    files: {
        name: string
        type: string
    }[]
}