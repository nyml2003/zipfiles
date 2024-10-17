import React, { useEffect } from 'react';
// import {
//   ArrowLeftOutlined,
//   HomeOutlined,
//   Loading3QuartersOutlined,
// } from '@ant-design/icons';
// import { Breadcrumb, Button, Splitter } from 'antd';
// import { useDispatch, useSelector } from 'react-redux';
// import { RootState } from '@/stores/store';
// import { BreadcrumbItemType } from 'antd/es/breadcrumb/Breadcrumb';
import TreeMenu from './TreeMenu';
import TableView from './TableView';
import { Splitter } from 'antd';
import { CommitLog } from '@/apis/GetCommitList';
import { NestedFileDetail } from '@/types';
import useApi from '@/hooks/useApi';
import { GetCommitDetailRequest, GetCommitDetailResponse } from '@/apis/GetCommitDetail';
import { ApiEnum } from '@/apis';
import { FileDetail } from '@/apis/GetAllFileDetails';

function nestify(files: FileDetail[]): NestedFileDetail {
  const newFiles = files.sort( (a, b) => a.path.localeCompare(b.path));
  const root : NestedFileDetail = { children: [], ...newFiles[0] };
  for (const file of newFiles) {
    const pathParts = file.path.split('/');
    let currentNode = root;
    if (currentNode.children === null) {
      continue;
    }
    for (let i = 1; i < pathParts.length; i++) {
      const existingChild = currentNode.children!.find(child => child.path === pathParts[i]);
      if (existingChild) {
        currentNode = existingChild;
      } else {
        const newChild = { path: pathParts[i], children: [] };
        currentNode.children!.push( 
        currentNode = newChild;
      }
    }
  }

  return root;
}

const Explorer: React.FC = (uuid: string, commitLog: CommitLog) => {
  // const isFiltering = useSelector((state: RootState) => state.createCommit.isFiltering);
  // const currentPath = useSelector((state: RootState) => state.createCommit.currentPath);
  // const dispatch = useDispatch();
  const [currentFile, setCurrentFile] = React.useState<string>('');
  const [currentPath, setCurrentPath] = React.useState<string>('');
  const [files, setFiles] = React.useState<NestedFileDetail[] | null>(null);
  const api = useApi();
  useEffect(() => {
    api
      .request<GetCommitDetailRequest, GetCommitDetailResponse>(ApiEnum.GetCommitDetail, { uuid })
      .then(res => {
        setFiles(res.files);
        setCurrentPath('/');
      });
  }, []);

  return (
    <div className='split-container-col grow-item'>
      <div className='flex rounded-xl items-center justify-between bg-gray-100'>
        {/* <div className='flex p-2 items-center'>
          <Button
            type='text'
            icon={<ArrowLeftOutlined />}
            onClick={() =>
              dispatch(updateCurrentPath(currentPath.split('/').slice(0, -1).join('/')))
            }></Button>
          <Button
            type='text'
            onClick={() => dispatch(handleRefresh())}
            icon={<Loading3QuartersOutlined />}
            disabled={isFiltering}></Button>
          <Breadcrumb
            items={currentPath.split('/').reduce((acc, item, index, arr) => {
              const path = arr.slice(0, index + 1).join('/');
              acc.push({
                title: item === '' ? <HomeOutlined /> : item,
                onClick: () => dispatch(updateCurrentPath(path)),
                className: 'cursor-pointer px-2 py-1 rounded hover:bg-gray-200',
              });
              return acc;
            }, [] as BreadcrumbItemType[])}
          />
        </div> */}
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
      </div>
    </div>
  );
};

export default Explorer;
