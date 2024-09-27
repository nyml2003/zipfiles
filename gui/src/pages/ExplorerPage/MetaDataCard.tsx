import { ApiEnum } from '@/apis';
import { FileTypeToString, GetFileDetailRequest, GetFileDetailResponse } from '@/apis/GetFileDetail';
import useApi from '@/hooks/useApi';
import React from 'react';
import { useEffect, useState } from 'react';

interface Props {
  currentFile: string;
}

const MetaDataCard: React.FC<Props> = ({ currentFile }) => {
  const [metadata, setMetadata] = useState<GetFileDetailResponse['metadata'] | null>(null);
  const api = useApi();

  useEffect(() => {
    if (currentFile) {
      api
        .request<GetFileDetailRequest, GetFileDetailResponse>(ApiEnum.GetFileDetail, {
          path: currentFile,
        })
        .then((res: GetFileDetailResponse) => {
          console.log(res);
          setMetadata(res.metadata);
        });
    }
  }, [currentFile]);

  return (
    <div>
      {metadata ? (
        <div>
          <p> 路径: {metadata.path}</p>
          <p>文件名: {metadata.name}</p>
          <p>文件类型: {FileTypeToString(metadata.type)}</p>
          <p>创建时间: {metadata.createTime.toString()}</p>
          <p>更新时间: {metadata.updateTime.toString()}</p>
          <p>大小: {metadata.size}</p>
          <p>所有者: {metadata.owner}</p>
          <p>组: {metadata.group}</p>
          <p>权限: {metadata.mode}</p>
        </div>
      ) : (
        <p>暂无数据</p>
      )}
    </div>
  );
};

export default MetaDataCard;
