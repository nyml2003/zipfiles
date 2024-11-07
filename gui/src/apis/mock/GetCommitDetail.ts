import { GetCommitDetailRequest, GetCommitDetailResponse } from '../GetCommitDetail';
import { backups, cachedCommitList } from './utils';

export function mock(request: GetCommitDetailRequest): GetCommitDetailResponse {
  // const files = backups.get(request.uuid);
  // if (!files) {
  //   throw new Error('no such commit');
  // }
  return {
    files: [
        {
          createTime: 1730533246.0,
          group: 'root',
          mode: 33279,
          name: 'small_picture.jpg',
          owner: 'root',
          path: 'picture',
          size: 550070,
          type: 1,
          updateTime: 1675767711.0,
        },
        {
          createTime: 1730533246.0,
          group: 'root',
          mode: 33279,
          name: 'small_picture.png',
          owner: 'root',
          path: 'picture',
          size: 302514,
          type: 1,
          updateTime: 1688438810.0,
        },
        {
          createTime: 1730533246.0,
          group: 'root',
          mode: 33279,
          name: 'small_video.mp4',
          owner: 'root',
          path: 'video',
          size: 97844606,
          type: 1,
          updateTime: 1722764481.0,
        },
      ],
  };
}
