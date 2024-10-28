import React, { useContext, useEffect, useMemo, useState } from 'react';
import {
  ArrowLeftOutlined,
  ClearOutlined,
  HomeOutlined,
  Loading3QuartersOutlined,
} from '@ant-design/icons';
import { Breadcrumb, Button, Splitter } from 'antd';
import { BreadcrumbItemType } from 'antd/es/breadcrumb/Breadcrumb';
import TreeMenu from './TreeMenu';
import TableView from './TableView';
import { ApiEnum } from '@/apis';
import useApi from '@/hooks/useApi';
import { Context } from '../store/context';
import { GetCommitDetailResponse, GetCommitDetailRequest } from '@/apis/GetCommitDetail';

const Explorer: React.FC = () => {
  const { state, actions } = useContext(Context);
  const { path, commitId } = state;
  const api = useApi();

  const fetchData = async () => {
    const res = await api.request<GetCommitDetailRequest, GetCommitDetailResponse>(
      ApiEnum.GetCommitDetail,
      {
        uuid: commitId,
      },
    );
    actions.updateFiles({ payload: res.files });
  };

  useEffect(() => {
    fetchData();
  }, []);

  const breadcrumbItems = useMemo(() => {
    const rootLength = state.files.path.split('/').length;
    return state.path.split('/').reduce((acc, item, index, arr) => {
      const path = arr.slice(0, index + 1).join('/');
      acc.push({
        title: item === '' ? <HomeOutlined /> : item,
        onClick: index > rootLength - 2 ? () => actions.updatePath({ payload: path }) : undefined,
        className: `px-2 py-1 rounded ${
          index > rootLength - 2 ? 'cursor-pointer hover:bg-gray-200 ' : 'cursor-not-allowed'
        }`,
      });
      return acc;
    }, [] as BreadcrumbItemType[]);
  }, [state.path]);

  return (
    <div className='split-container-col grow-item'>
      <div className='flex rounded-xl items-center justify-between bg-gray-100'>
        <div className='flex p-2 items-center'>
          <Button
            type='text'
            icon={<ArrowLeftOutlined />}
            onClick={() =>
              actions.updatePath({ payload: path.split('/').slice(0, -1).join('/') })
            }></Button>
          <Button
            type='text'
            onClick={() => {
              console.log('刷新');
            }}
            icon={<Loading3QuartersOutlined />}></Button>
          <Button
            type='text'
            onClick={() => actions.updatePath({ payload: '' })}
            icon={<ClearOutlined />}></Button>
          <Breadcrumb items={breadcrumbItems} />
        </div>
        <div>
          <Button
            type='text'
            onClick={() => {
              console.log(state);
            }}>
            打印
          </Button>
        </div>
      </div>
      <div
        className='
        bg-white
        rounded-xl
        m-2
        p-2
        fade-in-down
        grow-item 
        split-container-row
        '>
        {
          <Splitter className='fade-in-down split-container-row grow-item'>
            <Splitter.Panel
              defaultSize='20%'
              min='10%'
              max='70%'
              className='split-container-row grow-item'>
              <TreeMenu />
            </Splitter.Panel>
            <Splitter.Panel className='split-container-row grow-item'>
              <TableView />
            </Splitter.Panel>
          </Splitter>
        }
      </div>
    </div>
  );
};

export default Explorer;
