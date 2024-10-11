import { ApiEnum } from '@/apis';
import { GetFileDetailRequest, GetFileDetailResponse } from '@/apis/GetFileDetail';
import LoadingWrapper from '@/components/LoadingWrapper';
import useApi from '@/hooks/useApi';
import { RootState } from '@/stores/store';
import { FileTypeToString, LoadingState } from '@/types';
import React from 'react';
import { useEffect, useState } from 'react';
import { useSelector } from 'react-redux';



const MetaDataCard = () => {
  const [metadata, setMetadata] = useState<GetFileDetailResponse | null>(null);
  const api = useApi();
  const [loading, setLoading] = useState<LoadingState>(LoadingState.Done);
  const currentFile = useSelector((state: RootState) => state.file.currentFile);

  // useEffect(() => {
  //   if (currentFile) {
  //     setLoading(LoadingState.Loading);
  //     api
  //       .request<GetFileDetailRequest, GetFileDetailResponse>(ApiEnum.GetFileDetail, {
  //         path: currentFile === '' ? '/' : currentFile,
  //       })
  //       .then((res: GetFileDetailResponse) => {
  //         setMetadata(res);
  //         setLoading(LoadingState.Done);
  //       })
  //       .catch(e => {
  //         console.log('获取文件详情失败: ', e);
  //       });
  //   }
  // }, [currentFile]);

  useEffect(() => {
    api
        .request<GetFileDetailRequest, GetFileDetailResponse>(ApiEnum.GetFileDetail, {
          path: currentFile === '' ? '/' : currentFile,
        })
        .then((res: GetFileDetailResponse) => {
          setMetadata(res);
          setLoading(LoadingState.Done);
        })
        .catch(e => {
          console.log('获取文件详情失败: ', e);
        });
  }, [currentFile]);

  return (
    <LoadingWrapper
      loading={loading}
      hasData={() => !!metadata}
      children={
        metadata && (
          <div>
            <h1>文件详情</h1>
            <p> 路径: {metadata.path}</p>
            <p>文件名: {metadata.name}</p>
            <p>文件类型: {FileTypeToString(metadata.type)}</p>
            <p>创建时间: {new Date(metadata.createTime * 1000).toLocaleString()}</p>
            <p>更新时间: {new Date(metadata.updateTime * 1000).toLocaleString()}</p>
            <p>大小: {metadata.size}</p>
            <p>所有者: {metadata.owner}</p>
            <p>组: {metadata.group}</p>
            <p>权限: {metadata.mode}</p>
          </div>
        )
      }></LoadingWrapper>
  );
};

export default MetaDataCard;
