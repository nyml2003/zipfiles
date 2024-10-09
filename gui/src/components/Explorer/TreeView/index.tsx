import { Splitter } from 'antd';
import React from 'react';
import MetaDataCard from './MetaDataCard';
import TreeMenu from './TreeMenu';
import styles from '../index.module.less';

const TreeView = () => {
  return (
    <Splitter className={`flex-grow overflow-hidden ${styles['fade-in-down']}`}>
      <Splitter.Panel
        defaultSize='20%'
        min='10%'
        max='70%'
        className='overflow-y-auto overflow-x-hidden bg-white flex '>
        <TreeMenu />
      </Splitter.Panel>
      <Splitter.Panel>
        {/* 文件信息 */}
        <MetaDataCard />
      </Splitter.Panel>
    </Splitter>
  );
};

export default TreeView;
